// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "SceneUtils.h"
#include "MeshDistortionAPI.generated.h"


#define GRID_SUBDIVITIONX 32
#define GRID_SUBDIVITIONY 16



USTRUCT(BlueprintType)
struct MESHDISTORTION_API FMeshDistortionModel
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshDistortionModel")
	float K1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshDistortionModel")
	float K2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshDistortionModel")
	float K3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshDistortionModel")
	float P1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshDistortionModel")
	float P2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshDistortionModel")
	FVector2D F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshDistortionModel")
	FVector2D C;


public:
	FMeshDistortionModel()
	{
		K1 = K2 = K3 = P1 = P2 = 0.f;
		F = FVector2D(1.f, 1.f);
		C = FVector2D(0.5f, 0.5f);
	}


	FVector2D UnDistortionNormalizedViewPosition(const FVector2D EngineV) const;


	float GetUnDistortionOverScanFactor(const float HorizontalFOV, const float AspectRatio) const;


	void DrawToRenderTarget(
		class UWorld* World,
		class UTextureRenderTarget2D* RenderTarget,
		const float HorizontalFOV,
		const float AspectRatio,
		const float Factor,
		float OutputMultiply,
		float OutputAdd) const;


	FORCEINLINE bool operator == (const FMeshDistortionModel& Other) const
	{
		return (
			K1 == Other.K1 && K2 == Other.K2 && K3 == Other.K3 &&
			P1 == Other.P1 && P2 == Other.P2 &&
			F == Other.F && C == Other.C);
	}

	FORCEINLINE bool operator != (const FMeshDistortionModel& Other) const
	{
		return !(*this == Other);
	}
};


struct FCompiledMeshModel
{
	FMeshDistortionModel OriginalModel;
	FVector4 DistortedMatrix;
	FVector4 UndistortedMatrix;
	FVector2D OutputMultiplyAndAdd;
};

