// Copyright 2023 bibCorp. All Rights Reserved.


#pragma once

#include "IDetailCustomization.h"


class UModularMusicTrack;

class FModularMusicTrackDetailsPanel : public IDetailCustomization
{
private:
	UModularMusicTrack* MusicTrackRef;

public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	void ProcessDroppedAssets(const FDragDropEvent& Event, TArrayView<FAssetData> AssetDatas);
};
