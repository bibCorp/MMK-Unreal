// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "IDetailCustomization.h"


class FModularMusicPlayerDetailsPanel : public IDetailCustomization
{
private:
	/* Contains references to all selected objects inside in the viewport */
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;

public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
