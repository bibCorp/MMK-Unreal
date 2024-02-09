// Copyright 2023 bibCorp. All Rights Reserved.


#include "ModularMusicPlaylist.h"
#include "ModularMusicTrack.h"
#include "MusicFile.h"
#include "Sound/SoundBase.h"


bool UModularMusicPlaylist::SelectNext()
{
	if (++MusicFileIndex == Playlist.Num())
	{
		if (bLoop)
			MusicFileIndex = 0;
		else
			return false;
	}
	return true;
}

UMusicFile* UModularMusicPlaylist::GetFirst()
{
	return Playlist[MusicFileIndex = 0];
}

UMusicFile* UModularMusicPlaylist::GetCurrent()
{
	return Playlist[MusicFileIndex];
}

bool UModularMusicPlaylist::Add(USoundBase* SoundFile)
{
	if (SoundFile == nullptr)
		return false;

	UMusicFile* NewMusicFile = NewObject<UMusicFile>(this, SoundFile->GetFName());
	if (USoundCue* SoundCue = dynamic_cast<USoundCue*>(SoundFile))
		NewMusicFile->SetSoundCue(SoundCue);
	else if (USoundWave* SoundWave = dynamic_cast<USoundWave*>(SoundFile))
		NewMusicFile->SetSoundWave(SoundWave);
	else
		return false;

	Playlist.Add(NewMusicFile);
	return true; 
}

bool UModularMusicPlaylist::Add(UModularMusicTrack* MusicTrack)
{
	if (MusicTrack == nullptr)
		return false;

	UMusicFile* NewMusicFile = NewObject<UMusicFile>(this, MusicTrack->GetFName());
	NewMusicFile->SetMusicTrack(MusicTrack);

	Playlist.Add(NewMusicFile);
	return true;
}
