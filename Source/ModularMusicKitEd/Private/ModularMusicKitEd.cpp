// Copyright 2023 bibCorp. All Rights Reserved.


#include "ModularMusicKitEd.h"
#include "ModularMusicPlayer.h"
#include "ModularMusicTrack.h"
#include "ModularMusicPlaylist.h"
#include "ModularMusicPlayerDetailsPanel.h"
#include "ModularMusicTrackDetailsPanel.h"
#include "ModularMusicPlaylistDetailsPanel.h"
#include "PropertyEditorModule.h"
#include <Styling/SlateStyleRegistry.h>
#include "PlaylistStyle.h"


#define LOCTEXT_NAMESPACE "FModularMusicKitEdModule"


void FModularMusicKitEdModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(AModularMusicPlayer::StaticClass()->GetFName(),   FOnGetDetailCustomizationInstance::CreateStatic(&FModularMusicPlayerDetailsPanel::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UModularMusicTrack::StaticClass()->GetFName(),    FOnGetDetailCustomizationInstance::CreateStatic(&FModularMusicTrackDetailsPanel::MakeInstance));
	PropertyModule.RegisterCustomClassLayout(UModularMusicPlaylist::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FModularMusicPlaylistDetailsPanel::MakeInstance));
    PropertyModule.NotifyCustomizationModuleChanged();

	//Hot reload
	FSlateStyleRegistry::UnRegisterSlateStyle(FPlaylistStyle::GetStyleSetName());
	FPlaylistStyle::Initialize();
}

void FModularMusicKitEdModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout(AModularMusicPlayer::StaticClass()->GetFName());
	PropertyModule.UnregisterCustomClassLayout(UModularMusicTrack::StaticClass()->GetFName());
	PropertyModule.UnregisterCustomClassLayout(UModularMusicPlaylist::StaticClass()->GetFName());
	PropertyModule.NotifyCustomizationModuleChanged();

	FPlaylistStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModularMusicKitEdModule, ModularMusicKitEd)
