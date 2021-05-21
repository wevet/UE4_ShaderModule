// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"
#include "CloudStaticMeshActor.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Engine/StaticMeshActor.h"

class FVolumetricCloudsPainterEdMode : public FEdMode
{
public:
	const static FEditorModeID EM_VolumetricCloudsPainterEdModeId;

public:
	FVolumetricCloudsPainterEdMode();
	virtual ~FVolumetricCloudsPainterEdMode();

	virtual void Enter() override;
	virtual void Exit() override;

	virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	virtual void ActorSelectionChangeNotify() override;
	virtual void DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
	virtual bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	bool UsesToolkits() const override;

public:
#pragma region LoadAssets
	UMaterialInstanceConstant* ColorBlendMaterialInstance;
	UMaterialInstanceConstant* AlphaBlendMaterialInstance;
	UMaterial* AlphCombineMaterial;
	UTextureRenderTarget2D* ColorBlendRenderTarget;
	UTextureRenderTarget2D* AlphaBlendRenderTarget;
	UTextureRenderTarget2D* RenderTarget;
#pragma endregion

	FVector2D PreviousMousePosition;
	FVector2D FinalTextureSize;
	FVector WorldBrushPos;

	UTexture2D* FinalTexture;
	ACloudStaticMeshActor* CloudsActor;
	UMaterialInstanceConstant* CloudsMaterial;

	FLinearColor BrushColor;

	bool bTextureNeedToLoad;
	bool bPressedLMB;
	bool bPainiting;
	bool bAdditivePaint;

	bool bRedChannelEnabled;
	bool bGreenChannelEnabled;
	bool bBlueChannelEnabled;
	bool bAlphaChannelEnabled;

	float BrushRadius;
	float BrushFalloff;
	float BrushOpacity;

public:
	const bool GetCloudsActor();
	bool IsChannelEnabled(const FName Channel) const;
	bool IsPainiting() const;
	bool IsAdditivePaint() const;
	float GetBrushChannelValue(const FName Channel) const;
	float GetBrushRadius() const;
	float GetBrushFalloff() const;
	float GetBrushOpacity() const;

	void LoadTexture();
	void ReleaseCloudsActor();
	void DrawToRenderTaget();
	void SetPreRenderBrushParameters();
	void SetBrushRadius(const float NewRadius);
	void SetBrushFalloff(const float NewFalloff);
	void SetBrushOpacity(const float NewOpacity);
	void SetBrushChannelValue(float NewValue, FName Channel);

	void SetPaintMode(const bool NewState);
	void SetPaintState(const bool NewState);
	void SetChannelState(const bool NewValue, const FName Channel);
};
