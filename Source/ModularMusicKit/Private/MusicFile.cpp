// Copyright 2023 bibCorp. All Rights Reserved.


#include "MusicFile.h"
#include <Sound/SoundCue.h>
#include <Sound/SoundWave.h>
#include "ModularMusicTrack.h"
#include <Components/AudioComponent.h>


void UMusicFile::SetMusicTrack(UModularMusicTrack* NewMusicTrack)
{
	this->MusicTrack = NewMusicTrack;
	Volume = NewMusicTrack->Volume;
	bLoop = NewMusicTrack->bLoop;
}
void UMusicFile::SetSoundCue(USoundCue* NewSoundCue)
{
	this->SoundCue = NewSoundCue;
	Volume = NewSoundCue->VolumeMultiplier;
	bLoop = !NewSoundCue->IsOneShot();
}
void UMusicFile::SetSoundWave(USoundWave* NewSoundWave)
{
	this->SoundWave = NewSoundWave;
	Volume = NewSoundWave->Volume;
	bLoop = !NewSoundWave->IsOneShot();
}

UModularMusicTrack* UMusicFile::GetMusicTrack() const
{
	return MusicTrack;
}

USoundBase* UMusicFile::GetSoundFile() const
{
	if (SoundCue) return SoundCue;
	if (SoundWave) return SoundWave;
	return nullptr;
}

FString UMusicFile::GetFileInfo(const FString Prefix) const
{
	if (MusicTrack != nullptr) return FString(Prefix + "MusicTrack: " + MusicTrack->GetName());
	else if (SoundCue != nullptr) return FString(Prefix + "SoundCue: " + SoundCue->GetName());
	else if (SoundWave != nullptr) return FString(Prefix + "SoundWave: " + SoundWave->GetName());
	else return FString(Prefix + "missing file");
}

float UMusicFile::GetDuration() const
{
	if (IsModularMusicTrack())
		return MusicTrack->GetDuration();
	else
		return GetSoundFile()->GetDuration();
}