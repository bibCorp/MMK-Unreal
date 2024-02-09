// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModularMusicPlayer.generated.h"


class UModularMusicTrack;
class UModularMusicPlaylist;
class USoundCue;


UENUM(BlueprintType)
enum class ESoundFiletype : uint8
{
	ESF_MusicTrack UMETA(DisplayName = "ModularMusicTrack"),
	ESF_SoundCue   UMETA(DisplayName = "SoundCue"),
	ESF_SoundWave  UMETA(DisplayName = "SoundWave"),
	ESF_Playlist   UMETA(DisplayName = "Playlist"),
};

UCLASS(Abstract)
class MODULARMUSICKIT_API AModularMusicPlayer : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Modular Music Kit|Music Player")
	bool bAutoplay;
	UPROPERTY(EditAnywhere, Category = "Modular Music Kit|Music Player")
	ESoundFiletype SoundFiletype;
	UPROPERTY(EditAnywhere, Category = "Modular Music Kit|Music Player")
	UModularMusicTrack* ModularMusicTrack;
	UPROPERTY(EditAnywhere, Category = "Modular Music Kit|Music Player")
	USoundCue* SoundCue;
	UPROPERTY(EditAnywhere, Category = "Modular Music Kit|Music Player")
	USoundWave* SoundWave;
	UPROPERTY(EditAnywhere, Category = "Modular Music Kit|Music Player")
	UModularMusicPlaylist* Playlist;

public:
	AModularMusicPlayer();

	// Play the music assigned in the MusicPlayer's details panel
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Music Player")
	void Play();

protected:
	virtual void BeginPlay() override;
};
