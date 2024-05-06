// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SmartImageButton.h"
#include "NativeGameplayTags.h"
#include "SmartSelector.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStringValueChange, FString, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTagValueChange, FGameplayTag, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntValueChange, int32, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressed);

UENUM(BlueprintType)
enum class ESelectorArrow : uint8 {
	Left,
	Right
};

UENUM(BlueprintType)
enum class ESelectorValueType : uint8 {
	String,
	Tag,
	Int
};

UCLASS()
class SMARTUI_API USmartSelector : public UUserWidget
{
	GENERATED_BODY()

public:
	
	USmartSelector(const FObjectInitializer& Initializer);

	void NativeConstruct() override;
	void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidget))
	USmartImageButton* NextButton;

	UPROPERTY(meta = (BindWidget))
	USmartImageButton* PrevButton;

	UPROPERTY(EditAnywhere)
	ESelectorValueType IterationType;

	UPROPERTY(EditAnywhere, meta=(EditConditionHides, EditCondition = "IterationType == ESelectorValueType::Tag"))
	TArray<FGameplayTag> TagList;

	UPROPERTY(EditAnywhere, meta=(EditConditionHides, EditCondition = "IterationType == ESelectorValueType::String"))
	TArray<FString> StringList;

	UPROPERTY(EditAnywhere, meta = (EditConditionHides, EditCondition = "IterationType == ESelectorValueType::Int"))
	int32 StartValue;

	UPROPERTY(EditAnywhere, meta = (EditConditionHides, EditCondition = "IterationType == ESelectorValueType::Int"))
	int32 StepValue;

	UPROPERTY(EditAnywhere, meta = (EditConditionHides, EditCondition = "IterationType == ESelectorValueType::Int"))
	int32 EndValue;

	UPROPERTY()
	int32 CurrentIndex;
	
	UPROPERTY(BlueprintAssignable)
	FOnStringValueChange OnStringValueChange;

	UPROPERTY(BlueprintAssignable)
	FOnButtonPressed OnPrevButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FOnButtonPressed OnNextButtonPressed;
	
	UPROPERTY(BlueprintAssignable)
	FOnIntValueChange OnIntValueChange;
	
	UPROPERTY(BlueprintAssignable)
	FOnTagValueChange OnTagValueChange;

	UPROPERTY()
	TArray<int32> IntList;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentIndex() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetCurrentStringValue() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetCurrentTagValue() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentIntValue() const;

	UFUNCTION(BlueprintCallable)
	void SetCurrentStringValue(FString InValue);
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentIntValue(int32 InValue);
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentTagValue(FGameplayTag InValue);

	UFUNCTION(BlueprintCallable)
	void InitializeStringSelector(TArray<FString> InArray);

	UFUNCTION(BlueprintCallable)
	void InitializeTagSelector(TArray<FGameplayTag> InArray);
	
	UFUNCTION(BlueprintCallable)
	void InitializeIntSelector(int32 Start, int32 End, int32 Step = 1);

	UFUNCTION()
	void OnNextPressed();

	UFUNCTION()
	void OnPrevPressed();

	UFUNCTION(BlueprintCallable)
	void SetCurrentIndex(int32 Index);

	UFUNCTION(BlueprintCallable)
	void SetArrowEnabled(ESelectorArrow Arrow, bool bEnabled);

protected:

	UFUNCTION(BlueprintCallable)
	void InitializeIntList();

};
