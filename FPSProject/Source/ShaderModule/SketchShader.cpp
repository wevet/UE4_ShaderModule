// Fill out your copyright notice in the Description page of Project Settings.


#include "SketchShader.h"

#pragma region Vertex
FSketchShaderVS::FSketchShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer)
{
}


void FSketchShaderVS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}
#pragma endregion


#pragma region Pixel
FSketchShaderPS::FSketchShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer)
{
}


void FSketchShaderPS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}
#pragma endregion



IMPLEMENT_SHADER_TYPE(, FSketchShaderVS, TEXT("/Project/SketchShader.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FSketchShaderPS, TEXT("/Project/SketchShader.usf"), TEXT("MainPS"), SF_Pixel);

