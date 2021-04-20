// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MeshDistortionAPI.h"
#include "MeshDistortionBlueprintLibrary.generated.h"


UCLASS(MinimalAPI, meta = (ScriptName = "MeshDistortionLibrary"))
class UMeshDistortionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


public:
	UFUNCTION(BlueprintPure, Category = "MeshDistortion")
	static void GetUnDistortionOverScanFactor(const FMeshDistortionModel& CameraModel, float HorizontalFOV, float AspectRatio, float& OutOverScanFactor);


	UFUNCTION(BlueprintCallable, Category = "MeshDistortion", meta = (WorldContext = "WorldContextObject"))
	static void DrawToRenderTarget(
		const UObject* WorldContextObject, 
		const FMeshDistortionModel& CameraModel,
		class UTextureRenderTarget2D* RenderTarget,
		const float HorizontalFOV, 
		const float AspectRatio,
		const float OverScanFactor,
		float OutputMultiply = 0.5f,
		float OutputAdd = 0.5f);


	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (MeshDistortionModel)", CompactNodeTitle = "==", Keywords = "== equal"), Category = "MeshDistortion")
	static bool EqualEqual_CompareMeshDistortionModels(const FMeshDistortionModel& A, const FMeshDistortionModel& B);



	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (MeshDistortionModel)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = "MeshDistortion")
	static bool NotEqual_CompareMeshDistortionModels(const FMeshDistortionModel& A, const FMeshDistortionModel& B);

};
