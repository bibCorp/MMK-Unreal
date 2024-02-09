// Copyright 2023 bibCorp. All Rights Reserved.


#include "MusicPlaybackHandle.h"
#include "MusicFile.h"
#include "ModularMusicTrack.h"
#include <Components/AudioComponent.h>
#include <Kismet/GameplayStatics.h>
#include "Quartz/QuartzSubsystem.h"
#include "Sound/QuartzQuantizationUtilities.h"


void UMusicPlaybackHandle::PlayMusicFile(UQuartzClockHandle* QuartzClock, UMusicFile* Music, float StartTime)
{
	PlayMusicFileInternal(QuartzClock, Music, StartTime, 0);
}

void UMusicPlaybackHandle::FadeInMusicFile(UQuartzClockHandle* QuartzClock, UMusicFile* Music, float StartTime, float FadeDuration)
{
	PlayMusicFileInternal(QuartzClock, Music, StartTime, FadeDuration);
}

void UMusicPlaybackHandle::PlayMusicFileInternal(UQuartzClockHandle* QuartzClock, UMusicFile* Music, float StartTime, float FadeDuration)
{
	MusicFile = Music;
	AudioComponents.Empty();
	
	FQuartzQuantizationBoundary QuartzQuantizationBoundary{
		EQuartzCommandQuantization::Tick, 1, EQuarztQuantizationReference::CurrentTimeRelative
	};

	if (MusicFile->IsModularMusicTrack() && MusicFile->GetMusicTrack())
	{
		auto Channels = MusicFile->GetMusicTrack()->GetChannels();

		// go through each channel and play its sound file
		for (int i = 0; i < Channels.Num(); i++)
		{
			// if this channel is disabled, we still set up a new AudioComponent but we override volume to 0
			float Volume = (Channels[i]->IsEnabled()) ? Channels[i]->GetVolume() * MusicFile->GetVolume() : 0.f;
			auto NewAudioComponment = UGameplayStatics::CreateSound2D(GetOuter(), Channels[i]->GetSound(), Volume, 1.f,
			                                                          StartTime, nullptr, true, false);
			AudioComponents.Add(NewAudioComponment);
			NewAudioComponment->PlayQuantized(this, QuartzClock, QuartzQuantizationBoundary, OnQuartzCommandEventBP,
			                                  StartTime, FadeDuration, Volume, EAudioFaderCurve::SCurve);
		}
	}
	else
	{
		auto NewAudioComponment = UGameplayStatics::CreateSound2D(GetOuter(), Music->GetSoundFile(),
		                                                          MusicFile->GetVolume(),
		                                                          1.f, StartTime, nullptr, true, false);
		AudioComponents.Add(NewAudioComponment);
		NewAudioComponment->PlayQuantized(this, QuartzClock, QuartzQuantizationBoundary, OnQuartzCommandEventBP,
		                                  StartTime, FadeDuration, MusicFile->GetVolume(), EAudioFaderCurve::SCurve);
	}

	Active = true;
}

void UMusicPlaybackHandle::Stop()
{
	for (auto AudioComponent : AudioComponents)
		AudioComponent->Stop();

	// Set a Timestamp for when the track was fully stopped.
	StoppedTimestamp = GetWorld()->TimeSeconds;

	Active = false;
}

void UMusicPlaybackHandle::FadeOut(float pFadeDuration)
{
	for (auto AudioComponment : AudioComponents)
		AudioComponment->FadeOut(pFadeDuration, 0.0f, EAudioFaderCurve::SCurve);

	// Set a Timestamp for when the fade is expected to be completed.
	// This can be used to later check if the music volume has fully faded to 0 before deleting the file.
	StoppedTimestamp = GetWorld()->TimeSeconds + pFadeDuration;

	Active = false;
}

float UMusicPlaybackHandle::GetDuration()
{
	if (!AudioComponents.IsEmpty() && AudioComponents[0] != nullptr)
		return AudioComponents[0]->GetSound()->GetDuration();

	return 0.f;
}

bool UMusicPlaybackHandle::IsFadeOutCompleted()
{
	if (StoppedTimestamp > 0)
		return (StoppedTimestamp <= GetWorld()->TimeSeconds);

	return false;
}
