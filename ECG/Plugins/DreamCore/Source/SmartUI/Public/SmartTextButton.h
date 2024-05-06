// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmartButton.h"
#include "SmartTextButton.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class SMARTUI_API USmartTextButton : public USmartButton
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere)
	FText ActiveText;

	UPROPERTY(EditAnywhere)
	FSlateColor ActiveTextColor;

	UPROPERTY(EditAnywhere)
	FSlateFontInfo ActiveTextFont;

	UPROPERTY(EditAnywhere)
	FText InactiveText;

	UPROPERTY(EditAnywhere)
	FSlateColor InactiveTextColor;

	UPROPERTY(EditAnywhere)
	FSlateFontInfo InactiveTextFont;

	virtual void SetActiveStatus(bool bStatus) override;

	void SetActiveStatusAndText(bool bStatus, FText InText);

private:
	//Inner widgets
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ButtonText;
};
