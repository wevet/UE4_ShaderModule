// Copyright Epic Games, Inc. All Rights Reserved.

#include "GlobalShader.h"
#include "RHIDefinitions.h"
#include "RenderResource.h"
#include "MeshDistortionAPI.h"
#include "ShaderCompilerCore.h"


class FMeshDistortionBaseShader : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FMeshDistortionBaseShader, NonVirtual);

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}


	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.SetDefine(TEXT("GRID_SUBDIVISION_X"), GRID_SUBDIVITIONX);
		OutEnvironment.SetDefine(TEXT("GRID_SUBDIVISION_Y"), GRID_SUBDIVITIONY);
	}

	FMeshDistortionBaseShader() 
	{
		//
	}

	FMeshDistortionBaseShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		PixelUVSize.Bind(Initializer.ParameterMap, TEXT("PixelUVSize"));
		RadialDistortionCoefs.Bind(Initializer.ParameterMap, TEXT("RadialDistortionCoefs"));
		TangentialDistortionCoefs.Bind(Initializer.ParameterMap, TEXT("TangentialDistortionCoefs"));
		DistortedCameraMatrix.Bind(Initializer.ParameterMap, TEXT("DistortedCameraMatrix"));
		UndistortedCameraMatrix.Bind(Initializer.ParameterMap, TEXT("UndistortedCameraMatrix"));
		OutputMultiplyAndAdd.Bind(Initializer.ParameterMap, TEXT("OutputMultiplyAndAdd"));
	}


	template<typename TShaderRHIParamRef>
	void SetParameters(FRHICommandListImmediate& RHICmdList, const TShaderRHIParamRef ShaderRHI, const FCompiledMeshModel& Model, const FIntPoint& DisplacementMapResolution)
	{
		const FVector2D PixelUVSizeValue(1.f / float(DisplacementMapResolution.X), 1.f / float(DisplacementMapResolution.Y));
		const FVector RadialDistortionCoefsValue(Model.OriginalModel.K1, Model.OriginalModel.K2, Model.OriginalModel.K3);
		const FVector2D TangentialDistortionCoefsValue(Model.OriginalModel.P1, Model.OriginalModel.P2);

		SetShaderValue(RHICmdList, ShaderRHI, PixelUVSize, PixelUVSizeValue);
		SetShaderValue(RHICmdList, ShaderRHI, DistortedCameraMatrix, Model.DistortedMatrix);
		SetShaderValue(RHICmdList, ShaderRHI, UndistortedCameraMatrix, Model.UndistortedMatrix);
		SetShaderValue(RHICmdList, ShaderRHI, RadialDistortionCoefs, RadialDistortionCoefsValue);
		SetShaderValue(RHICmdList, ShaderRHI, TangentialDistortionCoefs, TangentialDistortionCoefsValue);
		SetShaderValue(RHICmdList, ShaderRHI, OutputMultiplyAndAdd, Model.OutputMultiplyAndAdd);
	}

private:
	LAYOUT_FIELD(FShaderParameter, PixelUVSize);
	LAYOUT_FIELD(FShaderParameter, RadialDistortionCoefs);
	LAYOUT_FIELD(FShaderParameter, TangentialDistortionCoefs);
	LAYOUT_FIELD(FShaderParameter, DistortedCameraMatrix);
	LAYOUT_FIELD(FShaderParameter, UndistortedCameraMatrix);
	LAYOUT_FIELD(FShaderParameter, OutputMultiplyAndAdd);
};


/*
*	VertexShader
*/
class FMeshDistortionVSShader : public FMeshDistortionBaseShader
{
	DECLARE_SHADER_TYPE(FMeshDistortionVSShader, Global);
public:

	FMeshDistortionVSShader() 
	{
	}

	FMeshDistortionVSShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMeshDistortionBaseShader(Initializer)
	{
	}
};


/*
*	PixelShader
*/
class FMeshDistortionPSShader : public FMeshDistortionBaseShader
{
	DECLARE_SHADER_TYPE(FMeshDistortionPSShader, Global);
public:

	FMeshDistortionPSShader() 
	{
	}

	FMeshDistortionPSShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMeshDistortionBaseShader(Initializer)
	{ 
	}
};


IMPLEMENT_SHADER_TYPE(, FMeshDistortionVSShader, TEXT("/Plugins/MeshDistortion/Shader/MeshDistortion.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FMeshDistortionPSShader, TEXT("/Plugins/MeshDistortion/Shader/MeshDistortion.usf"), TEXT("MainPS"), SF_Pixel)


