// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmartButton.h"
#include "Components/Image.h"
#include "SmartImageButton.generated.h"

/**
 * 
 */
UCLASS()
class SMARTUI_API USmartImageButton : public USmartButton
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FSlateBrush ActiveImage;

	UPROPERTY(EditAnywhere)
	FSlateBrush InactiveImage;

	virtual void SetActiveStatus(bool bStatus) override;
	void NativePreConstruct() override;

protected:

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* ButtonImage;

};
