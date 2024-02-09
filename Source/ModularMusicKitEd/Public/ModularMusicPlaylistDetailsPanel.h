// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "IDetailCustomization.h"


class FModularMusicPlaylistDetailsPanel : public IDetailCustomization
{
private:
	class UModularMusicPlaylist* PlaylistRef;

public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
