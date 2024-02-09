// Copyright 2023 bibCorp. All Rights Reserved.


#include "ModularMusicPlayerDetailsPanel.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/Text/STextBlock.h"
#include "PropertyEditing.h"
#include "ModularMusicPlayer.h"

#define LOCTEXT_NAMESPACE "ModularMusicKitModule"

TSharedRef<IDetailCustomization> FModularMusicPlayerDetailsPanel::MakeInstance()
{
	return MakeShareable(new FModularMusicPlayerDetailsPanel);
}

void FModularMusicPlayerDetailsPanel::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TSharedRef<IPropertyHandle> bAutoplay = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AModularMusicPlayer, bAutoplay));
	TSharedRef<IPropertyHandle> SoundFiletype = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AModularMusicPlayer, SoundFiletype));
	TSharedRef<IPropertyHandle> ModularMusicTrack = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AModularMusicPlayer, ModularMusicTrack));
	TSharedRef<IPropertyHandle> SoundCue = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AModularMusicPlayer, SoundCue));
	TSharedRef<IPropertyHandle> SoundWave = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AModularMusicPlayer, SoundWave));
	TSharedRef<IPropertyHandle> Playlist = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AModularMusicPlayer, Playlist));


	FSimpleDelegate OnFiletypeChanged = FSimpleDelegate::CreateLambda([&DetailBuilder]
		{
			DetailBuilder.ForceRefreshDetails();
		});
	SoundFiletype->SetOnPropertyValueChanged(OnFiletypeChanged);


	IDetailCategoryBuilder& CustomCategory = DetailBuilder.EditCategory("Modular Music Player");
	{
		CustomCategory.AddProperty(bAutoplay);

		TArray<TWeakObjectPtr<UObject>> Objects;
		DetailBuilder.GetObjectsBeingCustomized(Objects);
		if (Objects.Num() == 1)
		{
			CustomCategory.AddProperty(SoundFiletype);

			DetailBuilder.HideProperty(ModularMusicTrack);
			DetailBuilder.HideProperty(SoundCue);
			DetailBuilder.HideProperty(SoundWave);
			DetailBuilder.HideProperty(Playlist);

			TWeakObjectPtr<AModularMusicPlayer> MyObject = Cast<AModularMusicPlayer>(Objects[0].Get());

			switch (MyObject->SoundFiletype)
			{
			case ESoundFiletype::ESF_MusicTrack:
				CustomCategory.AddProperty(ModularMusicTrack);
				break;
			case ESoundFiletype::ESF_SoundCue:
				CustomCategory.AddProperty(SoundCue);
				break;
			case ESoundFiletype::ESF_SoundWave:
				CustomCategory.AddProperty(SoundWave);
				break;
			case ESoundFiletype::ESF_Playlist:
				CustomCategory.AddProperty(Playlist);
				break;
			default:
				break;
			}
		}
		else
		{
			DetailBuilder.HideProperty(SoundFiletype);
			DetailBuilder.HideProperty(ModularMusicTrack);
			DetailBuilder.HideProperty(SoundCue);
			DetailBuilder.HideProperty(SoundWave);
			DetailBuilder.HideProperty(Playlist);

			CustomCategory.AddCustomRow(FText::FromString("Modular Music Player"))
				.WholeRowContent()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Multiple objects warning", "Please edit single object to see all settings"))
				];

		}
	}
}


#undef LOCTEXT_NAMESPACE
