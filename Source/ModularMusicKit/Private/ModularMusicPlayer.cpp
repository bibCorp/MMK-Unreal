// Copyright 2023 bibCorp. All Rights Reserved.


#include "ModularMusicPlayer.h"
#include "ModularMusicEngine.h"


AModularMusicPlayer::AModularMusicPlayer()
{
	PrimaryActorTick.bCanEverTick = false;

	bAutoplay = true;
}

void AModularMusicPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoplay)
		Play();
}

void AModularMusicPlayer::Play()
{
	UModularMusicEngine* MMEngine = GEngine->GetEngineSubsystem<UModularMusicEngine>();

	switch (SoundFiletype)
	{
	case ESoundFiletype::ESF_MusicTrack:
		if (ModularMusicTrack == nullptr)
			UE_LOG(LogTemp, Error, TEXT("MusicPlayer is unable to play: no 'Music Track' has been assigned"))
		else
			MMEngine->PlayMusicTrack(ModularMusicTrack);
		break;

	case ESoundFiletype::ESF_SoundCue:
		if (SoundCue == nullptr)
			UE_LOG(LogTemp, Error, TEXT("MusicPlayer is unable to play: no 'SoundCue' has been assigned"))
		else
			MMEngine->PlaySoundCue(SoundCue);
		break;

	case ESoundFiletype::ESF_SoundWave:
		if (SoundWave == nullptr)
			UE_LOG(LogTemp, Error, TEXT("MusicPlayer is unable to play: no 'SoundWave' has been assigned"))
		else
			MMEngine->PlaySoundWave(SoundWave);
		break;

	case ESoundFiletype::ESF_Playlist:
		if (Playlist == nullptr)
			UE_LOG(LogTemp, Error, TEXT("MusicPlayer is unable to play: no 'Playlist' has been assigned"))
		else
			MMEngine->PlayPlaylist(Playlist);
		break;
	}
}
