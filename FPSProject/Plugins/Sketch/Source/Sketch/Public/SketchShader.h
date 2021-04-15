// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHIDefinitions.h"
#include "RenderResource.h"
#include "ShaderCompilerCore.h"


/**
 *
 */
class FSketchShaderVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FSketchShaderVS, Global);

public:
	FSketchShaderVS() 
	{

	}

	explicit FSketchShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCache(EShaderPlatform Platform) 
	{
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5); 
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& PermutationParams) 
	{
		return true; 
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

};


class FSketchShaderPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FSketchShaderPS, Global);

public:
	FSketchShaderPS() 
	{
	}
	explicit FSketchShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

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
	template<typename TShaderRHIParamRef>
	void SetMainTexture(FRHICommandListImmediate& RHICmdList, const TShaderRHIParamRef ShaderRHI, const FTexture2DRHIRef InMainTexture)
	{
		SetTextureParameter(RHICmdList, ShaderRHI, MainTexture, InMainTexture);
	}

	template<typename TShaderRHIParamRef>
	void SetMainColor(FRHICommandListImmediate& RHICmdList, const TShaderRHIParamRef ShaderRHI, const FLinearColor& InColor)
	{
		SetShaderValue(RHICmdList, ShaderRHI, MainColor, InColor);
	}

protected:
	LAYOUT_FIELD(FShaderParameter, MainColor);
	LAYOUT_FIELD(FShaderResourceParameter, MainTexture);
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
