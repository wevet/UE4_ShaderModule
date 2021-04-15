// Fill out your copyright notice in the Description page of Project Settings.


#include "SketchComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SketchShader.h"
#include "RHIResources.h"
#include "MeshPassProcessor.h"

#define GETSAFERHISHADER_PIXEL(Shader) ((Shader) ? (Shader)->GetPixelShader() : nullptr)
#define GETSAFERHISHADER_VERTEX(Shader) ((Shader) ? (Shader)->GetVertexShader() : nullptr)



USketchComponent::USketchComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void USketchComponent::BeginPlay()
{
	Super::BeginPlay();
}


void USketchComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!RenderTexture)
	{
		return;
	}

	auto Self = this;
	auto RenderTargetResource = RenderTexture->GameThread_GetRenderTargetResource();

	ENQUEUE_RENDER_COMMAND(FRaymarchingPostprocess)
	([Self, RenderTargetResource](FRHICommandListImmediate& RHICmdList)
	{
		Self->ExecuteInRenderThread(RHICmdList, RenderTargetResource);
	});
}


void USketchComponent::ExecuteInRenderThread(FRHICommandListImmediate& RHICmdList, FTextureRenderTargetResource* OutputRenderTargetResource)
{
	check(IsInRenderingThread());

#if WANTS_DRAW_MESH_EVENTS
	FString EventName;
	RenderTexture->GetFName().ToString(EventName);
	SCOPED_DRAW_EVENTF(RHICmdList, SceneCapture, TEXT("Sketch %s"), *EventName);

#else
	SCOPED_DRAW_EVENT(RHICmdList, DrawUVDisplacementToRenderTarget_RenderThread);
#endif

	// SetRT
	FRHIRenderPassInfo RPInfo(OutputRenderTargetResource->GetRenderTargetTexture(), ERenderTargetActions::DontLoad_DontStore);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("Sketch"));

	// Shader setup
	FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(ERHIFeatureLevel::SM5);
	TShaderMapRef<FSketchShaderVS> VertexShader(ShaderMap);
	TShaderMapRef<FSketchShaderPS> PixelShader(ShaderMap);

	auto PixelShaderPtr = (&PixelShader)->GetPixelShader();
	if (PixelShaderPtr)
	{
		if (MainTexture)
		{
			FTexture2DRHIRef Texture = MainTexture->Resource->TextureRHI->GetTexture2D();
			PixelShader->SetMainTexture(RHICmdList, PixelShaderPtr, Texture);
		}
		PixelShader->SetMainColor(RHICmdList, PixelShaderPtr, MainColor);
	}


	FSketchVertexDeclaration VertexDec;
	VertexDec.InitRHI();

	// Declare a pipeline state object that holds all the rendering state
	FGraphicsPipelineStateInitializer PipelineStateInitializer;
	RHICmdList.ApplyCachedRenderTargets(PipelineStateInitializer);
	PipelineStateInitializer.PrimitiveType = PT_TriangleList;
	PipelineStateInitializer.BoundShaderState.VertexDeclarationRHI = VertexDec.VertexDeclarationRHI;

	// ver 4.26 nothings macros
	PipelineStateInitializer.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(&VertexShader);
	PipelineStateInitializer.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(&PixelShader);
	
	PipelineStateInitializer.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	PipelineStateInitializer.BlendState = TStaticBlendState<>::GetRHI();
	PipelineStateInitializer.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	SetGraphicsPipelineState(RHICmdList, PipelineStateInitializer);

	static const FSketchVertex Vertices[4] = {
		{ FVector4(-1.0f,  1.0f, 0.0f, 1.0f), FVector2D(0.0f, 0.0f)},
		{ FVector4(1.0f,  1.0f, 0.0f, 1.0f), FVector2D(1.0f, 0.0f)},
		{ FVector4(-1.0f, -1.0f, 0.0f, 1.0f), FVector2D(0.0f, 1.0f)},
		{ FVector4(1.0f, -1.0f, 0.0f, 1.0f), FVector2D(1.0f, 1.0f)},
	};

	static const uint16 Indices[6] = {
		0, 1, 2,
		2, 1, 3
	};

	DrawIndexedPrimitiveUP(RHICmdList, PT_TriangleList, 0, UE_ARRAY_COUNT(Vertices), 2, Indices, sizeof(Indices[0]), Vertices, sizeof(Vertices[0]));

	// Resolve render target.  
	RHICmdList.CopyToResolveTarget(OutputRenderTargetResource->GetRenderTargetTexture(), OutputRenderTargetResource->TextureRHI, FResolveParams());
	RHICmdList.EndRenderPass();
}


void USketchComponent::DrawIndexedPrimitiveUP(FRHICommandList& RHICmdList, uint32 PrimitiveType, uint32 MinVertexIndex, uint32 NumVertices, uint32 NumPrimitives, const void* IndexData, uint32 IndexDataStride, const void* VertexData, uint32 VertexDataStride)
{
	const uint32 NumIndices = GetVertexCountForPrimitiveCount(NumPrimitives, PrimitiveType);
	FRHIResourceCreateInfo CreateInfo;


	FVertexBufferRHIRef VertexBufferRHI = RHICreateVertexBuffer(VertexDataStride * NumVertices, BUF_Volatile, CreateInfo);
	{
		void* VoidPtr = RHILockVertexBuffer(VertexBufferRHI, 0, VertexDataStride * NumVertices, RLM_WriteOnly);
		FPlatformMemory::Memcpy(VoidPtr, VertexData, VertexDataStride * NumVertices);
	}
	RHIUnlockVertexBuffer(VertexBufferRHI);


	FIndexBufferRHIRef IndexBufferRHI = RHICreateIndexBuffer(IndexDataStride, IndexDataStride * NumIndices, BUF_Volatile, CreateInfo);
	{
		void* VoidPtr = RHILockIndexBuffer(IndexBufferRHI, 0, IndexDataStride * NumIndices, RLM_WriteOnly);
		FPlatformMemory::Memcpy(VoidPtr, IndexData, IndexDataStride * NumIndices);
	}
	RHIUnlockIndexBuffer(IndexBufferRHI);

	RHICmdList.SetStreamSource(0, VertexBufferRHI, 0);
	RHICmdList.DrawIndexedPrimitive(IndexBufferRHI, MinVertexIndex, 0, NumVertices, 0, NumPrimitives, 1);

	IndexBufferRHI.SafeRelease();
	VertexBufferRHI.SafeRelease();
}

