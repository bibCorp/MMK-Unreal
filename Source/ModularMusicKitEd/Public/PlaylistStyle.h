// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "SlateBasics.h"


class FPlaylistStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();

	static const ISlateStyle& Get();
	static FName GetStyleSetName();

private:
	static TSharedRef<class FSlateStyleSet> Create();
	static TSharedPtr<class FSlateStyleSet> PlaylistStyleInstance;
};
