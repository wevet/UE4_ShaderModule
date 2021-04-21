// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BillboardComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget.h"
#include "VC_Clouds.generated.h"


UCLASS()
class VOLUMETRICCLOUDS_API AVC_Clouds : public AActor
{
	GENERATED_BODY()

public:
	AVC_Clouds(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostLoad() override;
	virtual void PostActorCreated() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
#endif 


protected:
	void CreateDynamicMaterials();

	void LoadMaterialParameters();
	void SetMaterialScalar(const FName Name, const float Value);
	void SetMaterialColor(const FName Name, const FLinearColor Value);
	void SetMaterialTexture(const FName Name, UTexture2D* Value);
	void UpdateLightDirection();
	void UpdateTransform();
	void RenderShadows();

	bool HasGameWorld() const;

public:
	FORCEINLINE bool IsDynamicInGame() const
	{
		return bUseDynamicMaterials; 
	};


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* SpriteComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CloudsBoxMesh;


#pragma region CppProperties
protected:
	UPROPERTY()
	class UTexture2D* SpriteTexture;

	UPROPERTY()
	class UTextureRenderTarget2D* ShadowRenderTarget;

	UPROPERTY()
	class UMaterialInstanceConstant* CloudsMaterialInstance;

	UPROPERTY()
	class UMaterialInstanceConstant* ShadowRenderMaterialInstance;

	UPROPERTY()
	class UMaterialInstanceConstant* LightFunctionMaterialInstance;

	UPROPERTY()
	class UMaterialInstanceDynamic* CloudsMaterialInstanceDynamic;

	UPROPERTY()
	class UMaterialInstanceDynamic* ShadowRenderMaterialInstanceDynamic;

	UPROPERTY()
	class UMaterialInstanceDynamic* LightFunctionMaterialInstanceDynamic;

	UPROPERTY()
	float Time;

	const TArray<FName> ArrayMaterialsParameterNames
	{
		FName("AtmosphereBlendDistance"),
		FName("AtmosphereBlendIntensity"),
		FName("AttenClampIntensity"),
		FName("BeerLawDensity"),
		FName("CloudOutScatterAmbient"),
		FName("Coverage"),
		FName("Density"),
		FName("InOutScatterLerp"),
		FName("InScatter"),
		FName("InScatterIntensity"),
		FName("LightColorIntensity"),
		FName("LightIntensity"),
		FName("LightPow"),
		FName("LightStepScale"),
		FName("MipMapScaleDistance"),
		FName("NoiseTile"),
		FName("SmallNoiseTile"),
		FName("OutScatter"),
		FName("RayMaxSteps"),
		FName("ShadowColor"),
		FName("ShadowMaxSteps"),
		FName("SilverLightExp"),
		FName("SilverLightIntensity"),
		FName("StepScaleDistance"),
		FName("WeatherMapTile"),
		FName("WindDirectionX"),
		FName("WindDirectionY"),
		FName("WindDirectionZ"),
		FName("WindSpeed"),
		FName("ActorPosition"),
		FName("ActorScale"),
		FName("LightDirection"),
		FName("Time"),
		FName("ShadowBlur"),
		FName("ShadowDensity"),
		FName("ShadowIntensity")
		//LightDirection
		//ActorPos
		//ActorScale
	};
#pragma endregion


#pragma region Properties
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds")
	ADirectionalLight* DirectionalLightActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds")
	FLinearColor ShadowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds")
	UTexture2D* WeatherMapTexure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Atmosphere", DisplayName = "Blend Disntace", meta = (ClampMin = "0.0", ClampMax = "20.0", UIMin = "0.0", UIMax = "20.0"))
	float AtmosphereBlendDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Atmosphere", DisplayName = "Blend Intensity", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float AtmosphereBlendIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "Shadow Density", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float BeerLawDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "Attenusation Clamp", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float AttenClampIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "Ambient Intensity", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float CloudOutScatterAmbient;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "In Scatter", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float InScatter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "In Scatter Instensity", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float InScatterIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "Silver Light Exp", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SilverLightExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "Silver Light Intensity", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SilverLightIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "Out Scatter", meta = (ClampMin = "-1.0", ClampMax = "0.0", UIMin = "-1.0", UIMax = "0.0"))
	float OutScatter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "InOutScatterLerp", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float InOutScatterLerp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "Light Color Intensity", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float LightColorIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "Light Intensity", meta = (ClampMin = "0.0", ClampMax = "6.0", UIMin = "0.0", UIMax = "6.0"))
	float LightIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Light", DisplayName = "Light Power", meta = (ClampMin = "0.0", ClampMax = "5.0", UIMin = "0.0", UIMax = "5.0"))
	float LightPow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Weather", DisplayName = "Coverage", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float Coverage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Weather", DisplayName = "Density", meta = (ClampMin = "0.0", ClampMax = "20.0", UIMin = "0.0", UIMax = "20.0"))
	float Density;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Weather", DisplayName = "Map Tile", meta = (ClampMin = "0.0", ClampMax = "14.0", UIMin = "0.0", UIMax = "14.0"))
	float WeatherMapTile;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Ray", DisplayName = "View Max Steps", meta = (ClampMin = "1.0", ClampMax = "4000.0", UIMin = "1.0", UIMax = "4000.0"))
	float RayMaxSteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Ray", DisplayName = "View Step Scale Distance", meta = (ClampMin = "1.0", ClampMax = "4000.0", UIMin = "1.0", UIMax = "4000.0"))
	float StepScaleDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Ray", DisplayName = "Shadow Max Steps", meta = (ClampMin = "1.0", ClampMax = "32.0", UIMin = "1.0", UIMax = "32.0"))
	float ShadowMaxSteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Ray", DisplayName = "Shadow Step Scale", meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0"))
	float LightStepScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Wind", DisplayName = "X Direction", meta = (ClampMin = "-1.0", ClampMax = "1.0", UIMin = "-1.0", UIMax = "1.0"))
	float WindDirectionX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Wind", DisplayName = "Y Direction", meta = (ClampMin = "-1.0", ClampMax = "1.0", UIMin = "-1.0", UIMax = "1.0"))
	float WindDirectionY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Wind", DisplayName = "Z Direction", meta = (ClampMin = "-1.0", ClampMax = "1.0", UIMin = "-1.0", UIMax = "1.0"))
	float WindDirectionZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Wind", DisplayName = "Speed", meta = (UIMin = "0.0", UIMax = "20.0"))
	float WindSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Noise", DisplayName = "Noise Tile", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float NoiseTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Noise", DisplayName = "Detail Noise Tile", meta = (ClampMin = "0.0", ClampMax = "1000.0", UIMin = "0.0", UIMax = "1000.0"))
	float SmallNoiseTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Ground Shadow", DisplayName = "Blur", meta = (ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0"))
	float ShadowBlur;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Ground Shadow", DisplayName = "Density", meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0"))
	float ShadowDensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Ground Shadow", DisplayName = "Intensity", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float ShadowIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Misc", DisplayName = "Mip Map Scale Disntace", meta = (ClampMin = "0.0", ClampMax = "20.0", UIMin = "0.0", UIMax = "20.0"))
	float MipMapScaleDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clouds|Misc", DisplayName = "Use Dynamic Materials")
	bool bUseDynamicMaterials;
#pragma endregion



};
