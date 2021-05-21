// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VolumetricCloudsPainterEdMode.h"
#include "VolumetricCloudsPainterEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

#include "Engine/Selection.h"
#include "Engine/Canvas.h"

#include "EditorViewportClient.h"
#include "LevelEditorViewport.h"
#include "CanvasTypes.h"

#include "SlateBasics.h" 
#include "LevelEditor.h"
#include "SLevelViewport.h"

#include "Blueprint/WidgetLayoutLibrary.h"

#include "Kismet/KismetRenderingLibrary.h"

#include "FileHelpers.h"
#include "UObject/Package.h"
#include "AssetRegistryModule.h"



#include "EngineUtils.h"


const FEditorModeID FVolumetricCloudsPainterEdMode::EM_VolumetricCloudsPainterEdModeId = TEXT("EM_VolumetricCloudsPainterEdMode");


FVolumetricCloudsPainterEdMode::FVolumetricCloudsPainterEdMode()
{
	PreviousMousePosition = FVector2D(-10000.0, -10000.0);
	BrushColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	FinalTexture = nullptr;
	CloudsActor = nullptr;
	CloudsMaterial = nullptr;
	bTextureNeedToLoad = true;
	bPressedLMB = false;
	bPainiting = false;
	bAdditivePaint = true;
	bRedChannelEnabled = true;
	bGreenChannelEnabled = true;
	bBlueChannelEnabled = false;
	bAlphaChannelEnabled = false;
	BrushRadius = 0.1f;
	BrushFalloff = 1.0f;
	BrushOpacity = 0.25f;


	ColorBlendMaterialInstance = Cast<UMaterialInstanceConstant>(
		StaticLoadObject(
			UMaterialInstanceConstant::StaticClass(),
			nullptr,
			TEXT("MaterialInstanceConstant'/VolumetricClouds/Painter/Materials/MI_ColorBlend.MI_ColorBlend'")));

	AlphaBlendMaterialInstance = Cast<UMaterialInstanceConstant>(
		StaticLoadObject(
			UMaterialInstanceConstant::StaticClass(), 
			nullptr,
			TEXT("MaterialInstanceConstant'/VolumetricClouds/Painter/Materials/MI_AlphaBlend.MI_AlphaBlend'")));

	AlphCombineMaterial = Cast<UMaterial>(
		StaticLoadObject(
			UMaterial::StaticClass(), 
			nullptr,
			TEXT("Material'/VolumetricClouds/Painter/Materials/M_AlphaCombine.M_AlphaCombine'")));

	ColorBlendRenderTarget = Cast<UTextureRenderTarget2D>(
		StaticLoadObject(
			UTextureRenderTarget2D::StaticClass(), 
			nullptr,
			TEXT("TextureRenderTarget2D'/VolumetricClouds/Painter/Textures/RT_ColorBlend.RT_ColorBlend'")));

	AlphaBlendRenderTarget = Cast<UTextureRenderTarget2D>(
		StaticLoadObject(
			UTextureRenderTarget2D::StaticClass(),
			nullptr,
			TEXT("TextureRenderTarget2D'/VolumetricClouds/Painter/Textures/RT_AlphaBlend.RT_AlphaBlend'")));

	RenderTarget = Cast<UTextureRenderTarget2D>(
		StaticLoadObject(
			UTextureRenderTarget2D::StaticClass(),
			nullptr,
			TEXT("TextureRenderTarget2D'/VolumetricClouds/Painter/Textures/RT_FinalRenderTarget.RT_FinalRenderTarget'")));
}


FVolumetricCloudsPainterEdMode::~FVolumetricCloudsPainterEdMode()
{

}


#pragma region Override
void FVolumetricCloudsPainterEdMode::Enter()
{
	FEdMode::Enter();

	GetCloudsActor();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FVolumetricCloudsPainterEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}


void FVolumetricCloudsPainterEdMode::Exit()
{
	ReleaseCloudsActor();
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}
	FEdMode::Exit();
}


void FVolumetricCloudsPainterEdMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	if (!CloudsActor || !CloudsMaterial || !IsPainiting())
	{
		return;
	}

	if (!GEditor || !GEditor->GetActiveViewport() || !GEditor->GetActiveViewport()->GetClient())
	{
		return;
	}

	FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
	const FVector ViewPosition = ViewportClient->GetViewLocation();
	const FVector ViewDirection = ViewportClient->GetCursorWorldLocationFromMousePos().GetDirection();

	float WeatherMapSize = 1000.0;
	CloudsMaterial->GetScalarParameterValue(FMaterialParameterInfo("WeatherMapSize"), WeatherMapSize);

	const FVector CloudsPosition = CloudsActor->GetActorLocation();
	float GroundCloudsHeight = CloudsPosition.Z;
	//CloudsMaterial->GetScalarParameterValue(FMaterialParameterInfo("GroundCloudsHeight"), GroundCloudsHeight);
	float CloudsHeight = CloudsActor->GetActorScale3D().Z * 100.0f;
	//CloudsMaterial->GetScalarParameterValue(FMaterialParameterInfo("CloudsHeight"), CloudsHeight);

	const float RayLength = FVector::DotProduct(
		CloudsPosition - ViewPosition, 
		FVector(0, 0, 1)) * (1.0f / FVector::DotProduct(ViewDirection, FVector(0, 0, 1)));

	WorldBrushPos = ViewPosition + ViewDirection * RayLength;
	float WorldBrushRadius = (BrushRadius / 2.0f) * 1000000.0f * WeatherMapSize;

	const float RenderHelpersThickness = FMath::Lerp(10.0f, 1000.0f, RayLength / GroundCloudsHeight);
	const FLinearColor RenderHelpersColor = FLinearColor(0.0f, 1.0f, 1.0f, 0.25f);
	const FVector Normal = FVector(0.0f, 0.0f, 1.0f);

	DrawCircle(PDI, WorldBrushPos, FVector(1, 0, 0), FVector(0, 1, 0), RenderHelpersColor, WorldBrushRadius, 256, ESceneDepthPriorityGroup::SDPG_Foreground, RenderHelpersThickness);
	PDI->DrawLine(WorldBrushPos, WorldBrushPos - Normal * RenderHelpersThickness * 20.0f, RenderHelpersColor, ESceneDepthPriorityGroup::SDPG_Foreground, RenderHelpersThickness);

}


void FVolumetricCloudsPainterEdMode::ActorSelectionChangeNotify()
{

}


void FVolumetricCloudsPainterEdMode::DrawHUD(FEditorViewportClient* ViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (!IsPainiting())
	{
		return;
	}

	FIntPoint BaseMousePos;
	Viewport->GetMousePos(BaseMousePos);

	FVector2D MousePos = BaseMousePos;
	MousePos = MousePos / Canvas->GetDPIScale();
	const FString PaintModeTooltip = (!bAdditivePaint) ? FString("SUB") : FString("ADD");
	Canvas->DrawShadowedString(MousePos.X + 10.0f, MousePos.Y + 10.0f, *PaintModeTooltip, GEngine->GetSmallFont(), FLinearColor(1, 1, 1, 1));
}


bool FVolumetricCloudsPainterEdMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::Escape && Event == EInputEvent::IE_Pressed)
	{
		SetPaintState(false);
	}

	if (CloudsActor != nullptr)
	{
		if (IsPainiting())
		{
			if (Key == EKeys::LeftMouseButton)
			{
				if (Event == EInputEvent::IE_Pressed)
				{
					bPressedLMB = true;
					return true;
				}

				if (Event == EInputEvent::IE_Released)
				{
					bPressedLMB = false;
					PreviousMousePosition = FVector2D(-10000.0, -10000.0);
					return false;
				}
			}

			if (Key == EKeys::LeftControl)
			{
				if (Event == EInputEvent::IE_Pressed)
				{
					SetPaintMode(false);
				}

				if (Event == EInputEvent::IE_Released)
				{
					SetPaintMode(true);
				}
			}
		}

		if (Key == EKeys::P && Event == EInputEvent::IE_Pressed)
		{
			SetPaintState(!bPainiting);
		}
	}
	return false;
}


void FVolumetricCloudsPainterEdMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	FEdMode::Tick(ViewportClient, DeltaTime);

	if (IsPainiting())
	{
		FViewport* Viewport = ViewportClient->Viewport;
		FIntPoint ViewportSize = Viewport->GetSizeXY();
		FIntPoint BaseMousePos;
		Viewport->GetMousePos(BaseMousePos);

		if (bPressedLMB && PreviousMousePosition != FVector2D(BaseMousePos))
		{
			DrawToRenderTaget();
			PreviousMousePosition = FVector2D(BaseMousePos);
		}
	}
}


bool FVolumetricCloudsPainterEdMode::UsesToolkits() const
{
	return true;
}
#pragma endregion


const bool FVolumetricCloudsPainterEdMode::GetCloudsActor()
{
	bool bCloudsFound = false;

	for (TActorIterator<ACloudStaticMeshActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		ACloudStaticMeshActor* SelectedCloudsActor = Cast<ACloudStaticMeshActor>(*ActorIterator);

		if (!SelectedCloudsActor)
		{
			continue;
		}

		CloudsActor = SelectedCloudsActor;
		CloudsMaterial = Cast<UMaterialInstanceConstant>(CloudsActor->GetStaticMeshComponent()->GetMaterial(0));
		if (CloudsMaterial)
		{
			UTexture* TempTexturePointer;
			const bool bTextureFound = CloudsMaterial->GetTextureParameterValue(FMaterialParameterInfo("WeatherMap"), TempTexturePointer);

			if (bTextureFound)
			{
				FinalTexture = Cast<UTexture2D>(TempTexturePointer);
				LoadTexture();
			}
			bCloudsFound = true;
			break;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CloudsMaterial nullptr"));
		}
	}

	if (!bCloudsFound)
	{
		ReleaseCloudsActor();
	}

	return bCloudsFound;
}


bool FVolumetricCloudsPainterEdMode::IsChannelEnabled(const FName Channel) const
{
	bool bEnabled = true;
	if (Channel == "RedChannel")
	{
		bEnabled = bRedChannelEnabled;
	}
	else if (Channel == "GreenChannel")
	{
		bEnabled = bGreenChannelEnabled;
	}
	else if (Channel == "BlueChannel")
	{
		bEnabled = bBlueChannelEnabled;
	}
	else if (Channel == "AlphaChannel")
	{
		bEnabled = bAlphaChannelEnabled;
	}
	return bEnabled;
}


bool FVolumetricCloudsPainterEdMode::IsPainiting() const
{
	return bPainiting;
};


bool FVolumetricCloudsPainterEdMode::IsAdditivePaint() const
{
	return bAdditivePaint;
};


float FVolumetricCloudsPainterEdMode::GetBrushChannelValue(const FName Channel) const
{
	float Value = 0.0f;

	if (Channel == "RedChannel")
	{
		Value = BrushColor.R;
	}
	else if (Channel == "GreenChannel")
	{
		Value = BrushColor.G;
	}
	else if (Channel == "BlueChannel")
	{
		Value = BrushColor.B;
	}
	else if (Channel == "AlphaChannel")
	{
		Value = BrushColor.A;
	}
	return Value;
}


float FVolumetricCloudsPainterEdMode::GetBrushRadius() const 
{
	return BrushRadius; 
};


float FVolumetricCloudsPainterEdMode::GetBrushFalloff() const 
{
	return BrushFalloff; 
};


float FVolumetricCloudsPainterEdMode::GetBrushOpacity() const 
{
	return BrushOpacity; 
};


void FVolumetricCloudsPainterEdMode::LoadTexture()
{
	if (FinalTexture == nullptr)
	{
		UE_LOG(LogTemp,  Error, TEXT("Failed FinalTexture : %s"), *FString(__FUNCTION__));
		return;
	}
	FinalTextureSize = FVector2D(FinalTexture->GetSizeX(), FinalTexture->GetSizeY());


	if (RenderTarget == nullptr || ColorBlendRenderTarget == nullptr || AlphaBlendRenderTarget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed TextureRenderTarget2D : %s"), *FString(__FUNCTION__));
		return;
	}

	if (ColorBlendMaterialInstance == nullptr || AlphaBlendMaterialInstance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed MaterialInstance : %s"), *FString(__FUNCTION__));
		return;
	}

	RenderTarget->ResizeTarget(FinalTextureSize.X, FinalTextureSize.Y);
	ColorBlendRenderTarget->ResizeTarget(FinalTextureSize.X, FinalTextureSize.Y);
	AlphaBlendRenderTarget->ResizeTarget(FinalTextureSize.X, FinalTextureSize.Y);

	ColorBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushColor"), BrushColor);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushFalloff"), BrushFalloff);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushOpacity"), BrushOpacity * 0.1);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushRadius"), BrushRadius);

	AlphaBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushColor"), BrushColor);
	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushFalloff"), BrushFalloff);
	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushOpacity"), BrushOpacity * 0.1);
	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushRadius"), BrushRadius);

	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("RedChannelEnabled"), (float)bRedChannelEnabled);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("GreenChannelEnabled"), (float)bGreenChannelEnabled);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BlueChannelEnabled"), (float)bBlueChannelEnabled);
	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AlphaChannelEnabled"), (float)bAlphaChannelEnabled);

	UCanvas* DrawCanvas;
	FVector2D DrawSize = FVector2D(1.0f, 1.0f);
	AlphaBlendMaterialInstance->SetTextureParameterValueEditorOnly(FName("TextureToRead"), FinalTexture);
	ColorBlendMaterialInstance->SetTextureParameterValueEditorOnly(FName("TextureToRead"), FinalTexture);

	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("bReadTexture"), 1);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("bReadTexture"), 1);

	FDrawToRenderTargetContext DrawContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ColorBlendRenderTarget, DrawCanvas, DrawSize, DrawContext);
	DrawCanvas->K2_DrawMaterial(ColorBlendMaterialInstance, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), DrawContext);

	FDrawToRenderTargetContext AlphaBlendRenderTargetDrawContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), AlphaBlendRenderTarget, DrawCanvas, DrawSize, AlphaBlendRenderTargetDrawContext);
	DrawCanvas->K2_DrawMaterial(AlphaBlendMaterialInstance, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), AlphaBlendRenderTargetDrawContext);

	FDrawToRenderTargetContext RenderTargetDrawContext;
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTarget, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, DrawCanvas, DrawSize, RenderTargetDrawContext);
	DrawCanvas->K2_DrawMaterial(AlphCombineMaterial, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), RenderTargetDrawContext);
	AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("bReadTexture"), 0);
	ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("bReadTexture"), 0);
}


void FVolumetricCloudsPainterEdMode::ReleaseCloudsActor()
{
	SetPaintState(false);
	CloudsActor = nullptr;
	CloudsMaterial = nullptr;
	FinalTexture = nullptr;
}


void FVolumetricCloudsPainterEdMode::DrawToRenderTaget()
{
	if (CloudsActor != nullptr && CloudsMaterial != nullptr && RenderTarget != nullptr && FinalTexture != nullptr)
	{
		if (ColorBlendRenderTarget != nullptr && AlphaBlendRenderTarget != nullptr && ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
		{
			FVector2D ScreenPosition;
			ScreenPosition.X = WorldBrushPos.X;
			ScreenPosition.Y = WorldBrushPos.Y;

			float WeatherMapSize = 0.0;
			CloudsMaterial->GetScalarParameterValue(FMaterialParameterInfo("WeatherMapTile"), WeatherMapSize);
			float RepeatSize = WeatherMapSize * 1000000.0f;


			ScreenPosition = (ScreenPosition + RepeatSize / 2.0f) / (RepeatSize);

			int BrushPosIndexX = (int)(ScreenPosition.X);
			int BrushPosIndexY = (int)(ScreenPosition.Y);

			ScreenPosition = ScreenPosition - FVector2D(BrushPosIndexX, BrushPosIndexY);

			ColorBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushPosition"), FVector(ScreenPosition.X, ScreenPosition.Y, 0));
			AlphaBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushPosition"), FVector(ScreenPosition.X, ScreenPosition.Y, 0));

			SetPreRenderBrushParameters();

			UCanvas* DrawCanvas;
			FVector2D DrawSize = FVector2D(1.0f, 1.0f);

			FDrawToRenderTargetContext DrawContext;

			UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), ColorBlendRenderTarget, DrawCanvas, DrawSize, DrawContext);
			DrawCanvas->K2_DrawMaterial(ColorBlendMaterialInstance, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
			UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), DrawContext);

			FDrawToRenderTargetContext AlphaBlendRenderTargetDrawContext;

			UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), AlphaBlendRenderTarget, DrawCanvas, DrawSize, AlphaBlendRenderTargetDrawContext);
			DrawCanvas->K2_DrawMaterial(AlphaBlendMaterialInstance, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
			UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), AlphaBlendRenderTargetDrawContext);

			FDrawToRenderTargetContext RenderTargetDrawContext;

			UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTarget, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
			UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, DrawCanvas, DrawSize, RenderTargetDrawContext);
			DrawCanvas->K2_DrawMaterial(AlphCombineMaterial, FVector2D(0.0f, 0.0f), FinalTextureSize, FVector2D(0.0f, 0.0f));
			UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), RenderTargetDrawContext);
		}
	}

}


void FVolumetricCloudsPainterEdMode::SetPreRenderBrushParameters()
{
	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		if (!bAdditivePaint)
		{
			ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AdditivePaint"), -1);
			AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AdditivePaint"), -1);
		}
		else
		{
			ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AdditivePaint"), 1);
			AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AdditivePaint"), 1);
		}

	}
}


void FVolumetricCloudsPainterEdMode::SetBrushRadius(float NewRadius)
{
	BrushRadius = NewRadius;

	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushRadius"), BrushRadius);
		AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushRadius"), BrushRadius);
	}
}


void FVolumetricCloudsPainterEdMode::SetBrushFalloff(float NewFalloff)
{
	BrushFalloff = NewFalloff;

	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushFalloff"), BrushFalloff);
		AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushFalloff"), BrushFalloff);
	}
}


void FVolumetricCloudsPainterEdMode::SetBrushOpacity(float NewOpacity)
{
	BrushOpacity = NewOpacity;

	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushOpacity"), BrushOpacity * 0.1f);
		AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BrushOpacity"), BrushOpacity * 0.1f);
	}
}


void FVolumetricCloudsPainterEdMode::SetBrushChannelValue(float NewValue, FName Channel)
{
	if (Channel == "RedChannel")
	{
		BrushColor.R = NewValue;
	}
	else if (Channel == "GreenChannel")
	{
		BrushColor.G = NewValue;
	}
	else if (Channel == "BlueChannel")
	{
		BrushColor.B = NewValue;
	}
	else if (Channel == "AlphaChannel")
	{
		BrushColor.A = NewValue;
	}

	if (ColorBlendMaterialInstance != nullptr && AlphaBlendMaterialInstance != nullptr)
	{
		ColorBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushColor"), BrushColor);
		AlphaBlendMaterialInstance->SetVectorParameterValueEditorOnly(FName("BrushColor"), BrushColor);
	}
}


void FVolumetricCloudsPainterEdMode::SetPaintMode(const bool NewState)
{
	bAdditivePaint = NewState;
};


void FVolumetricCloudsPainterEdMode::SetPaintState(const bool NewState)
{
	bPainiting = NewState;

	if (!bPainiting)
	{
		if (CloudsMaterial && FinalTexture)
		{
			CloudsMaterial->SetTextureParameterValueEditorOnly(FName("WeatherMap"), FinalTexture);
			UKismetRenderingLibrary::ConvertRenderTargetToTexture2DEditorOnly(GetWorld(), RenderTarget, FinalTexture);
		}
	}
	else
	{
		LoadTexture();
		if (CloudsMaterial && FinalTexture)
		{
			CloudsMaterial->SetTextureParameterValueEditorOnly(FName("WeatherMap"), RenderTarget);
		}
	}

	if (CloudsMaterial)
	{
		CloudsMaterial->PostLoad();
	}
};


void FVolumetricCloudsPainterEdMode::SetChannelState(const bool NewValue, const FName Channel)
{
	if (Channel == "RedChannel")
	{
		bRedChannelEnabled = NewValue;

		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("RedChannelEnabled"), (float)NewValue);

	}
	else if (Channel == "GreenChannel")
	{
		bGreenChannelEnabled = NewValue;
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("GreenChannelEnabled"), (float)NewValue);
	}
	else if (Channel == "BlueChannel")
	{
		bBlueChannelEnabled = NewValue;
		ColorBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("BlueChannelEnabled"), (float)NewValue);
	}
	else if (Channel == "AlphaChannel")
	{
		bAlphaChannelEnabled = NewValue;
		AlphaBlendMaterialInstance->SetScalarParameterValueEditorOnly(FName("AlphaChannelEnabled"), (float)NewValue);
	}

	if (RenderTarget)
	{
		ColorBlendMaterialInstance->SetTextureParameterValueEditorOnly(FName("RenderTarget"), RenderTarget);
		AlphaBlendMaterialInstance->SetTextureParameterValueEditorOnly(FName("RenderTarget"), RenderTarget);
	}
}
