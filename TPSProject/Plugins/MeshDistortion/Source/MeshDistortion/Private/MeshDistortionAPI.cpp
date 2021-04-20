// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeshDistortionAPI.h"
#include "MeshDistortionBlueprintLibrary.h"
#include "MeshDistortionShader.h"


#define LOCTEXT_NAMESPACE "MeshDistortionPlugin"


#pragma region Public
static void DrawToRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	const FCompiledMeshModel& CompiledModel,
	const FName& TargetName,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel)
{
	check(IsInRenderingThread());

#if WANTS_DRAW_MESH_EVENTS
	FString EventName;
	TargetName.ToString(EventName);
	SCOPED_DRAW_EVENTF(RHICmdList, SceneCapture, TEXT("MeshDistortion %s"), *EventName);
#else
	SCOPED_DRAW_EVENT(RHICmdList, DrawToRenderTarget_RenderThread);
#endif

	FRHITexture2D* RenderTargetTexture = OutTextureRenderTargetResource->GetRenderTargetTexture();

	RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::SRVMask, ERHIAccess::RTV));

	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("DrawUVDisplacement"));

	{
		FIntPoint DisplacementMapResolution(OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY());

		RHICmdList.SetViewport(0, 0, 0.f, DisplacementMapResolution.X, DisplacementMapResolution.Y, 1.f);

		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		TShaderMapRef<FMeshDistortionVSShader> VertexShader(GlobalShaderMap);
		TShaderMapRef<FMeshDistortionPSShader> PixelShader(GlobalShaderMap);

		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

		RHICmdList.SetViewport(0, 0, 0.f, OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY(), 1.f);
		VertexShader->SetParameters(RHICmdList, VertexShader.GetVertexShader(), CompiledModel, DisplacementMapResolution);
		PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(), CompiledModel, DisplacementMapResolution);

		const uint32 PrimitiveCount = GRID_SUBDIVITIONX * GRID_SUBDIVITIONY * 2;
		RHICmdList.DrawPrimitive(0, PrimitiveCount, 1);
	}
	
	RHICmdList.EndRenderPass();
	RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::RTV, ERHIAccess::SRVMask));
}


static const FVector2D UnDistortionUVIntoViewSpace(
	const FMeshDistortionModel& CameraModel,
	const FVector2D ViewportUV,
	const float HorizontalFOV,
	const float AspectRatio)
{
	const FVector2D AspectRatioAwareF = CameraModel.F * FVector2D(1, -AspectRatio);
	return CameraModel.UnDistortionNormalizedViewPosition((ViewportUV - CameraModel.C) / AspectRatioAwareF);
}
#pragma endregion


FVector2D FMeshDistortionModel::UnDistortionNormalizedViewPosition(const FVector2D EngineV) const
{
	const FVector2D V = FVector2D(1, -1) * EngineV;
	const FVector2D V2 = V * V;
	const float R2 = V2.X + V2.Y;
	FVector2D UndistortedV = V * (1.0 + (R2 * K1 + (R2 * R2) * K2 + (R2 * R2 * R2) * K3));
	UndistortedV.X += P2 * (R2 + 2 * V2.X) + 2 * P1 * V.X * V.Y;
	UndistortedV.Y += P1 * (R2 + 2 * V2.Y) + 2 * P2 * V.X * V.Y;
	return UndistortedV * FVector2D(1, -1);
}


float FMeshDistortionModel::GetUnDistortionOverScanFactor(const float HorizontalFOV, const float AspectRatio) const
{
	if (*this == FMeshDistortionModel())
	{
		return 1.0f;
	}

	const float TanHalfHFOV = FMath::Tan(HorizontalFOV * 0.5f);
	const float TanHalfVFOV = TanHalfHFOV / AspectRatio;

	const FVector2D CornerPos0 = UnDistortionUVIntoViewSpace(*this, FVector2D(0.0f, 0.0f), TanHalfHFOV, AspectRatio);
	const FVector2D CornerPos1 = UnDistortionUVIntoViewSpace(*this, FVector2D(0.5f, 0.0f), TanHalfHFOV, AspectRatio);
	const FVector2D CornerPos2 = UnDistortionUVIntoViewSpace(*this, FVector2D(1.0f, 0.0f), TanHalfHFOV, AspectRatio);
	const FVector2D CornerPos3 = UnDistortionUVIntoViewSpace(*this, FVector2D(1.0f, 0.5f), TanHalfHFOV, AspectRatio);
	const FVector2D CornerPos4 = UnDistortionUVIntoViewSpace(*this, FVector2D(1.0f, 1.0f), TanHalfHFOV, AspectRatio);
	const FVector2D CornerPos5 = UnDistortionUVIntoViewSpace(*this, FVector2D(0.5f, 1.0f), TanHalfHFOV, AspectRatio);
	const FVector2D CornerPos6 = UnDistortionUVIntoViewSpace(*this, FVector2D(0.0f, 1.0f), TanHalfHFOV, AspectRatio);
	const FVector2D CornerPos7 = UnDistortionUVIntoViewSpace(*this, FVector2D(0.0f, 0.5f), TanHalfHFOV, AspectRatio);

	FVector2D MinViewport;
	FVector2D MaxViewport;
	MinViewport.X = FMath::Max3(CornerPos0.X, CornerPos6.X, CornerPos7.X);
	MinViewport.Y = FMath::Max3(CornerPos4.Y, CornerPos5.Y, CornerPos6.Y);
	MaxViewport.X = FMath::Min3(CornerPos2.X, CornerPos3.X, CornerPos4.X);
	MaxViewport.Y = FMath::Min3(CornerPos0.Y, CornerPos1.Y, CornerPos2.Y);

	check(MinViewport.X < 0.f);
	check(MinViewport.Y < 0.f);
	check(MaxViewport.X > 0.f);
	check(MaxViewport.Y > 0.f);

	const float X = TanHalfHFOV / FMath::Max(-MinViewport.X, MaxViewport.X);
	const float Y = TanHalfVFOV / FMath::Max(-MinViewport.Y, MaxViewport.Y);
	const FVector2D ViewportScaleUpAxis = 0.5 * FVector2D(X, Y);
	const float ViewportScaleMultiplier = 1.02f;
	return FMath::Max(ViewportScaleUpAxis.X, ViewportScaleUpAxis.Y) * ViewportScaleMultiplier;
}


void FMeshDistortionModel::DrawToRenderTarget(
	class UWorld* World,
	class UTextureRenderTarget2D* RenderTarget,
	const float HorizontalFOV,
	const float AspectRatio,
	const float Factor,
	float OutputMultiply,
	float OutputAdd) const
{
	check(IsInGameThread());

	if (!RenderTarget)
	{
		FMessageLog("Blueprint").Warning(LOCTEXT("MeshDistortionModel_DrawUVDisplacementToRenderTarget", "DrawUVDisplacementToRenderTarget: Output render target is required."));
		return;
	}

	const float TanHalfHFOV = FMath::Tan(HorizontalFOV * 0.5f) * Factor;
	const float TanHalfVFOV = TanHalfHFOV / AspectRatio;

	FCompiledMeshModel MeshModel;
	MeshModel.OriginalModel = *this;
	MeshModel.DistortedMatrix.X = 1.0f / TanHalfHFOV;
	MeshModel.DistortedMatrix.Y = 1.0f / TanHalfVFOV;
	MeshModel.DistortedMatrix.Z = 0.5f;
	MeshModel.DistortedMatrix.W = 0.5f;

	MeshModel.UndistortedMatrix.X = F.X;
	MeshModel.UndistortedMatrix.Y = F.Y * AspectRatio;
	MeshModel.UndistortedMatrix.Z = C.X;
	MeshModel.UndistortedMatrix.W = C.Y;

	MeshModel.OutputMultiplyAndAdd.X = OutputMultiply;
	MeshModel.OutputMultiplyAndAdd.Y = OutputAdd;

	const FName TargetName = RenderTarget->GetFName();
	FTextureRenderTargetResource* TargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();

	if (FeatureLevel < ERHIFeatureLevel::SM5)
	{
		FMessageLog("Blueprint").Warning(LOCTEXT("MeshDistortionModel_DrawUVDisplacementToRenderTarget", "DrawUVDisplacementToRenderTarget: Requires RHIFeatureLevel::SM5 which is unavailable."));
		return;
	}

	ENQUEUE_RENDER_COMMAND(CaptureCommand)
	([MeshModel, TargetResource, TargetName, FeatureLevel]
	(FRHICommandListImmediate& RHICmdList)
	{
		DrawToRenderTarget_RenderThread(RHICmdList, MeshModel, TargetName, TargetResource, FeatureLevel);
	});

}


#undef LOCTEXT_NAMESPACE
