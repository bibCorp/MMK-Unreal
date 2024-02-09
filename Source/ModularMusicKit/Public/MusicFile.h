// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "MusicFile.generated.h"


class UModularMusicTrack;


UCLASS(BlueprintType)
class MODULARMUSICKIT_API UMusicFile : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY() UModularMusicTrack* MusicTrack = nullptr;
	UPROPERTY() USoundCue* SoundCue = nullptr;
	UPROPERTY() USoundWave* SoundWave = nullptr;

private:
	bool bLoop = false;
	float Volume = 1.f;

public:
	void SetMusicTrack(UModularMusicTrack* NewMusicTrack);
	void SetSoundCue(USoundCue* NewSoundCue);
	void SetSoundWave(USoundWave* NewSoundWave);

	bool IsModularMusicTrack() const { return MusicTrack != nullptr; }
	bool IsLooping() const { return bLoop; }
	float GetVolume() const { return Volume; }

	/// <returns> can return null </returns>
	UModularMusicTrack* GetMusicTrack() const;

	/// <returns> SoundCue or SoundWave, can be null </returns>
	USoundBase* GetSoundFile() const;

	FString GetFileInfo(const FString Prefix = "") const;
	float GetDuration() const;
};
