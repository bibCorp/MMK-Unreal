// Copyright 2023 bibCorp. All Rights Reserved.


#include "ModularMusicKitEditorSubsystem.h"
#include "ModularMusicEngine.h"
#include "ModularMusicTrack.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "IContentBrowserSingleton.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "FileHelpers.h"


void UModularMusicKitEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FEditorDelegates::BeginPIE.AddUObject(this, &UModularMusicKitEditorSubsystem::OnBeginPIE);
}

void UModularMusicKitEditorSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UModularMusicKitEditorSubsystem::OnBeginPIE(bool bIsSimulating)
{
	GEngine->GetEngineSubsystem<UModularMusicEngine>()->OnBeginPIE();
}

void UModularMusicKitEditorSubsystem::OpenModularMusicTrackEditor(UModularMusicTrack* MusicTrack)
{
	const FSoftObjectPath WidgetAssetPath(
		"/ModularMusicKit/EditorUtilityWidgets/EUW_MMTrackEditor.EUW_MMTrackEditor");
	UObject* WidgetAssetLoaded = WidgetAssetPath.TryLoad();
	if (WidgetAssetLoaded == nullptr)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Missing. Expected widget class at : /ModularMusicKit/EditorUtilityWidgets/EditorWindows/"));
		return;
	}

	UEditorUtilityWidgetBlueprint* widget = Cast<UEditorUtilityWidgetBlueprint>(WidgetAssetLoaded);
	if (widget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast failed"));
		return;
	}

	CurrentMusicTrack = MusicTrack;

	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();

	if (MMTrackEditorWindowID != "")
	{
		EditorUtilitySubsystem->CloseTabByID(MMTrackEditorWindowID);
		MMTrackEditorWindowID = FName("");
	}
	auto Window = EditorUtilitySubsystem->SpawnAndRegisterTabAndGetID(widget, MMTrackEditorWindowID);
}

void UModularMusicKitEditorSubsystem::SaveEditorChanges(UModularMusicTrack* MusicTrackEditorCopy)
{
	if (CurrentMusicTrack)
	{
		CurrentMusicTrack->Apply(MusicTrackEditorCopy);

		TArray<UObject*> ObjectsToSave;
		for (UObject* TestObject = CurrentMusicTrack; TestObject != nullptr; TestObject = TestObject->GetOuter())
		{
			if (TestObject->IsAsset())
			{
				ObjectsToSave.Add(TestObject);
				break;
			}
		}

		if (ObjectsToSave.Num() == 0)
		{
			return;
		}

		TArray<UPackage*> PackagesToSave;

		for (UObject* Object : ObjectsToSave)
		{
			if ((Object == nullptr) || !Object->IsAsset())
			{
				// Log invalid object. Don't try to save it
				UE_LOG(LogTemp, Warning, TEXT("Invalid object to save: %s"),
				       (Object != nullptr) ? *Object->GetFullName() : TEXT("Null Object"));
			}
			else
			{
				PackagesToSave.Add(Object->GetOutermost());
			}
		}

		FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);
	}
}
