// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UISoundSettings.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "SmartCheckBox.generated.h"

UCLASS()
class SMARTUI_API USmartCheckBox : public UUserWidget
{
	GENERATED_BODY()
	
public:

	USmartCheckBox(const FObjectInitializer& ObjectInitializer);

	void NativeConstruct() override;
	void NativePreConstruct() override;

	void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta=(BindWidget))
	UCheckBox* BaseCheckBox;

	UPROPERTY(BlueprintAssignable)
	FOnButtonHoverEvent OnHovered;
	
	UPROPERTY(BlueprintAssignable)
	FOnButtonHoverEvent OnUnhovered;

	UPROPERTY(BlueprintAssignable)
	FOnCheckBoxComponentStateChanged OnStateChanged;
	
	UFUNCTION(BlueprintCallable)
	void SetChecked(ECheckBoxState CheckedState);

	UFUNCTION()
	void OnCheckBoxStateChanged(bool bChecked);

	UPROPERTY(EditAnywhere)
	FCheckBoxStyle Style;

	UPROPERTY(EditAnywhere)
	EUISoundType SoundType;

protected:
	UPROPERTY(BlueprintReadWrite)
	UWidgetAnimation* HoverAnimation;
};
