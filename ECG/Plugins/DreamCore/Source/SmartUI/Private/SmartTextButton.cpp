// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartTextButton.h"
#include "Components/TextBlock.h"

void USmartTextButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(CurrentActivationTime > 0.f)
	{
		ButtonText->SetText(FText::AsNumber(FMath::CeilToInt(CurrentActivationTime)));
	}
}

void USmartTextButton::SetActiveStatus(bool bStatus)
{
	ButtonText->SetText(bStatus ? ActiveText : InactiveText);
	ButtonText->SetFont(bStatus ? ActiveTextFont : InactiveTextFont);
	ButtonText->SetColorAndOpacity(bStatus ? ActiveTextColor : InactiveTextColor);

	Super::SetActiveStatus(bStatus);

}

void USmartTextButton::SetActiveStatusAndText(bool bStatus, FText InText)
{
	(bStatus ? ActiveText : InactiveText) = InText;
	SetActiveStatus(bStatus);
}
