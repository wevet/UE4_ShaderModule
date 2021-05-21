// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

#include "Runtime/Engine/Classes/Engine/TextureRenderTarget.h"
#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"





class FVolumetricCloudsPainterEdModeToolkit : public FModeToolkit
{
public:

	FVolumetricCloudsPainterEdModeToolkit();

	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }


private:
	TSharedPtr<SWidget> ToolkitWidget;
	TAttribute<FString> ObjectPath;
	TSharedPtr<SPropertyEditorAsset> PropertyEditorAsset;
	TSharedPtr<IPropertyHandle> PropertyHandle;
	FOnShouldSetAsset OnShouldSetAsset;
	FCheckBoxStyle PaintCheckBoxStyle;
	FCheckBoxStyle PaintTypeCheckBoxStyle;

	bool IsActorSelected() const;

	void OnPainterCheckBoxStateChanged(const ECheckBoxState NewState);
	void OnPaintModeCheckBoxStateChanged(const ECheckBoxState NewState);
	void OnEraseModeCheckBoxStateChanged(const ECheckBoxState NewState);
	void OnChannelCheckBoxStateChanged(const ECheckBoxState NewState, const FName ChannelName);

	FText PainterCheckBoxText() const;
	FText ChannelCheckBoxText(const FName ChannelName) const;

	ECheckBoxState IsPainterCheckBoxChecked() const;
	ECheckBoxState IsPaintModeCheckBoxChecked() const;
	ECheckBoxState IsEraseModeCheckBoxChecked() const;
	ECheckBoxState IsChannelBoxChecked(const FName ChannelName) const;


};

