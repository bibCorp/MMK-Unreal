// Copyright 2023 bibCorp. All Rights Reserved.


#include "ModularMusicTrack.h"


void UModularMusicTrack::Apply(UModularMusicTrack* Other)
{
	this->bLoop = Other->bLoop;
	this->Volume = Other->Volume;
	this->Channels = Other->CopyChannels();
}

bool UModularMusicTrack::Contains(USoundBase* Sound)
{
	for (auto Channel : Channels)
		if (Channel->GetSound()->GetPathName() == Sound->GetPathName())
			return true;
	return false;
}

bool UModularMusicTrack::CreateNewChannel(USoundBase* Sound)
{
	if (Contains(Sound))
		return false;

	auto Channel = NewObject<UModularMusicChannel>(this, Sound->GetFName());
	Channel->Init(Sound);
	Channels.Add(Channel);
	return true;
}

UModularMusicTrack* UModularMusicTrack::DeepCopy()
{
	FString CopyName = GetFName().ToString() + "_copy";
	auto Copy = NewObject<UModularMusicTrack>(this, FName(*CopyName));
	Copy->Apply(this);
	return Copy;
}

TArray<UModularMusicChannel*> UModularMusicTrack::CopyChannels()
{
	TArray<UModularMusicChannel*> CopiedChannels;
	for (auto Channel : Channels)
		CopiedChannels.Add(Channel->DeepCopy());
	return CopiedChannels;
}

float UModularMusicTrack::GetDuration() const
{
	float Duration = 0.f;
	for (auto Channel : Channels)
	{
		float f = Channel->GetSound()->GetDuration();
		if (f > Duration)
			Duration = f;
	}

	return Duration;
}
