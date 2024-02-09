// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/QuartzQuantizationUtilities.h"
#include "ModularMusicTrack.generated.h"


UCLASS(BlueprintType)
class MODULARMUSICKIT_API UModularMusicChannel : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Modular Music Kit|Music Channel")
	USoundBase* Sound;
	
private:
	UPROPERTY()
	bool bEnabled = true;
	UPROPERTY(meta=(ClampMin=0.1f, ClampMax=2.f))
	float Volume = 1.f;
	UPROPERTY()
	bool bSolo = false;

public:
	void Init(USoundBase* NewSound) { Sound = NewSound; }

	UModularMusicChannel* DeepCopy()
	{
		auto Channel = NewObject<UModularMusicChannel>(this, this->GetFName());
		Channel->Sound = this->Sound;
		Channel->bEnabled = this->bEnabled;
		Channel->Volume = this->Volume;
		return Channel;
	}

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Channel")
	void SetEnabled(bool Enabled) { bEnabled = Enabled; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Channel")
	void SetVolume(float NewVolume) { Volume = NewVolume; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Channel")
	void SetSolo(bool Solo) { bSolo = Solo; }
	
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Channel")
	bool IsEnabled() const { return bEnabled; }
	
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Channel")
	bool IsSolo() const { return bSolo; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Channel")
	USoundBase* GetSound() const { return Sound; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Channel")
	float GetVolume() const { return Volume; }
};

UCLASS(BlueprintType)
class MODULARMUSICKIT_API UModularMusicTrack : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Modular Music Kit|Music Track")
	bool bLoop = true;
	UPROPERTY(EditDefaultsOnly, Category = "Modular Music Kit|Music Track")
	float Volume = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Modular Music Kit|Music Track")
	TArray<UModularMusicChannel*> Channels;
	UPROPERTY(EditDefaultsOnly, Category = "Modular Music Kit|Music Track")
	bool bEditChannels = false;
	UPROPERTY(EditDefaultsOnly, Category = "Modular Music Kit|Music Track")
	int BPM = 120;
	UPROPERTY(EditDefaultsOnly, Category = "Modular Music Kit|Music Track")
	FQuartzTimeSignature TimeSignature;

public:
	void Apply(UModularMusicTrack* Other);
	bool Contains(USoundBase* Sound);
	bool CreateNewChannel(USoundBase* Sound);

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Track")
	UModularMusicTrack* DeepCopy();

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Track")
	bool GetLooping() const { return bLoop; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Track")
	float GetVolume() const { return Volume; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Track")
	TArray<UModularMusicChannel*> GetChannels() const { return Channels; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Track")
	TArray<UModularMusicChannel*> CopyChannels();
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Track")
	void SetLooping(bool Loop) { bLoop = Loop; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Track")
	void SetVolume(float NewVolume) { Volume = NewVolume; }

	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Track")
	float GetDuration() const;
};
