
#include "VolumetricCloudsPainter.h"
#include "VolumetricCloudsPainterEdMode.h"

#define LOCTEXT_NAMESPACE "FVolumetricCloudsPainterModule"

void FVolumetricCloudsPainterModule::StartupModule()
{
	FEditorModeRegistry::Get().RegisterMode<FVolumetricCloudsPainterEdMode>(FVolumetricCloudsPainterEdMode::EM_VolumetricCloudsPainterEdModeId, LOCTEXT("VolumetricCloudsPainterEdModeName", "VolumetricCloudsPainterEdMode"), FSlateIcon(), true);
	UE_LOG(LogTemp, Warning, TEXT("VolumetricCloudsPainterModule->StartupModule"));
}

void FVolumetricCloudsPainterModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FVolumetricCloudsPainterEdMode::EM_VolumetricCloudsPainterEdModeId);
	UE_LOG(LogTemp, Warning, TEXT("VolumetricCloudsPainterModule->ShutdownModule"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVolumetricCloudsPainterModule, VolumetricCloudsPainter)
