// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UISoundSettings.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "SmartButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeActiveStatusEvent, bool, bValue);


UENUM(BlueprintType)
enum class EHoverAnimationType : uint8 {
	Resize,
	Rotate,
	Bordering,
	TopBottomBordering,
	Color,
	None
};

/**
 * 
 */
UCLASS()
class SMARTUI_API USmartButton : public UUserWidget
{
	GENERATED_BODY()

public: 
	
	USmartButton(const FObjectInitializer& Initializer);
	
	UFUNCTION(BlueprintPure)
	bool GetActiveStatus() const;

	UFUNCTION(BlueprintCallable)
	virtual void SetActiveStatus(bool bStatus);

	UFUNCTION(BlueprintPure)
	virtual bool IsEnabled() const;

	//Delegates
	UPROPERTY(BlueprintAssignable)
	FOnButtonHoverEvent OnHovered;

	UPROPERTY(BlueprintAssignable)
	FOnButtonClickedEvent OnClickedActive;

	UPROPERTY(BlueprintAssignable)
	FOnButtonClickedEvent OnClickedInactive;

	UPROPERTY(BlueprintAssignable)
	FOnChangeActiveStatusEvent OnChangeActiveStatus;

	UPROPERTY(BlueprintAssignable)
	FOnButtonHoverEvent OnUnhovered;

	UFUNCTION(BlueprintCallable)
	void ForceUnhover();

protected:

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	virtual void OnHoveredNative();

	UFUNCTION()
	virtual void OnUnhoveredNative();

	UFUNCTION()
	virtual void OnClickedNative();

	UFUNCTION(BlueprintCallable)
	void StartActivationTimer();

protected:
	//Settings
	UPROPERTY(EditAnywhere)
	EUISoundType SoundType;

	UPROPERTY(EditAnywhere)
	FButtonStyle ActiveStyle;

	UPROPERTY(EditAnywhere)
	FButtonStyle InactiveStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHoverAnimationType AnimationType;

	UPROPERTY(EditAnywhere)
	float CosmeticDelay;

	UPROPERTY(EditAnywhere)
	float HitboxScale;

	UPROPERTY(EditAnywhere)
	bool AlwaysEnabled;

	UPROPERTY(EditAnywhere)
	bool bActiveStatus;

	UPROPERTY(EditAnywhere)
	bool bPlayInactiveAnimation;

	UPROPERTY(EditAnywhere)
	bool bUseActivationTimer;
	
	UPROPERTY(EditAnywhere, meta = (editcondition = "bUseActivationTimer"))
	float ActivationTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EHoverAnimationType, UWidgetAnimation*> AnimationList;

protected:
	//Bind widgets
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* BaseButton;

	UPROPERTY()
	FTimerHandle CosmeticTimer;

	UPROPERTY()
	float CurrentActivationTime;
};

FORCEINLINE bool USmartButton::GetActiveStatus() const
{
	return bActiveStatus;
}

FORCEINLINE bool USmartButton::IsEnabled() const
{
	return false;
}

