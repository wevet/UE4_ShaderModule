
#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "CloudStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class VOLUMETRICCLOUDS_API ACloudStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	ACloudStaticMeshActor(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
};
