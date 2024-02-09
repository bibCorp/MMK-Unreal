// Copyright 2023 bibCorp. All Rights Reserved.


#include "ModularMusicEngine.h"
#include <ModularMusicTrack.h>
#include <ModularMusicPlaylist.h>
#include "MusicFile.h"
#include <MusicPlaybackHandle.h>
#include <Sound/SoundCue.h>
#include <Sound/SoundWave.h>
#include "AudioDevice.h"
#include <Components/AudioComponent.h>
#include <Kismet/GameplayStatics.h>
#include "AssetRegistry/AssetRegistryModule.h"
#include "Timermanager.h"
#include "Quartz/QuartzSubsystem.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "Engine.h"


// Console command for showing debugging info
const TAutoConsoleVariable<int32> DebugStatus(
	TEXT("bibCorp.MMK.Debug.DebugStatus"), 0, TEXT("1 = Show debug information"));


void UModularMusicEngine::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bAllowTicking = true;
	bPlaybackActive = false;
	PlaybackTime = 0.f;
}

void UModularMusicEngine::Deinitialize()
{
	Super::Deinitialize();
}

void UModularMusicEngine::OnBeginPIE()
{
	CurrentMusicFile = nullptr;
	CurrentMusicHandle = nullptr;
	CurrentPlaylist = nullptr;
	QueuedMusicFile = nullptr;
	MusicHandles.Empty();

	PlaybackMode = EPlaybackMode::EPM_Solo;
	bPlaybackActive = false;
	PlaybackTime = 0.f;

	OnMusicFinishedPlayingDelegate.Clear();
	TimerHandle_FadeCurrentTrack.Invalidate();
	TimerHandle_PlaybackFinished.Invalidate();
}

UWorld* UModularMusicEngine::GetWorld() const
{
	if (auto World = GEngine->GetCurrentPlayWorld())
		return World;

	// 5.2 incompatibility // TODO investigate
#if ENGINE_MINOR_VERSION > 1
	if (!IsRunningDedicatedServer())
		if (auto World = GEngine->GameViewport->GetWorld())
			return World;
#endif	

	checkNoEntry();
	UE_LOG(LogTemp, Error,
	       TEXT(
		       "UModularMusicEngine::GetWorld - Unable to locate valid GameWorld.\nThis can happen if you are for example running multiple worlds/levels simultaneously.\nPlease contact bibCorp Audio support if you need assistance."
	       ))
	return nullptr;
}

void UModularMusicEngine::PlayMusicTrack(UModularMusicTrack* MusicTrack, float StartTime)
{
	if (MusicTrack == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Null MusicTrack"));
		return;
	}

	UMusicFile* MusicFile = NewObject<UMusicFile>(this, MusicTrack->GetFName());
	MusicFile->SetMusicTrack(MusicTrack);
	PlayMusicFile(MusicFile, StartTime, false, EPlaybackMode::EPM_Solo);
}

void UModularMusicEngine::PlaySoundCue(USoundCue* SoundCue, float StartTime)
{
	if (SoundCue == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Null SoundCue"));
		return;
	}

	UMusicFile* MusicFile = NewObject<UMusicFile>(this, SoundCue->GetFName());
	MusicFile->SetSoundCue(SoundCue);
	PlayMusicFile(MusicFile, StartTime, false, EPlaybackMode::EPM_Solo);
}

void UModularMusicEngine::PlaySoundWave(USoundWave* SoundWave, float StartTime)
{
	if (SoundWave == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Null SoundWave"));
		return;
	}

	UMusicFile* MusicFile = NewObject<UMusicFile>(this, SoundWave->GetFName());
	MusicFile->SetSoundWave(SoundWave);
	PlayMusicFile(MusicFile, StartTime, false, EPlaybackMode::EPM_Solo);
}

void UModularMusicEngine::PlayPlaylist(UModularMusicPlaylist* Playlist)
{
	if (Playlist == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Null Playlist"));
		return;
	}

	CurrentPlaylist = Playlist;
	PlayMusicFile(Playlist->GetFirst(), 0.f, false, EPlaybackMode::EPM_Playlist);
}

void UModularMusicEngine::CrossFadeMusicTrack(UModularMusicTrack* MusicTrack, float StartTime)
{
	if (MusicTrack == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Null MusicTrack"));
		return;
	}

	UMusicFile* MusicFile = NewObject<UMusicFile>(this, MusicTrack->GetFName());
	MusicFile->SetMusicTrack(MusicTrack);
	PlayMusicFile(MusicFile, StartTime, true, EPlaybackMode::EPM_Solo);
}

void UModularMusicEngine::CrossFadeSoundCue(USoundCue* SoundCue, float StartTime)
{
	if (SoundCue == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Null SoundCue"));
		return;
	}

	UMusicFile* MusicFile = NewObject<UMusicFile>(this, SoundCue->GetFName());
	MusicFile->SetSoundCue(SoundCue);
	PlayMusicFile(MusicFile, StartTime, true, EPlaybackMode::EPM_Solo);
}

void UModularMusicEngine::CrossFadeSoundWave(USoundWave* SoundWave, float StartTime)
{
	if (SoundWave == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Null SoundWave"));
		return;
	}

	UMusicFile* MusicFile = NewObject<UMusicFile>(this, SoundWave->GetFName());
	MusicFile->SetSoundWave(SoundWave);
	PlayMusicFile(MusicFile, StartTime, true, EPlaybackMode::EPM_Solo);
}

void UModularMusicEngine::PlayMusicFile(UMusicFile* MusicFile, float StartTime, bool CrossFadeIn,
                                        EPlaybackMode NewPlaybackMode)
{
	CurrentMusicFile = MusicFile;

	// Validate the PlaybackMode
	// Only 'Solo' and 'Playlist' allowed
	switch (NewPlaybackMode)
	{
	case EPlaybackMode::EPM_Solo:
		CurrentPlaylist = nullptr;
		break;

	case EPlaybackMode::EPM_Playlist:
		if (!HasActivePlaylist())
		{
			UE_LOG(LogTemp, Error, TEXT("Requested to play music as part of a Playlist, but no Playlist is active"));
			return;
		}
		break;

	default:
		checkNoEntry();
		return;
	}

	// if music is already playing
	if (IsMusicPlaybackActive())
	{
		// fade out current music
		FadeOutCurrentMusic();

		// if new music should not cross fade in, and we don't already have music queued up
		if (!CrossFadeIn && PlaybackMode != EPlaybackMode::EPM_PlaybackQueued)
		{
			// queue new music for later, then return
			QueuedMusicFile = MusicFile;
			PlaybackMode = EPlaybackMode::EPM_PlaybackQueued;
			return;
		}
	}

	/*
	** no music is currently playing, or we are going to cross fade, so we go ahead and finish the playback
	*/

	PlaybackMode = NewPlaybackMode;
	PlaybackTime = StartTime;

	// we no longer wait for any music to fade out
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_FadeCurrentTrack);

	FQuartzTimeSignature QuartzTimeSignature;
	if (MusicFile->IsModularMusicTrack())
		QuartzTimeSignature = MusicFile->GetMusicTrack()->TimeSignature;
	// else
	// 	QuartzTimeSignature.BeatType = EQuartzTimeSignatureQuantization::ThirtySecondNote;

	FQuartzClockSettings QuartzClockSettings{QuartzTimeSignature, true};
	UQuartzClockHandle* QuartzClock = UQuartzSubsystem::Get(GetWorld())->CreateNewClock(
		this, QuartzClockName, QuartzClockSettings, true, true);
	if (!QuartzClock->IsClockRunning(this))
		QuartzClock->StartClock(this, QuartzClock);

	UMusicPlaybackHandle* NewMusicHandle = NewObject<UMusicPlaybackHandle>(this);
	if (CrossFadeIn)
		NewMusicHandle->FadeInMusicFile(QuartzClock, MusicFile, StartTime);
	else
		NewMusicHandle->PlayMusicFile(QuartzClock, MusicFile, StartTime);

	CurrentMusicHandle = NewMusicHandle;
	MusicHandles.Add(NewMusicHandle);

	// keep track of when music has finished playing
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_PlaybackFinished, this,
	                                       &UModularMusicEngine::OnMusicFinishedPlaying,
	                                       CurrentMusicFile->GetDuration() - StartTime);

	bPlaybackActive = true;
}

void UModularMusicEngine::FadeOutCurrentMusic()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_FadeCurrentTrack);

	bool bStartNewFade = false;
	for (auto MusicHandle : MusicHandles)
		if (MusicHandle->IsActive())
		{
			bStartNewFade = true;
			MusicHandle->SetActive(false);
			MusicHandle->FadeOut();
		}

	if (bStartNewFade)
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_FadeCurrentTrack, this,
		                                       &UModularMusicEngine::OnTrackFadeCompleted,
		                                       UMusicPlaybackHandle::FADE_DURATION);
}

void UModularMusicEngine::StopCurrentMusic()
{
	bPlaybackActive = false;
	PlaybackTime = 0.f;

	FadeOutCurrentMusic();
	CleanUpMusicHandles();

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_FadeCurrentTrack);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PlaybackFinished);
}

FString UModularMusicEngine::GetCurrentTrackInfo()
{
	return CurrentMusicFile->GetFileInfo();
}

void UModularMusicEngine::CleanUpMusicHandles()
{
	for (int i = MusicHandles.Num() - 1; i >= 0; i--)
		if (!MusicHandles[i]->IsActive() && MusicHandles[i]->IsFadeOutCompleted())
		{
			MusicHandles.Swap(i, MusicHandles.Num() - 1);
			MusicHandles.RemoveAt(MusicHandles.Num() - 1, 1, false);
		}
	MusicHandles.Shrink();
}

void UModularMusicEngine::OnTrackFadeCompleted()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_FadeCurrentTrack);

	if (bPlaybackActive)
		switch (PlaybackMode)
		{
		case EPlaybackMode::EPM_PlaybackQueued:
			if (QueuedMusicFile != nullptr)
			{
				bPlaybackActive = false;
				if (HasActivePlaylist())
					PlayMusicFile(QueuedMusicFile, 0.f, false, EPlaybackMode::EPM_Playlist);
				else
					PlayMusicFile(QueuedMusicFile, 0.f, false, EPlaybackMode::EPM_Solo);
				QueuedMusicFile = nullptr;
			}
			break;

		case EPlaybackMode::EPM_Solo:
		case EPlaybackMode::EPM_Playlist:
			StopCurrentMusic();
			CleanUpMusicHandles();
			break;

		default:
			checkNoEntry();
			break;
		}
}

void UModularMusicEngine::OnMusicFinishedPlaying()
{
	bPlaybackActive = false;
	PlaybackTime = 0.f;

	switch (PlaybackMode)
	{
	case EPlaybackMode::EPM_PlaybackQueued:
		break;

	case EPlaybackMode::EPM_Solo:
		if (CurrentMusicFile->IsLooping())
			PlayMusicFile(CurrentMusicFile);
		else
			StopCurrentMusic();
		break;

	case EPlaybackMode::EPM_Playlist:
		if (HasActivePlaylist())
		{
			// try to progress the Playlist
			if (CurrentPlaylist->SelectNext())
				PlayMusicFile(CurrentPlaylist->GetCurrent(), 0.f, false, EPlaybackMode::EPM_Playlist);
			else if (CurrentPlaylist->IsLooping())
				PlayPlaylist(CurrentPlaylist);
			else
				StopCurrentMusic();
		}
		break;
	}

	CleanUpMusicHandles();
	OnMusicFinishedPlayingDelegate.Broadcast(PlaybackMode);
}

void UModularMusicEngine::Tick(float DeltaTime)
{
	if (IsMusicPlaybackActive())
	{
		PlaybackTime += DeltaTime;
	}

	// only show debug information if DebugStatus has been set to 1 using the console (bibCorp.MMK.Debug.DebugStatus 1)
	if (DebugStatus.GetValueOnGameThread() == 1)
	{
		if (CurrentMusicFile)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(
				                                 TEXT("Current Music File = %s"), *CurrentMusicFile->GetName()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(
				                                 TEXT("Current Music File = ---")));
		}

		if (CurrentMusicHandle)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(
				                                 TEXT("Music Duration = %f"), CurrentMusicHandle->GetDuration()));
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(
				                                 TEXT("Current Music Handle = %s"), *CurrentMusicHandle->GetName()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(
				                                 TEXT("Music Duration = ---")));
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(
				                                 TEXT("Current Music Handle = ---")));
		}

		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(
			                                 TEXT("PlaybackTime = %f"), PlaybackTime));
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(
			                                 TEXT("PlaybackMode = %s"), *UEnum::GetValueAsString(PlaybackMode)));
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(
			                                 TEXT("bPlaybackActive = %hc"), (bPlaybackActive) ? *"True" : *"False"));
	}
}
