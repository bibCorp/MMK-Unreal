// Copyright 2023 bibCorp. All Rights Reserved.


#include "PlaylistStyle.h"
#include "Slate/SlateGameResources.h"


TSharedPtr<FSlateStyleSet> FPlaylistStyle::PlaylistStyleInstance = NULL;

void FPlaylistStyle::Initialize()
{
	if (!PlaylistStyleInstance.IsValid())
	{
		PlaylistStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*PlaylistStyleInstance);
	}
}

void FPlaylistStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*PlaylistStyleInstance);
	ensure(PlaylistStyleInstance.IsUnique());
	PlaylistStyleInstance.Reset();
}

FName FPlaylistStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("PlaylistStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH(RelativePath, ... )	 FSlateImageBrush(	FPaths::ProjectPluginsDir() / "ModularMusicKit/Content/Resources/Textures"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BOX_BRUSH(RelativePath, ... )	 FSlateBoxBrush(	FPaths::ProjectPluginsDir() / "ModularMusicKit/Content/Resources/Textures"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BORDER_BRUSH(RelativePath, ... ) FSlateBorderBrush( FPaths::ProjectPluginsDir() / "ModularMusicKit/Content/Resources/Textures"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define TTF_FONT(RelativePath, ... )	 FSlateFontInfo(	FPaths::ProjectPluginsDir() / "ModularMusicKit/Content/Resources/Textures"/ RelativePath + TEXT(".ttf"), __VA_ARGS__ )
#define OTF_FONT(RelativePath, ... )	 FSlateFontInfo(	FPaths::ProjectPluginsDir() / "ModularMusicKit/Content/Resources/Textures"/ RelativePath + TEXT(".otf"), __VA_ARGS__ )

TSharedRef<FSlateStyleSet> FPlaylistStyle::Create()
{
	auto color_normal = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f));
	auto color_hovered = FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f));
	auto color_pressed = FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f));

	TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(FPlaylistStyle::GetStyleSetName(),
	                                                               FPaths::ProjectPluginsDir() /
	                                                               "ModularMusicKit/Content/Resources/Textures",
	                                                               FPaths::ProjectPluginsDir() /
	                                                               "ModularMusicKit/Content/Resources/Textures");

	FSlateStyleSet& Style = StyleRef.Get();
	Style.Set("RemoveBrush",
	          FButtonStyle()
	          .SetNormal(IMAGE_BRUSH("cross", FVector2D(32, 16), color_normal))
	          .SetHovered(IMAGE_BRUSH("cross", FVector2D(32, 16), color_hovered))
	          .SetPressed(IMAGE_BRUSH("cross", FVector2D(32, 16), color_pressed)));

	Style.Set("MoveUpBrush",
	          FButtonStyle()
	          .SetNormal(IMAGE_BRUSH("arrow-up", FVector2D(32, 16), color_normal))
	          .SetHovered(IMAGE_BRUSH("arrow-up", FVector2D(32, 16), color_hovered))
	          .SetPressed(IMAGE_BRUSH("arrow-up", FVector2D(32, 16), color_pressed)));

	Style.Set("MoveDownBrush",
	          FButtonStyle()
	          .SetNormal(IMAGE_BRUSH("arrow-down", FVector2D(32, 16), color_normal))
	          .SetHovered(IMAGE_BRUSH("arrow-down", FVector2D(32, 16), color_hovered))
	          .SetPressed(IMAGE_BRUSH("arrow-down", FVector2D(32, 16), color_pressed)));

	Style.Set("NormalButtonText",
	          FTextBlockStyle(FTextBlockStyle::GetDefault())
	          .SetColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1))));

	return StyleRef;
}
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FPlaylistStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FPlaylistStyle::Get()
{
	return *PlaylistStyleInstance;
}
