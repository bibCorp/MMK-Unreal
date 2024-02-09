// Copyright 2023 bibCorp. All Rights Reserved.


#include "ModularMusicPlaylistDetailsPanel.h"
#include "ModularMusicPlaylist.h"
#include "ModularMusicTrack.h"
#include "MusicFile.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Brushes/SlateImageBrush.h"
#include "Widgets/SBoxPanel.h"
#include "PropertyEditing.h"
#include "UObject/Class.h"
#include "Slate/SObjectWidget.h"
#include "SAssetDropTarget.h"
#include "SAssetView.h"
#include <PropertyCustomizationHelpers.h>
#include "Widgets/Layout/SSeparator.h"
#include <Widgets/Layout/SHeader.h>
#include <Styling/SlateIconFinder.h>
#include "PlaylistStyle.h"


#define LOCTEXT_NAMESPACE "ModularMusicKitModule"


TSharedRef<IDetailCustomization> FModularMusicPlaylistDetailsPanel::MakeInstance()
{
	return MakeShareable(new FModularMusicPlaylistDetailsPanel);
}

void FModularMusicPlaylistDetailsPanel::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);
	PlaylistRef = Cast<UModularMusicPlaylist>(Objects[0]);

	TSharedRef<IPropertyHandle> bLoop = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UModularMusicPlaylist, bLoop));
	TSharedRef<IPropertyHandle> Playlist = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UModularMusicPlaylist, Playlist));

	if (Objects.Num() == 1)
	{
		IDetailCategoryBuilder& CustomCategory = DetailBuilder.EditCategory("Playlist");
		{
			/* /////  Playlist Settings header  ///// */
			{
				CustomCategory.AddCustomRow(FText::FromString("Playlist"))
					[
						SNew(SHeader).Content()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("Playlist Settings")))
						]
					];
			}

			/* /////  loop check box  ///// */
			{
				CustomCategory.AddCustomRow(FText::FromString("Playlist"))
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					.MaxWidth(100.f)
					[
						SNew(STextBlock)
						.MinDesiredWidth(50.f)
					.Justification(ETextJustify::Left)
					.Text(FText::FromString(TEXT("Loop")))
					]

				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					[
						SNew(SCheckBox)
						.HAlign(HAlign_Left)
					.IsChecked(PlaylistRef->bLoop)
					.OnCheckStateChanged_Lambda([&](ECheckBoxState State)
						{
							PlaylistRef->bLoop = (State == ECheckBoxState::Checked) ? true : false;
						})
					]
					];
			}

			/* /////  hide default properties  ///// */
			DetailBuilder.HideProperty(bLoop);
			DetailBuilder.HideProperty(Playlist);

			/* /////  drop assets header  ///// */
			{
				CustomCategory.AddCustomRow(FText::FromString("Playlist"))
					[
						SNew(SHeader).Content()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("Add To Playlist")))
						]
					];
			}

			/* /////  asset drop target  ///// */
			{
				CustomCategory.AddCustomRow(FText::FromString("Playlist"))
					[
						SNew(SAssetDropTarget)
						.bSupportsMultiDrop(true)
					.OnAssetsDropped_Lambda([&](const FDragDropEvent& DragDropEvent, TArrayView<FAssetData> AssetDatas)
						{
							for (auto Data : AssetDatas)
								if (auto SoundFile = dynamic_cast<USoundBase*>(Data.GetAsset()))
									PlaylistRef->Add(SoundFile);
								else if (auto MusicTrack = dynamic_cast<UModularMusicTrack*>(Data.GetAsset()))
									PlaylistRef->Add(MusicTrack);

							CustomCategory.GetParentLayout().ForceRefreshDetails();
						})
					[
						SNew(SBorder)
						.HAlign(HAlign_Left)
							.VAlign(VAlign_Fill)
							.Padding(FMargin(5.0f))
							//.BorderImage(Brush)
							//.ColorAndOpacity(FSlateColor(FLinearColor(0, 0.220739f, 1.f)))
							[
								SNew(STextBlock)
								.MinDesiredWidth(450.f)
							.Justification(ETextJustify::Center)
							.Text(FText::FromString(TEXT("\n[ DROP FILES HERE TO ADD NEW TRACKS ]\n supported file types: ModularMusicTrack, SoundCue, SoundWave\n")))
							]
					]
					];
			}


			/* /////  adding new MusicTrack  ///// */
			{
				CustomCategory.AddCustomRow(FText::FromString("Playlist"))
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					.MaxWidth(150.f)
					[
						SNew(STextBlock)
						.Justification(ETextJustify::Right)
					.Text(FText::FromString("+ ModularMusicTrack "))
					]

				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.MaxWidth(200.f)
					[
						SNew(SObjectPropertyEntryBox)
						.DisplayBrowse(true)
					.DisplayThumbnail(true)
					.AllowedClass(UModularMusicTrack::StaticClass())
					.EnableContentPicker(true)
					.OnObjectChanged_Lambda([&](const FAssetData& SelectedAsset)
						{
							if (auto MusicTrack = dynamic_cast<UModularMusicTrack*>(SelectedAsset.GetAsset()))
								PlaylistRef->Add(MusicTrack);
							else if (auto SoundBase = dynamic_cast<USoundBase*>(SelectedAsset.GetAsset()))
								PlaylistRef->Add(SoundBase);

							CustomCategory.GetParentLayout().ForceRefreshDetails();
						})
					]
					];
			}

			/* /////  adding new SoundCue  ///// */
			{
				CustomCategory.AddCustomRow(FText::FromString("Playlist"))
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					.MaxWidth(150.f)
					[
						SNew(STextBlock)
						.Justification(ETextJustify::Right)
					.Text(FText::FromString("+ SoundCue "))
					]

				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.MaxWidth(200.f)
					[
						SNew(SObjectPropertyEntryBox)
						.DisplayBrowse(true)
					.DisplayThumbnail(true)
					.AllowedClass(USoundCue::StaticClass())
					.EnableContentPicker(true)
					.OnObjectChanged_Lambda([&](const FAssetData& SelectedAsset)
						{
							if (auto MusicTrack = dynamic_cast<UModularMusicTrack*>(SelectedAsset.GetAsset()))
								PlaylistRef->Add(MusicTrack);
							else if (auto SoundBase = dynamic_cast<USoundBase*>(SelectedAsset.GetAsset()))
								PlaylistRef->Add(SoundBase);

							CustomCategory.GetParentLayout().ForceRefreshDetails();
						})
					]
					];
			}

			/* /////  adding new SoundWave  ///// */
			{
				CustomCategory.AddCustomRow(FText::FromString("Playlist"))
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					.MaxWidth(150.f)
					[
						SNew(STextBlock)
						.Justification(ETextJustify::Right)
					.Text(FText::FromString("+ SoundWave "))
					]

				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					.MaxWidth(200.f)
					[
						SNew(SObjectPropertyEntryBox)
						.DisplayBrowse(true)
					.DisplayThumbnail(true)
					.AllowedClass(USoundWave::StaticClass())
					.EnableContentPicker(true)
					.OnObjectChanged_Lambda([&](const FAssetData& SelectedAsset)
						{
							if (auto MusicTrack = dynamic_cast<UModularMusicTrack*>(SelectedAsset.GetAsset()))
								PlaylistRef->Add(MusicTrack);
							else if (auto SoundBase = dynamic_cast<USoundBase*>(SelectedAsset.GetAsset()))
								PlaylistRef->Add(SoundBase);

							CustomCategory.GetParentLayout().ForceRefreshDetails();
						})
					]
					];
			}


			/* /////  playlist header  ///// */
			{
				CustomCategory.AddCustomRow(FText::FromString("Playlist"))
					[
						SNew(SHeader).Content()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("Playlist")))
						]
					];
			}

			const FSlateBrush* ClassIcon = FSlateIconFinder::FindIconBrushForClass(UModularMusicPlaylist::StaticClass());

			/* /////  loop through playlist  ///// */
			for (size_t i = 0; i < PlaylistRef->Playlist.Num(); i++)
			{
				CustomCategory.AddCustomRow(FText::FromString("Playlist"))
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
					.HAlign(HAlign_Center)
					.MaxWidth(25.f)
					[

						SNew(SButton)
						.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.ButtonStyle(FPlaylistStyle::Get(), "RemoveBrush")
					.OnClicked_Lambda([&, i]()
						{
							PlaylistRef->Playlist.RemoveAt(i);
							CustomCategory.GetParentLayout().ForceRefreshDetails();
							return FReply::Handled();
						}
					)
					]

				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Center)
					.MaxWidth(25.f)
					[

						SNew(SButton)
						.IsEnabled(i > 0)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.ButtonStyle(FPlaylistStyle::Get(), "MoveUpBrush")
					.OnClicked_Lambda([&, i]()
						{
							PlaylistRef->Playlist.Swap(i, i - 1);
							CustomCategory.GetParentLayout().ForceRefreshDetails();
							return FReply::Handled();
						}
					)
					]

				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Center)
					.MaxWidth(25.f)
					[

						SNew(SButton)
						.IsEnabled(i < PlaylistRef->Playlist.Num() - 1)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.ButtonStyle(FPlaylistStyle::Get(), "MoveDownBrush")
					.OnClicked_Lambda([&, i]()
						{
							PlaylistRef->Playlist.Swap(i, i + 1);
							CustomCategory.GetParentLayout().ForceRefreshDetails();
							return FReply::Handled();
						}
					)
					]

				+ SHorizontalBox::Slot()
					.HAlign(HAlign_Left)
					[
						SNew(SButton)
						.OnClicked_Lambda([&, i]()
							{
								TArray<UObject*> ObjectsToSync;
								auto MusicFile = PlaylistRef->Playlist[i];

								if (auto MusicTrack = (MusicFile->GetMusicTrack()))
									ObjectsToSync.Add(MusicTrack);
								else if (auto SoundFile = (MusicFile->GetSoundFile()))
									ObjectsToSync.Add(SoundFile);

								GEditor->SyncBrowserToObjects(ObjectsToSync);
								return FReply::Handled();
							}
						)
					.ToolTipText(FText::FromString("Browse to file"))
								.HAlign(HAlign_Left)
								.VAlign(VAlign_Fill)
								[
									SNew(STextBlock)
									.Text(FText::FromString(PlaylistRef->Playlist[i]->GetFileInfo(FString::FromInt(i + 1) + ":   ")))
								.Justification(ETextJustify::Left)
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
			DetailBuilder.HideProperty(Playlist);

			CustomCategory.AddCustomRow(FText::FromString("Playlist"))
				.WholeRowContent()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Left)
				.Text(LOCTEXT("MultiplePlaylistsWarning", "Please edit single Playlist to see all settings"))
				];
		}
	}
}


#undef LOCTEXT_NAMESPACE
