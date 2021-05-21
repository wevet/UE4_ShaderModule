

#include "CloudStaticMeshActor.h"


ACloudStaticMeshActor::ACloudStaticMeshActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


void ACloudStaticMeshActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}


void ACloudStaticMeshActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void ACloudStaticMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACloudStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

