// Fill out your copyright notice in the Description page of Project Settings.


#include "SketchShader.h"

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FConstantParameters, "constants");
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FVariableParameters, "variables");

#pragma region Vertex
FSketchShaderVS::FSketchShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	SrcBuffer.Bind(Initializer.ParameterMap, TEXT("sketchData"));
}


void FSketchShaderVS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}


void FSketchShaderVS::SetUniformBuffers(FRHICommandList& CommandList, FConstantParameters& Constants, FVariableParameters& Variables)
{
	FRHIVertexShader* VertexShaderRHI = CommandList.GetBoundVertexShader();
	SetUniformBufferParameter(
		CommandList,
		VertexShaderRHI,
		GetUniformBufferParameter<FConstantParameters>(),
		FConstantParametersRef::CreateUniformBufferImmediate(Constants, UniformBuffer_SingleDraw));

	SetUniformBufferParameter(
		CommandList,
		VertexShaderRHI,
		GetUniformBufferParameter<FVariableParameters>(),
		FVariableParametersRef::CreateUniformBufferImmediate(Variables, UniformBuffer_SingleDraw));
}


void FSketchShaderVS::SetStructuredBuffers(FRHICommandList& CommandList, FShaderResourceViewRHIRef& StructuredBuffer)
{
	FRHIVertexShader* VertexShaderRHI = CommandList.GetBoundVertexShader();
	if (SrcBuffer.IsBound())
	{
		CommandList.SetShaderResourceViewParameter(VertexShaderRHI, SrcBuffer.GetBaseIndex(), StructuredBuffer);
	}
}
#pragma endregion


#pragma region Pixel
FSketchShaderPS::FSketchShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	MainColor.Bind(Initializer.ParameterMap, TEXT("mainColor"));
	MainTexture.Bind(Initializer.ParameterMap, TEXT("mainTexture"));
	SrcBuffer.Bind(Initializer.ParameterMap, TEXT("sketchData"));
}


void FSketchShaderPS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}


void FSketchShaderPS::SetMainTexture(FRHICommandListImmediate& CommandList, const FTexture2DRHIRef InMainTexture)
{
	FRHIPixelShader* PixelShaderRHI = CommandList.GetBoundPixelShader();
	SetTextureParameter(CommandList, PixelShaderRHI, MainTexture, InMainTexture);
}


void FSketchShaderPS::SetMainColor(FRHICommandListImmediate& CommandList, const FLinearColor& InColor)
{
	FRHIPixelShader* PixelShaderRHI = CommandList.GetBoundPixelShader();
	SetShaderValue(CommandList, PixelShaderRHI, MainColor, InColor);
}


void FSketchShaderPS::SetUniformBuffers(FRHICommandList& CommandList, FConstantParameters& Constants, FVariableParameters& Variables)
{
	FRHIPixelShader* PixelShaderRHI = CommandList.GetBoundPixelShader();
	SetUniformBufferParameter(
		CommandList,
		PixelShaderRHI,
		GetUniformBufferParameter<FConstantParameters>(),
		FConstantParametersRef::CreateUniformBufferImmediate(Constants, UniformBuffer_SingleDraw));

	SetUniformBufferParameter(
		CommandList,
		PixelShaderRHI,
		GetUniformBufferParameter<FVariableParameters>(),
		FVariableParametersRef::CreateUniformBufferImmediate(Variables, UniformBuffer_SingleDraw));
}


void FSketchShaderPS::SetStructuredBuffers(FRHICommandList& CommandList, FShaderResourceViewRHIRef& StructuredBuffer)
{
	FRHIPixelShader* PixelShaderRHI = CommandList.GetBoundPixelShader();
	if (SrcBuffer.IsBound())
	{
		CommandList.SetShaderResourceViewParameter(PixelShaderRHI, SrcBuffer.GetBaseIndex(), StructuredBuffer);
	}
}
#pragma endregion


IMPLEMENT_SHADER_TYPE(, FSketchShaderVS, TEXT("/Plugins/Sketch/Shader/SketchShader.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FSketchShaderPS, TEXT("/Plugins/Sketch/Shader/SketchShader.usf"), TEXT("MainPS"), SF_Pixel);

