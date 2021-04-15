// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SketchComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SKETCH_API USketchComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USketchComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Sketch)
	class UTextureRenderTarget2D* RenderTexture;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Sketch)
	class UTexture2D* MainTexture;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Sketch)
	FLinearColor MainColor;

protected:
	void ExecuteInRenderThread(FRHICommandListImmediate& RHICmdList, FTextureRenderTargetResource* OutputRenderTargetResource);
	void DrawIndexedPrimitiveUP(FRHICommandList& RHICmdList, uint32 PrimitiveType, uint32 MinVertexIndex, uint32 NumVertices, uint32 NumPrimitives, const void* IndexData, uint32 IndexDataStride, const void* VertexData, uint32 VertexDataStride);
};
