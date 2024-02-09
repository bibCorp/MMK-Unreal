// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Tickable.h"
#include "Sound/SoundCue.h"
#include "ModularMusicEngine.generated.h"


class UMusicFile;
class UModularMusicTrack;


UENUM(BlueprintType)
enum class EPlaybackMode : uint8
{
	EPM_Solo UMETA(DisplayName = "Solo"),
	EPM_Playlist UMETA(DisplayName = "Playlist"),
	EPM_PlaybackQueued UMETA(DisplayName = "Queued"),
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMusicFinishedPlayingSignature, EPlaybackMode, PlaybackMode);


UCLASS()
class MODULARMUSICKIT_API UModularMusicEngine : public UEngineSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Modular Music Kit|Music Engine")
	FOnMusicFinishedPlayingSignature OnMusicFinishedPlayingDelegate;

private:
	bool bPlaybackActive;

	UPROPERTY()
	UMusicFile* CurrentMusicFile;
	UPROPERTY()
	class UMusicPlaybackHandle* CurrentMusicHandle;
	UPROPERTY()
	class UModularMusicPlaylist* CurrentPlaylist;
	EPlaybackMode PlaybackMode;
	FTimerHandle TimerHandle_FadeCurrentTrack;
	FTimerHandle TimerHandle_PlaybackFinished;
	UPROPERTY()
	UMusicFile* QueuedMusicFile;
	UPROPERTY()
	TArray<class UMusicPlaybackHandle*> MusicHandles;
	float PlaybackTime;
	const FName QuartzClockName = "MMK_Clock";

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// When a new Play In Editor session starts, a lot of variables need to be reset
	void OnBeginPIE();

	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	bool IsMusicPlaybackActive() { return bPlaybackActive; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	bool HasActivePlaylist() { return CurrentPlaylist != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	bool GetTrack() { return CurrentPlaylist != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	void PlayMusicTrack(UModularMusicTrack* MusicTrack, float StartTime = 0.f);
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	void PlaySoundCue(USoundCue* SoundCue, float StartTime = 0.f);
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	void PlaySoundWave(USoundWave* SoundWave, float StartTime = 0.f);
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	void PlayPlaylist(UModularMusicPlaylist* Playlist);
	
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	void CrossFadeMusicTrack(UModularMusicTrack* MusicTrack, float StartTime);
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	void CrossFadeSoundCue(USoundCue* SoundCue, float StartTime);
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	void CrossFadeSoundWave(USoundWave* SoundWave, float StartTime);

	/**
	 * @brief Main 'Play' function that all other play functions use.
	 * If music is already playing, it will automatically smoothly fade out before the new music starts.
	 * @param MusicFile UMusicFile is a generic wrapper class that contains another sound file.
	 * See documentation for how to use UMusicFile, or use one of the other specialized play functions, 'PlayMusicTrack', 'PlaySoundCue', 'PlaySoundWave'.
	 * @param StartTime Default is 0, which plays from the beginning.
	 * If set to negative value (e.g. -1), the current playback time will be used for the new music.
	 * This is best used when seamlessly transitioning between 2 variations of the same music, without restarting the music from the begining.
	 * @param CrossFadeIn Intended to be used when seamlessly transitioning between 2 variations of the same music, without restarting the music from the begining.
	 * Also pay attention to 'StartTime'.
	 * @param PlaybackMode Should this music be played solo or as part of a Playlist?
	 */
	void PlayMusicFile(UMusicFile* MusicFile, float StartTime = 0.f, bool CrossFadeIn = false,
	                   EPlaybackMode PlaybackMode = EPlaybackMode::EPM_Solo);

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	void StopCurrentMusic();

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Engine")
	FORCEINLINE float GetCurrentPlaybackTime() const { return PlaybackTime; }
	
private:
	FString GetCurrentTrackInfo();

	// Removes handles that are no longer active and have completely faded out
	void CleanUpMusicHandles();
	// Whenever music is stopped, this function is used to fade the music out smoothly 
	void FadeOutCurrentMusic();

	UFUNCTION()
	void OnTrackFadeCompleted();
	UFUNCTION()
	void OnMusicFinishedPlaying();

#pragma region FTickableGameObject implementation
private:
	// Prevents the 'default object' version of UModularMusicEngine from ticking
	bool bAllowTicking = false;
public:
	virtual void Tick(float DeltaTime) override;

	virtual bool IsAllowedToTick() const override
	{
		return bAllowTicking;
	}

	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Always;
	}

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTickableThing, STATGROUP_Tickables);
	}

	virtual bool IsTickableWhenPaused() const
	{
		return true;
	}
#pragma endregion FTickableGameObject implementation
};
