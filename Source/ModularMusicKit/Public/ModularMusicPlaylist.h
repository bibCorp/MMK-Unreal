// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ModularMusicPlaylist.generated.h"


class UMusicFile;


// Playlists are able to hold 3 different class types: UModularMusicTrack, USoundCue & USoundWave.
// Internally they are each wrapped inside a 'UMusicFile' to enable unified storage.

UCLASS()
class MODULARMUSICKIT_API UModularMusicPlaylist : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Modular Music Kit|Playlist")
	TArray<UMusicFile*> Playlist;
	UPROPERTY(EditDefaultsOnly, Category = "Modular Music Kit|Playlist")
	bool bLoop = false;

protected:
	int MusicFileIndex;

public:
	/**
	 * @brief Prepare the next music file, if possible. 
	 * @return FALSE if Playlist has finished and is not looping.
	 */
	bool SelectNext();
	bool IsLooping() { return bLoop; }
	UMusicFile* GetFirst();
	UMusicFile* GetCurrent();

	bool Add(USoundBase* SoundFile);
	bool Add(class UModularMusicTrack* MusicTrack);
};
