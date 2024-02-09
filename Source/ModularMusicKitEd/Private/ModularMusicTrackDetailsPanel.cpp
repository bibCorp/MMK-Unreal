// Copyright 2023 bibCorp. All Rights Reserved.


#include "ModularMusicTrackDetailsPanel.h"
#include "ModularMusicTrack.h"
#include "ModularMusicKitEditorSubsystem.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Brushes/SlateImageBrush.h"
#include "PropertyEditing.h"
#include "EditorWidgetFunctionLibrary.h"
#include "Slate/SObjectWidget.h"
#include "SAssetDropTarget.h"
#include "SAssetView.h"


#define LOCTEXT_NAMESPACE "ModularMusicKitModule"


TSharedRef<IDetailCustomization> FModularMusicTrackDetailsPanel::MakeInstance()
{
	return MakeShareable(new FModularMusicTrackDetailsPanel);
}

void FModularMusicTrackDetailsPanel::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);
	MusicTrackRef = Cast<UModularMusicTrack>(Objects[0]);

	TSharedRef<IPropertyHandle> bLoop = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UModularMusicTrack, bLoop));
	TSharedRef<IPropertyHandle> Volume = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UModularMusicTrack, Volume));
	TSharedRef<IPropertyHandle> Channels = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UModularMusicTrack, Channels));
	TSharedRef<IPropertyHandle> bEditChannels = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UModularMusicTrack, bEditChannels));
	TSharedRef<IPropertyHandle> BPM = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UModularMusicTrack, BPM));
	TSharedRef<IPropertyHandle> TimeSignature = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UModularMusicTrack, TimeSignature));

	if (Objects.Num() == 1)
	{
		IDetailCategoryBuilder& CustomCategory = DetailBuilder.EditCategory("Modular Music Track");
		{
			DetailBuilder.HideProperty(bEditChannels);

			CustomCategory.AddCustomRow(FText::FromString("Modular Music Track"))
				.WholeRowContent()
				[
					SNew(SButton)
					.ToolTipText(FText::FromString("Open this Music Track for editing in the custom Music Track Editor window"))
				.Text(LOCTEXT("OpenEditorBtnText", "Open in Music Track Editor"))
				.OnClicked_Lambda([&]()
					{
						if (GEditor)
							GEditor->GetEditorSubsystem<UModularMusicKitEditorSubsystem>()->OpenModularMusicTrackEditor(MusicTrackRef);
						return FReply::Handled();
					})
				];

			CustomCategory.AddProperty(bLoop);
			CustomCategory.AddProperty(Volume);
			CustomCategory.AddProperty(BPM);
			CustomCategory.AddProperty(TimeSignature);

			if (MusicTrackRef->bEditChannels)
			{
				CustomCategory.AddProperty(Channels)
					.DisplayName(FText::FromString("Channels"));
			}
			else
			{
				CustomCategory.AddProperty(Channels)
					.ShowPropertyButtons(false)
					.IsEnabled(false)
					.DisplayName(FText::FromString("Channels (read-only)"));
			}

			CustomCategory.AddCustomRow(FText::FromString("Modular Music Track"), EPropertyLocation::Advanced)
				.WholeRowContent()
				[
					SNew(SButton)
					.Text(LOCTEXT("EditChannelsBtnText", "Add / Remove Channels"))
				.OnClicked_Lambda([&]()
					{
						MusicTrackRef->bEditChannels = !MusicTrackRef->bEditChannels;
						CustomCategory.GetParentLayout().ForceRefreshDetails();
						return FReply::Handled();
					})
				];

			if (MusicTrackRef->bEditChannels)
			{
				FString ImagePath = TEXT("/ModularMusicKit/Resources/Textures/drop-files.png");
				FName BrushName = FName(*ImagePath);

				CustomCategory.AddCustomRow(FText::FromString("Modular Music Track"), EPropertyLocation::Advanced)
					[
						SNew(SAssetDropTarget)
						.bSupportsMultiDrop(true)
					.OnAssetsDropped(this, &FModularMusicTrackDetailsPanel::ProcessDroppedAssets)
					[
						SNew(SBorder)
						.HAlign(HAlign_Left)
					.VAlign(VAlign_Fill)
					.Padding(FMargin(5.0f))
					[
						SNew(STextBlock)
						.MinDesiredWidth(450.f)
					.Justification(ETextJustify::Center)
					.Text(FText::FromString(TEXT("\n[ DROP FILES HERE TO ADD NEW CHANNELS ]\nsupported file types: SoundWave, SoundCue\n")))
					]
					]
					];
			}
		}
	}
	else
	{
		IDetailCategoryBuilder& CustomCategory = DetailBuilder.EditCategory("Modular Music Track");
		{
			CustomCategory.AddProperty(bLoop);
			CustomCategory.AddProperty(Volume);
			CustomCategory.AddProperty(BPM);
			CustomCategory.AddProperty(TimeSignature);
			DetailBuilder.HideProperty(Channels);
			DetailBuilder.HideProperty(bEditChannels);

			CustomCategory.AddCustomRow(FText::FromString("Modular Music Track"))
				.WholeRowContent()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Left)
				.Text(LOCTEXT("MultipleMusicTracksWarning", "Please edit single MusicTrack to see all settings"))
				];
		}
	}
}

void FModularMusicTrackDetailsPanel::ProcessDroppedAssets(const FDragDropEvent& Event, TArrayView<FAssetData> AssetDatas)
{
	for (auto Data : AssetDatas)
		if (auto Sound = dynamic_cast<USoundBase*>(Data.GetAsset()))
			MusicTrackRef->CreateNewChannel(Sound);
}

#undef LOCTEXT_NAMESPACE
