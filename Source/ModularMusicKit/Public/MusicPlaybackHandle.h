// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MusicPlaybackHandle.generated.h"


class UMusicFile;
class UQuartzClockHandle;
class UAudioComponent;


UCLASS()
class MODULARMUSICKIT_API UMusicPlaybackHandle : public UObject
{
	GENERATED_BODY()

public:
	inline static const float FADE_DURATION = 2.f;

private:
	UPROPERTY()
	UMusicFile* MusicFile;
	UPROPERTY()
	TArray<UAudioComponent*> AudioComponents;
	bool Active = false;
	double StoppedTimestamp = 0.f;
	
	FName QuartzClockName = "MMK_Clock";
	FOnQuartzCommandEventBP OnQuartzCommandEventBP;

public:
	FORCEINLINE void SetActive(bool bActive) { Active = bActive; }
	FORCEINLINE bool IsActive() const { return Active; }

	/// <summary> Starts immediately. Can create noise/clicking if pStartTime is not 0. </summary>
	void PlayMusicFile(UQuartzClockHandle* QuartzClock, UMusicFile* Music, float StartTime);
	/// <summary> Fades in and prevents noise/clicking. </summary>
	void FadeInMusicFile(UQuartzClockHandle* QuartzClock, UMusicFile* Music, float StartTime, float FadeDuration = FADE_DURATION);
	/// <summary> Stops immediately. Can create noise/clicking. </summary>
	void Stop();
	/// <summary> Fades in and prevents noise/clicking. </summary>
	void FadeOut(float FadeDuration = FADE_DURATION);

	/// <summary> Check the internally stored 'StoppedTimestamp' to see if this track has been fully faded out/stopped. </summary>
	bool IsFadeOutCompleted();
	float GetDuration();

private:
	void PlayMusicFileInternal(UQuartzClockHandle* QuartzClock, UMusicFile* Music, float StartTime, float FadeDuration);
};
