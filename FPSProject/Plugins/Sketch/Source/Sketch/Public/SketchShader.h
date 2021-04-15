// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHIDefinitions.h"
#include "RenderResource.h"
#include "ShaderCompilerCore.h"

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FConstantParameters, )
	SHADER_PARAMETER(int, actorsNum)
	SHADER_PARAMETER(FVector4, resolution)
	SHADER_PARAMETER(FVector4, viewOrigin)
	SHADER_PARAMETER(FMatrix, viewMatrix)
	SHADER_PARAMETER(FMatrix, projMatrix)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FVariableParameters, )
	SHADER_PARAMETER(float, time)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

typedef TUniformBufferRef<FConstantParameters> FConstantParametersRef;
typedef TUniformBufferRef<FVariableParameters> FVariableParametersRef;


/*
*	VertexShader
*/
class FSketchShaderVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FSketchShaderVS, Global);

public:
	FSketchShaderVS() 
	{
	}

	FSketchShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCache(EShaderPlatform Platform) 
	{
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5); 
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& PermutationParams) 
	{
		return true; 
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	void SetUniformBuffers(FRHICommandList& CommandList, FConstantParameters& Constants, FVariableParameters& Variables);
	void SetStructuredBuffers(FRHICommandList& CommandList, FShaderResourceViewRHIRef& StructuredBuffer);

protected:
	LAYOUT_FIELD(FShaderResourceParameter, SrcBuffer);
};


/*
*	PixelShader
*/
class FSketchShaderPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FSketchShaderPS, Global);

public:
	FSketchShaderPS() 
	{
	}
	
	FSketchShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCache(EShaderPlatform Platform) 
	{
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& PermutationParams) 
	{
		return true; 
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);


public:
	void SetMainTexture(FRHICommandListImmediate& CommandList, const FTexture2DRHIRef InMainTexture);
	void SetMainColor(FRHICommandListImmediate& CommandList, const FLinearColor& InColor);
	void SetUniformBuffers(FRHICommandList& CommandList, FConstantParameters& Constants, FVariableParameters& Variables);
	void SetStructuredBuffers(FRHICommandList& CommandList, FShaderResourceViewRHIRef& StructuredBuffer);

protected:
	LAYOUT_FIELD(FShaderParameter, MainColor);
	LAYOUT_FIELD(FShaderResourceParameter, MainTexture);
	LAYOUT_FIELD(FShaderResourceParameter, SrcBuffer);
};


struct FSketchVertex
{
	FVector4 Position;
	FVector2D UV;
};


class FSketchVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FSketchVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FSketchVertex, Position), VET_Float4, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FSketchVertex, UV), VET_Float2, 1, Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI->Release();
	}
};

