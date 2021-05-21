// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "VolumetricCloudsPainterEdModeToolkit.h"
#include "VolumetricCloudsPainterEdMode.h"
#include "Engine/Selection.h"
#include "EditorModeManager.h"

#include "EditorStyleSet.h"
#include "Styling/SlateTypes.h"
#include "SlateFwd.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SNumericEntryBox.h"


#define LOCTEXT_NAMESPACE "FVolumetricCloudsPainterEdModeToolkit"

FVolumetricCloudsPainterEdModeToolkit::FVolumetricCloudsPainterEdModeToolkit()
{
}


#pragma region Override
void FVolumetricCloudsPainterEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{

	const float SlotHeight = 40.0f;

	const FButtonStyle& EditorButtonStyle = FEditorStyle::GetWidgetStyle<FButtonStyle>("Button");
	const FCheckBoxStyle& EditorCheckBoxStyle = FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckbox");

	PaintCheckBoxStyle = FCheckBoxStyle(EditorCheckBoxStyle);
	PaintTypeCheckBoxStyle = FCheckBoxStyle(EditorCheckBoxStyle);
	PaintTypeCheckBoxStyle.SetBorderBackgroundColor(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.9f)));


	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(10, 10))
		.IsEnabled_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::IsActorSelected)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.AutoHeight()
		[
			SNew(SCheckBox)
			.HAlign(HAlign_Center)
		.Style(&PaintCheckBoxStyle)
		.IsEnabled(true)
		.IsChecked_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::IsPainterCheckBoxChecked)
		.OnCheckStateChanged_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::OnPainterCheckBoxStateChanged)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		.Padding(FMargin(10, 10))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::PainterCheckBoxText)
		]
		]
		]

	+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.AutoHeight()
		[
			SNew(SSpacer)
			.Size(FVector2D(5.0f, 5.0f))
		]

	+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			SNew(SCheckBox)
			.HAlign(HAlign_Center)
		.Style(&PaintTypeCheckBoxStyle)
		.IsEnabled(true)
		.IsChecked_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::IsPaintModeCheckBoxChecked)
		.OnCheckStateChanged_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::OnPaintModeCheckBoxStateChanged)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		.Padding(FMargin(5, 5))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text(NSLOCTEXT("CloudsPaintSettings", "PaintTypeLabel", "Paint"))
		]
		]
		]

	+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			SNew(SCheckBox)
			.HAlign(HAlign_Center)
		.Style(&PaintTypeCheckBoxStyle)
		.IsEnabled(true)
		.IsChecked_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::IsEraseModeCheckBoxChecked)
		.OnCheckStateChanged_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::OnEraseModeCheckBoxStateChanged)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		.Padding(FMargin(5, 5))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text(NSLOCTEXT("CloudsPaintSettings", "EraseTypeLabel", "Erase"))
		]
		]
		]
		]

	+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.AutoHeight()
		[
			SNew(SSpacer)
			.Size(FVector2D(10.0f, 10.0f))
		]

	+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.AutoHeight()
		[

			SNew(SSplitter)
			.Style(FEditorStyle::Get(), FName("DetailsView.Splitter"))
		.PhysicalSplitterHandleSize(1.0f)
		.HitDetectionSplitterHandleSize(5.0f)

		+ SSplitter::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text(NSLOCTEXT("CloudsPaintSettings", "BrushRadiusLabel", "BrushRadius"))
		]
		]

	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text(NSLOCTEXT("CloudsPaintSettings", "BrushFalloffLabel", "BrushFalloff"))
		]
		]

	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text(NSLOCTEXT("CloudsPaintSettings", "BrushOpacityLabel", "BrushOpacity"))
		]
		]

	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight)
		.VAlign(VAlign_Top)
		[SNew(SSpacer).Size(FVector2D(10.0f, 10.0f))]


	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text(NSLOCTEXT("CloudsPaintSettings", "RedChannelLabel", "Clouds Probability"))
		]
		]

	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text(NSLOCTEXT("CloudsPaintSettings", "GreenChannelLabel", "Clouds Type"))
		]
		]
		]

	+ SSplitter::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.Value_Lambda([=]() -> float { return ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->GetBrushRadius(); })
		.MinValue(0.0f)
		.MaxSliderValue(1.0f)
		.MaxValue(1.0f)
		.OnValueChanged(SNumericEntryBox<float>::FOnValueChanged::CreateLambda([=](float Value) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetBrushRadius(Value); }))
		.OnValueCommitted(SNumericEntryBox<float>::FOnValueCommitted::CreateLambda([=](float Value, ETextCommit::Type CommitType) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetBrushRadius(Value); }))
		]
		]
		]

	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.Value_Lambda([=]() -> float { return ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->GetBrushFalloff(); })
		.MinValue(0.0f)
		.MaxSliderValue(1.0f)
		.MaxValue(1.0f)
		.OnValueChanged(SNumericEntryBox<float>::FOnValueChanged::CreateLambda([=](float Value) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetBrushFalloff(Value); }))
		.OnValueCommitted(SNumericEntryBox<float>::FOnValueCommitted::CreateLambda([=](float Value, ETextCommit::Type CommitType) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetBrushFalloff(Value); }))
		]
		]
		]

	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.Value_Lambda([=]() -> float { return ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->GetBrushOpacity(); })
		.MinValue(0.0f)
		.MaxSliderValue(1.0f)
		.MaxValue(1.0f)
		.OnValueChanged(SNumericEntryBox<float>::FOnValueChanged::CreateLambda([=](float Value) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetBrushOpacity(Value); }))
		.OnValueCommitted(SNumericEntryBox<float>::FOnValueCommitted::CreateLambda([=](float Value, ETextCommit::Type CommitType) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetBrushOpacity(Value); }))
		]
		]
		]

	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight)
		.VAlign(VAlign_Top)
		[SNew(SSpacer).Size(FVector2D(10.0f, 10.0f))]

	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.Value_Lambda([=]() -> float { return ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->GetBrushChannelValue("RedChannel"); })
		.MinValue(0.0f)
		.MaxValue(1.0f)
		.MaxSliderValue(1.0f)
		.OnValueChanged(SNumericEntryBox<float>::FOnValueChanged::CreateLambda([=](float Value) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetBrushChannelValue(Value, "RedChannel"); }))
		.OnValueCommitted(SNumericEntryBox<float>::FOnValueCommitted::CreateLambda([=](float Value, ETextCommit::Type CommitType) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetBrushChannelValue(Value, "RedChannel"); }))
		]

	+ SHorizontalBox::Slot()
		[SNew(SSpacer).Size(FVector2D(10.0f, 10.0f))]

	+ SHorizontalBox::Slot()
		[
			SNew(SCheckBox)
			.HAlign(HAlign_Center)
		.Style(&PaintCheckBoxStyle)
		.IsEnabled(true)
		.IsChecked_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::IsChannelBoxChecked, FName("RedChannel"))
		.OnCheckStateChanged_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::OnChannelCheckBoxStateChanged, FName("RedChannel"))
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		.Padding(FMargin(0, 0))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text_Lambda([=]() -> FText { return ChannelCheckBoxText("RedChannel"); })
		]
		]
		]

		]
		]

	+ SVerticalBox::Slot()
		.FillHeight(SlotHeight * 2.0)
		.VAlign(VAlign_Top)
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.Value_Lambda([=]() -> float { return ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->GetBrushChannelValue("GreenChannel"); })
		.MinValue(0.0f)
		.MaxValue(1.0f)
		.MaxSliderValue(1.0f)
		.OnValueChanged(SNumericEntryBox<float>::FOnValueChanged::CreateLambda([=](float Value) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetBrushChannelValue(Value, "GreenChannel"); }))
		.OnValueCommitted(SNumericEntryBox<float>::FOnValueCommitted::CreateLambda([=](float Value, ETextCommit::Type CommitType) { ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->GetBrushChannelValue("GreenChannel"); }))
		]

	+ SHorizontalBox::Slot()
		[SNew(SSpacer).Size(FVector2D(10.0f, 10.0f))]

	+ SHorizontalBox::Slot()
		[
			SNew(SCheckBox)
			.HAlign(HAlign_Center)
		.Style(&PaintCheckBoxStyle)
		.IsEnabled(true)
		.IsChecked_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::IsChannelBoxChecked, FName("GreenChannel"))
		.OnCheckStateChanged_Raw(this, &FVolumetricCloudsPainterEdModeToolkit::OnChannelCheckBoxStateChanged, FName("GreenChannel"))
		[
			SNew(SBorder)
			.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
		.Padding(FMargin(0, 0))
		[
			SNew(STextBlock)
			.AutoWrapText(false)
		.Text_Lambda([=]() -> FText { return ChannelCheckBoxText("GreenChannel"); })
		]
		]

		]
		]
		]
		]
		]
	];
	FModeToolkit::Init(InitToolkitHost);
}


FName FVolumetricCloudsPainterEdModeToolkit::GetToolkitFName() const
{
	return FName("VolumetricCloudsPainterEdMode");
}


FText FVolumetricCloudsPainterEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("VolumetricCloudsPainterEdModeToolkit", "DisplayName", "VolumetricCloudsPainterEdMode Tool");
}


class FEdMode* FVolumetricCloudsPainterEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FVolumetricCloudsPainterEdMode::EM_VolumetricCloudsPainterEdModeId);
}
#pragma endregion


bool FVolumetricCloudsPainterEdModeToolkit::IsActorSelected() const
{
	return (((FVolumetricCloudsPainterEdMode*)GetEditorMode()) != nullptr && ((FVolumetricCloudsPainterEdMode*)GetEditorMode())->CloudsActor != nullptr);
}


void FVolumetricCloudsPainterEdModeToolkit::OnPainterCheckBoxStateChanged(const ECheckBoxState NewState)
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetPaintState(NewState == ECheckBoxState::Checked);
	}
}


void FVolumetricCloudsPainterEdModeToolkit::OnPaintModeCheckBoxStateChanged(const ECheckBoxState NewState)
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetPaintMode(NewState == ECheckBoxState::Checked);
	}
}


void FVolumetricCloudsPainterEdModeToolkit::OnEraseModeCheckBoxStateChanged(const ECheckBoxState NewState)
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetPaintMode(NewState != ECheckBoxState::Checked);
	}
}


void FVolumetricCloudsPainterEdModeToolkit::OnChannelCheckBoxStateChanged(const ECheckBoxState NewState, FName ChannelName)
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		((FVolumetricCloudsPainterEdMode*)GetEditorMode())->SetChannelState(NewState == ECheckBoxState::Checked, ChannelName);
	}
}


FText FVolumetricCloudsPainterEdModeToolkit::PainterCheckBoxText() const
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		if (((FVolumetricCloudsPainterEdMode*)GetEditorMode())->IsPainiting())
		{
			return FText::FromString("STOP PAINT");
		}
		else
		{
			return FText::FromString("START PAINT");
		}
	}

	return FText::FromString("START PAINT");
}


FText FVolumetricCloudsPainterEdModeToolkit::ChannelCheckBoxText(const FName ChannelName) const
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		if (((FVolumetricCloudsPainterEdMode*)GetEditorMode())->IsChannelEnabled(ChannelName))
		{
			return FText::FromString("ENABLED");
		}
		else
		{
			return FText::FromString("DISABLED");
		}
	}

	return FText::FromString("DISABLED");
}


ECheckBoxState FVolumetricCloudsPainterEdModeToolkit::IsPainterCheckBoxChecked() const
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		if (((FVolumetricCloudsPainterEdMode*)GetEditorMode())->IsPainiting())
		{
			return ECheckBoxState::Checked;
		}
		else
		{
			return ECheckBoxState::Unchecked;
		}
	}

	return ECheckBoxState::Unchecked;
}


ECheckBoxState FVolumetricCloudsPainterEdModeToolkit::IsPaintModeCheckBoxChecked() const
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		if (((FVolumetricCloudsPainterEdMode*)GetEditorMode())->IsAdditivePaint())
		{
			return ECheckBoxState::Checked;
		}
		else
		{
			return ECheckBoxState::Unchecked;
		}
	}

	return ECheckBoxState::Checked;
}


ECheckBoxState FVolumetricCloudsPainterEdModeToolkit::IsEraseModeCheckBoxChecked() const
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		if (((FVolumetricCloudsPainterEdMode*)GetEditorMode())->IsAdditivePaint())
		{
			return ECheckBoxState::Unchecked;
		}
		else
		{
			return ECheckBoxState::Checked;
		}
	}

	return ECheckBoxState::Unchecked;
}


ECheckBoxState FVolumetricCloudsPainterEdModeToolkit::IsChannelBoxChecked(const FName ChannelName) const
{
	if ((FVolumetricCloudsPainterEdMode*)GetEditorMode() != nullptr)
	{
		if (((FVolumetricCloudsPainterEdMode*)GetEditorMode())->IsChannelEnabled(ChannelName))
		{
			return ECheckBoxState::Checked;
		}
		else
		{
			return ECheckBoxState::Unchecked;
		}
	}

	return ECheckBoxState::Unchecked;
}



#undef LOCTEXT_NAMESPACE
