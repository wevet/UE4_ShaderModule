// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShaderModuleHUD.generated.h"

UCLASS()
class AShaderModuleHUD : public AHUD
{
	GENERATED_BODY()

public:
	AShaderModuleHUD();

	virtual void DrawHUD() override;

private:
	class UTexture2D* CrosshairTex;

};

