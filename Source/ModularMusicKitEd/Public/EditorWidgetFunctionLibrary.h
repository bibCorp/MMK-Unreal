// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <EditorUtilityWidgetBlueprint.h>
#include "EditorWidgetFunctionLibrary.generated.h"


UCLASS(BlueprintType)
class MODULARMUSICKITED_API UEditorWidgetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Modular Music Kit|Editor Widget Function Library")
	static void StartWidget(UWidgetBlueprint* Blueprint);

	template <typename T>
	void GetObjectsOfClass(TArray<T*>& OutArray);
};
