// Copyright Epic Games, Inc. All Rights Reserved.

#include "VC_Clouds.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Components/LightComponent.h"

#define TIME FName(TEXT("Time"))
#define SHADOW_COLOR FName(TEXT("ShadowColor"))
#define WEATHER_MAP FName(TEXT("WeatherMapTexure"))
#define LIGHT_DIRECTION FName(TEXT("LightDirection"))
#define ACTOR_POSITION FName(TEXT("ActorPosition"))
#define ACTOR_SCALE FName(TEXT("ActorScale"))

//#define OVERRIDE_LIGHTDIRECTION 1

AVC_Clouds::AVC_Clouds(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	USceneComponent* SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	RootComponent->Mobility = EComponentMobility::Static;

	CloudsBoxMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CloudsBoxMesh"));
	CloudsBoxMesh->SetupAttachment(RootComponent);


#if WITH_EDITORONLY_DATA
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> TextureObject;
		FName ID;
		FText NAME;

		FConstructorStatics()
			: TextureObject(TEXT("Texture2D'/VolumetricClouds/Icons/I_VolumetricClouds.I_VolumetricClouds'")),
			ID(TEXT("VolumetricClouds")),
			NAME(NSLOCTEXT("SpriteCategory", "VolumetricClouds", "VolumetricClouds"))
		{

		}
	};

	static FConstructorStatics ConstructorStatics;
	SpriteComponent = ObjectInitializer.CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("Sprite"));
	SpriteComponent->Sprite = ConstructorStatics.TextureObject.Get();
	SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID;
	SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME;
	SpriteComponent->SetupAttachment(RootComponent);
	SpriteComponent->Mobility = EComponentMobility::Static;
#endif

	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> FindAsset(TEXT("/VolumetricClouds/Meshes/SM_CloudsBox.SM_CloudsBox"));
		CloudsBoxMesh->SetStaticMesh(FindAsset.Object);
	}

	{
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> FindAsset(TEXT("/VolumetricClouds/Materials/MI_VolumetricClouds.MI_VolumetricClouds"));
		CloudsMaterialInstance = FindAsset.Object;
		CloudsBoxMesh->SetMaterial(0, CloudsMaterialInstance);
	}

	{
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> FindAsset(TEXT("/VolumetricClouds/Materials/MI_GroundShadowRender.MI_GroundShadowRender"));
		ShadowRenderMaterialInstance = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> FindAsset(TEXT("/VolumetricClouds/Materials/MI_GroundShadows.MI_GroundShadows"));
		LightFunctionMaterialInstance = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> FindAsset(TEXT("/VolumetricClouds/Textures/RT_GroundShadows.RT_GroundShadows"));
		ShadowRenderTarget = FindAsset.Object;
	}

	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> FindAsset(TEXT("/VolumetricClouds/Textures/T_Weather_MA.T_Weather_MA"));
		WeatherMapTexure = FindAsset.Object;
	}


	// Init Params
	bUseDynamicMaterials = true;

	ShadowColor = FLinearColor(0.60f, 0.63f, 0.7f, 1.0f);
	AtmosphereBlendDistance = 5.0f;
	AtmosphereBlendIntensity = 1.0f;
	BeerLawDensity = 0.85f;
	AttenClampIntensity = 0.6f;
	CloudOutScatterAmbient = 1.0f;
	InScatterIntensity = 0.05f;
	SilverLightIntensity = 0.5f;
	LightColorIntensity = 1.25f;
	SilverLightExp = 1.0f;
	InOutScatterLerp = 0.5f;
	InScatter = 0.75f;
	OutScatter = -0.045f;
	LightIntensity = 4.0f;
	LightPow = 3.0f;
	Coverage = 0.65f;
	Density = 14.0f;
	WeatherMapTile = 1.5f;
	RayMaxSteps = 500.0f;
	StepScaleDistance = 2.0f;
	ShadowMaxSteps = 8.0f;
	LightStepScale = 2.0f;
	MipMapScaleDistance = 5.0f;
	ShadowIntensity = 1.0f;
	ShadowDensity = 1.0f;
	ShadowBlur = 0.25f;
	SmallNoiseTile = 230.f;
	NoiseTile = 20.f;
	WindSpeed = 12.f;
	WindDirectionX = 0.25f;
	WindDirectionZ = -0.05f;

	// Init Transform
	const FVector BasePos(FVector(0.0f, 0.0f, 125000.0f));
	const FVector BaseScale(FVector(40000.0f, 40000.0f, 2000.0f));
	SetActorLocation(BasePos);
	SetActorScale3D(BaseScale);
}


void AVC_Clouds::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateTransform();
}


void AVC_Clouds::PostLoad()
{
	Super::PostLoad();
	CreateDynamicMaterials();
	LoadMaterialParameters();
}


void AVC_Clouds::PostActorCreated()
{
	Super::PostActorCreated();
	CreateDynamicMaterials();
	LoadMaterialParameters();
}


void AVC_Clouds::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void AVC_Clouds::BeginPlay()
{
	Super::BeginPlay();
}


void AVC_Clouds::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Time += DeltaTime;
	SetMaterialScalar(TIME, Time);
	UpdateLightDirection();
	RenderShadows();
}


#if WITH_EDITOR
void AVC_Clouds::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AVC_Clouds, DirectionalLightActor))
	{
		UpdateLightDirection();
#ifdef OVERRIDE_LIGHTDIRECTION
		if (DirectionalLightActor)
		{
			DirectionalLightActor->SetLightFunctionFadeDistance(GetActorScale().X * 50.0f);
		}
#endif
	}

	if (FFloatProperty* PropertyFloat = ((FFloatProperty*)e.Property))
	{
		if (PropertyName == FName("R") || PropertyName == FName("G") || PropertyName == FName("B"))
		{
			SetMaterialColor(SHADOW_COLOR, FLinearColor(ShadowColor));
		}
		else
		{
			float Value = PropertyFloat->GetPropertyValue_InContainer(this);
			SetMaterialScalar(PropertyName, Value);
		}
	}

	if (FStructProperty* StructProperty = ((FStructProperty*)e.Property))
	{
		SetMaterialColor(SHADOW_COLOR, FLinearColor(ShadowColor));
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AVC_Clouds, WeatherMapTexure))
	{
		SetMaterialTexture(PropertyName, WeatherMapTexure);
	}

}


bool AVC_Clouds::ShouldTickIfViewportsOnly() const
{
	return true;
}
#endif


void AVC_Clouds::CreateDynamicMaterials()
{
	if (!GetWorld())
	{
		return;
	}

	if (CloudsMaterialInstance)
	{
		CloudsMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(CloudsMaterialInstance, this);
		CloudsBoxMesh->SetMaterial(0, CloudsMaterialInstanceDynamic);
	}

	if (ShadowRenderMaterialInstance)
	{
		ShadowRenderMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(ShadowRenderMaterialInstance, this);
	}

	if (LightFunctionMaterialInstance)
	{
		LightFunctionMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(LightFunctionMaterialInstance, this);

#ifdef OVERRIDE_LIGHTDIRECTION
		if (DirectionalLightActor)
		{
			DirectionalLightActor->SetLightFunctionMaterial(LightFunctionMaterialInstanceDynamic);
		}
#endif
	}
}


void AVC_Clouds::LoadMaterialParameters()
{
	if (!GetWorld())
	{
		return;
	}

	for (TFieldIterator<FProperty>PropIt(GetClass()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		const FName PropertyName = FName(*Property->GetName());

		if (ArrayMaterialsParameterNames.Contains(PropertyName))
		{
			if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
			{
				const float Value = FloatProperty->GetPropertyValue_InContainer(this);
				SetMaterialScalar(PropertyName, Value);
			}

			if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
			{
				const FLinearColor* ColorPtr = Property->ContainerPtrToValuePtr<FLinearColor>(this);
				if (ColorPtr)
				{
					FLinearColor Value = *ColorPtr;
					SetMaterialColor(PropertyName, Value);
				}
			}
		}
	}

	UpdateLightDirection();
	UpdateTransform();
	SetMaterialTexture(WEATHER_MAP, WeatherMapTexure);
}


void AVC_Clouds::SetMaterialScalar(const FName Name, const float Value)
{
	if (!GetWorld())
	{
		return;
	}

	if (IsDynamicInGame())
	{
		if (CloudsMaterialInstanceDynamic && ShadowRenderMaterialInstanceDynamic && LightFunctionMaterialInstanceDynamic)
		{
			CloudsMaterialInstanceDynamic->SetScalarParameterValue(Name, Value);
			ShadowRenderMaterialInstanceDynamic->SetScalarParameterValue(Name, Value);
			LightFunctionMaterialInstanceDynamic->SetScalarParameterValue(Name, Value);
		}
	}
	else
	{
		if (CloudsMaterialInstance && ShadowRenderMaterialInstance && LightFunctionMaterialInstance)
		{
			CloudsMaterialInstance->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(Name), Value);
			ShadowRenderMaterialInstance->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(Name), Value);
			LightFunctionMaterialInstance->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(Name), Value);
		}
	}
}


void AVC_Clouds::SetMaterialColor(const FName Name, const FLinearColor Value)
{
	if (!GetWorld())
	{
		return;
	}

	if (IsDynamicInGame())
	{
		if (CloudsMaterialInstanceDynamic && ShadowRenderMaterialInstanceDynamic && LightFunctionMaterialInstanceDynamic)
		{
			CloudsMaterialInstanceDynamic->SetVectorParameterValue(Name, Value);
			ShadowRenderMaterialInstanceDynamic->SetVectorParameterValue(Name, Value);
			LightFunctionMaterialInstanceDynamic->SetVectorParameterValue(Name, Value);
		}
	}
	else
	{
		if (CloudsMaterialInstance && ShadowRenderMaterialInstance && LightFunctionMaterialInstance)
		{
			CloudsMaterialInstance->SetVectorParameterValueEditorOnly(FMaterialParameterInfo(Name), Value);
			ShadowRenderMaterialInstance->SetVectorParameterValueEditorOnly(FMaterialParameterInfo(Name), Value);
			LightFunctionMaterialInstance->SetVectorParameterValueEditorOnly(FMaterialParameterInfo(Name), Value);
		}
	}
}


void AVC_Clouds::SetMaterialTexture(const FName Name, UTexture2D* Value)
{
	if (!GetWorld())
	{
		return;
	}

	if (IsDynamicInGame())
	{
		if (CloudsMaterialInstanceDynamic && ShadowRenderMaterialInstanceDynamic)
		{
			CloudsMaterialInstanceDynamic->SetTextureParameterValue(Name, Value);
			ShadowRenderMaterialInstanceDynamic->SetTextureParameterValue(Name, Value);
		}
	}
	else
	{
		if (CloudsMaterialInstance && ShadowRenderMaterialInstance)
		{
			CloudsMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(Name), Value);
			ShadowRenderMaterialInstance->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(Name), Value);
		}
	}
}


void AVC_Clouds::UpdateLightDirection()
{
	if (!DirectionalLightActor)
	{
		return;
	}

	const FVector LightDirection = DirectionalLightActor->GetActorForwardVector() * -1;
	SetMaterialColor(LIGHT_DIRECTION, FLinearColor(LightDirection));

#ifdef OVERRIDE_LIGHTDIRECTION
	if (IsDynamicInGame())
	{
		if (DirectionalLightActor->GetLightComponent()->LightFunctionMaterial != LightFunctionMaterialInstanceDynamic && LightFunctionMaterialInstanceDynamic)
		{
			DirectionalLightActor->SetLightFunctionMaterial(LightFunctionMaterialInstanceDynamic);
		}
	}
	else
	{
		if (DirectionalLightActor->GetLightComponent()->LightFunctionMaterial != LightFunctionMaterialInstance && LightFunctionMaterialInstance)
		{
			DirectionalLightActor->SetLightFunctionMaterial(LightFunctionMaterialInstance);
		}
	}
#endif
}


void AVC_Clouds::UpdateTransform()
{
	SetMaterialColor(ACTOR_POSITION, FLinearColor(GetTransform().GetLocation()));
	SetMaterialColor(ACTOR_SCALE, FLinearColor(GetActorScale3D()));
}


void AVC_Clouds::RenderShadows()
{
	if (!GetWorld())
	{
		return;
	}

	if (!ShadowRenderTarget)
	{
		return;
	}

	if (IsDynamicInGame())
	{
		if (ShadowRenderMaterialInstanceDynamic)
		{
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), ShadowRenderTarget, ShadowRenderMaterialInstanceDynamic);
		}
	}
	else
	{
		if (ShadowRenderMaterialInstance)
		{
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), ShadowRenderTarget, ShadowRenderMaterialInstance);
		}
	}
}


bool AVC_Clouds::HasGameWorld() const
{
	if (!GetWorld())
	{
		return false;
	}

	return GetWorld()->IsGameWorld();
}

