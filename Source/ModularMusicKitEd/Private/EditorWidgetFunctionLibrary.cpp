// Copyright 2023 bibCorp. All Rights Reserved.


#include "EditorWidgetFunctionLibrary.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistry/AssetRegistryModule.h"


void UEditorWidgetFunctionLibrary::StartWidget(UWidgetBlueprint* Blueprint)
{
	if (Blueprint->GeneratedClass->IsChildOf(UEditorUtilityWidget::StaticClass()))
	{
		const UEditorUtilityWidget* DefaultObject = Cast<UEditorUtilityWidget>(
			Blueprint->GeneratedClass->GetDefaultObject());
		if (DefaultObject->ShouldAutoRunDefaultAction())
		{
			UEditorUtilityWidget* Instance = NewObject<UEditorUtilityWidget>(
				GetTransientPackage(), Blueprint->GeneratedClass);
			Instance->ExecuteDefaultAction();
		}
		else
		{
			FName RegistrationName = FName(*(Blueprint->GetPathName() + TEXT("_ActiveTab")));
			FText DisplayName = FText::FromString(Blueprint->GetName());
			FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(
				TEXT("LevelEditor"));
			TSharedPtr LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
			TSharedPtr NewDockTab = LevelEditorTabManager->TryInvokeTab(RegistrationName);
		}
	}
}

template <typename T>
void UEditorWidgetFunctionLibrary::GetObjectsOfClass(TArray<T*>& OutArray)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<
		FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByClass(T::StaticClass()->GetFName(), AssetData);
	for (int i = 0; i < AssetData.Num(); i++)
	{
		T* Object = Cast<T>(AssetData[i].GetAsset());
		OutArray.Add(Object);
	}
}
