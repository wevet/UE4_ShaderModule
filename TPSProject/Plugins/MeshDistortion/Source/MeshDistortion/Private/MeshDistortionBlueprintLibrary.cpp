// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshDistortionBlueprintLibrary.h"



UMeshDistortionBlueprintLibrary::UMeshDistortionBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//
}


void UMeshDistortionBlueprintLibrary::GetUnDistortionOverScanFactor(
	const FMeshDistortionModel& CameraModel, 
	float HorizontalFOV, 
	float AspectRatio, 
	float& OutOverScanFactor)
{
	OutOverScanFactor = CameraModel.GetUnDistortionOverScanFactor(HorizontalFOV, AspectRatio);
}


void UMeshDistortionBlueprintLibrary::DrawToRenderTarget(
	const UObject* WorldContextObject,
	const FMeshDistortionModel& CameraModel,
	class UTextureRenderTarget2D* RenderTarget,
	const float HorizontalFOV,
	const float AspectRatio,
	const float OverScanFactor,
	float OutputMultiply,
	float OutputAdd)
{
	CameraModel.DrawToRenderTarget(
		WorldContextObject->GetWorld(),
		RenderTarget, 
		HorizontalFOV, 
		AspectRatio, 
		OverScanFactor, 
		OutputMultiply, 
		OutputAdd);
}


bool UMeshDistortionBlueprintLibrary::EqualEqual_CompareMeshDistortionModels(
	const FMeshDistortionModel& A, 
	const FMeshDistortionModel& B)
{
	return A == B;
}


bool UMeshDistortionBlueprintLibrary::NotEqual_CompareMeshDistortionModels(
	const FMeshDistortionModel& A, 
	const FMeshDistortionModel& B)
{
	return A != B;
}


