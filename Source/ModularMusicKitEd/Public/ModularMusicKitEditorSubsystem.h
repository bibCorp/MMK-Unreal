// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "ModularMusicKitEditorSubsystem.generated.h"


class UModularMusicTrack;


UCLASS()
class MODULARMUSICKITED_API UModularMusicKitEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Modular Music Kit|Editor Subsystem")
	UModularMusicTrack* CurrentMusicTrack;
	UPROPERTY(BlueprintReadOnly, Category = "Modular Music Kit|Editor Subsystem")
	FName MMTrackEditorWindowID = FName("");

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void OnBeginPIE(bool bIsSimulating);
	
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Editor Subsystem")
	void OpenModularMusicTrackEditor(UModularMusicTrack* MusicTrack = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Editor Subsystem")
	void SaveEditorChanges(UModularMusicTrack* MusicTrackEditorCopy);
};
