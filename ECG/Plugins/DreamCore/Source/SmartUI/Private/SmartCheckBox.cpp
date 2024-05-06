// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartCheckBox.h"

USmartCheckBox::USmartCheckBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//SoundType = EUISoundType::Set;
}

void USmartCheckBox::NativeConstruct()
{
	BaseCheckBox->OnCheckStateChanged.AddDynamic(this, &USmartCheckBox::OnCheckBoxStateChanged);

	Super::NativeConstruct();
}

void USmartCheckBox::NativePreConstruct()
{
	Super::NativePreConstruct();
	BaseCheckBox->SetWidgetStyle(Style);
}

void USmartCheckBox::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (HoverAnimation)
	{
		UUISubsystem::PlayHoverAnimation(this, HoverAnimation);
	}

	OnHovered.Broadcast();
}

void USmartCheckBox::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (HoverAnimation)
	{
		UUISubsystem::PlayUnhoverAnimation(this, HoverAnimation);
	}

	OnUnhovered.Broadcast();
}

void USmartCheckBox::SetChecked(ECheckBoxState CheckedState)
{
	BaseCheckBox->SetCheckedState(CheckedState);
}

void USmartCheckBox::OnCheckBoxStateChanged(bool bChecked)
{
	UUISubsystem* UISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUISubsystem>();

	if (IsValid(UISubsystem))
	{
		UISubsystem->PlaySound2D(SoundType);
	}

	OnStateChanged.Broadcast(bChecked);
}
