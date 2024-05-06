// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartImageButton.h"

void USmartImageButton::SetActiveStatus(bool bStatus)
{
	ButtonImage->SetBrush(bStatus ? ActiveImage : InactiveImage);

	Super::SetActiveStatus(bStatus);
}

void USmartImageButton::NativePreConstruct()
{
	if (HitboxScale >= 1)
	{
		ButtonImage->SetRenderScale(FVector2D(1.f / HitboxScale, 1.f / HitboxScale));
	}
	Super::NativePreConstruct();
}
