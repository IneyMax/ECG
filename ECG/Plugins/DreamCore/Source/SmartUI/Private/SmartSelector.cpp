// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartSelector.h"

USmartSelector::USmartSelector(const FObjectInitializer& Initializer) : Super(Initializer)
{
    IterationType = ESelectorValueType::Tag;
    StartValue = 0;
    EndValue = 10;
    StepValue = 1;
    CurrentIndex = 0;
}

void USmartSelector::NativeConstruct()
{
    Super::NativeConstruct();
    InitializeIntList();

    NextButton->OnClickedActive.AddDynamic(this, &USmartSelector::OnNextPressed);
    PrevButton->OnClickedActive.AddDynamic(this, &USmartSelector::OnPrevPressed);
}

void USmartSelector::NativePreConstruct()
{
    Super::NativePreConstruct();
}

int32 USmartSelector::GetCurrentIndex() const
{
    return CurrentIndex;
}

FString USmartSelector::GetCurrentStringValue() const
{
    if (StringList.IsValidIndex(CurrentIndex))
    {
        return StringList[CurrentIndex];
    }

    return "";
}

FGameplayTag USmartSelector::GetCurrentTagValue() const
{
    if (TagList.IsValidIndex(CurrentIndex))
    {
        return TagList[CurrentIndex];
    }

    return FGameplayTag::EmptyTag;
}

int32 USmartSelector::GetCurrentIntValue() const
{
    if (IntList.IsValidIndex(CurrentIndex))
    {
        return IntList[CurrentIndex];
    }
    
    return StartValue;
}

void USmartSelector::SetCurrentStringValue(FString InValue)
{
    ensureMsgf(IterationType == ESelectorValueType::String, TEXT("Invalid input type, selector has another type"));

    if (StringList.Contains(InValue))
    {
        CurrentIndex = StringList.IndexOfByKey(InValue);
        SetCurrentIndex(CurrentIndex);
    }
}

void USmartSelector::SetCurrentIntValue(int32 InValue)
{
    ensureMsgf(IterationType == ESelectorValueType::Int, TEXT("Invalid input type, selector has another type"));

    if (IntList.Contains(InValue))
    {
        CurrentIndex = IntList.IndexOfByKey(InValue);
        SetCurrentIndex(CurrentIndex);
    }
}

void USmartSelector::SetCurrentTagValue(FGameplayTag InValue)
{
    ensureMsgf(IterationType == ESelectorValueType::Tag, TEXT("Invalid input type, selector has another type"));

    if (TagList.Contains(InValue))
    {
        CurrentIndex = TagList.IndexOfByKey(InValue);
        SetCurrentIndex(CurrentIndex);
    }
}

void USmartSelector::InitializeStringSelector(TArray<FString> InArray)
{
    IterationType = ESelectorValueType::String;
    StringList = InArray;
    SetCurrentIndex(CurrentIndex);
}

void USmartSelector::InitializeTagSelector(TArray<FGameplayTag> InArray)
{
    IterationType = ESelectorValueType::Tag;
    TagList = InArray;
}

void USmartSelector::InitializeIntSelector(int32 Start, int32 End, int32 Step)
{
    IterationType = ESelectorValueType::Int;
    StartValue = Start;
    EndValue = End;
    StepValue = Step;
    InitializeIntList();
}

void USmartSelector::InitializeIntList()
{
    IntList.Empty();

    if (IterationType == ESelectorValueType::Int)
    {
        for (int i = StartValue; i <= EndValue; i += StepValue)
        {
            IntList.Add(i);
        }
    }

}

void USmartSelector::OnNextPressed()
{
    OnNextButtonPressed.Broadcast();
    SetCurrentIndex(CurrentIndex + 1);
}

void USmartSelector::OnPrevPressed()
{
    OnPrevButtonPressed.Broadcast();
    SetCurrentIndex(CurrentIndex - 1);
}

void USmartSelector::SetCurrentIndex(int32 Index)
{
    switch (IterationType)
    {
    case ESelectorValueType::String:
        if (StringList.IsValidIndex(Index))
        {
            CurrentIndex = Index;
        }
        else if (Index < 0)
        {
            CurrentIndex = StringList.Num() - 1;
        }
        else
        {
            CurrentIndex = 0;
        }

        OnStringValueChange.Broadcast(StringList[CurrentIndex]);
        break;
    case ESelectorValueType::Tag:
        if (TagList.IsValidIndex(Index))
        {
            CurrentIndex = Index;
            OnTagValueChange.Broadcast(TagList[CurrentIndex]);
        }
        else if (Index < 0)
        {
            CurrentIndex = TagList.Num() - 1;
        }
        else
        {
            CurrentIndex = 0;
        }
        break;
    case ESelectorValueType::Int:
        if (IntList.IsValidIndex(Index))
        {
            CurrentIndex = Index;
            OnIntValueChange.Broadcast(IntList[CurrentIndex]);
        }
        else if (Index < 0)
        {
            CurrentIndex = IntList.Num() - 1;
        }
        else
        {
            CurrentIndex = 0;
        }
        break;
    default:
        break;
    }
}

void USmartSelector::SetArrowEnabled(ESelectorArrow Arrow, bool bEnabled)
{
    switch (Arrow)
    {
    case ESelectorArrow::Left:
        PrevButton->SetActiveStatus(bEnabled);
        break;
    case ESelectorArrow::Right:
        NextButton->SetActiveStatus(bEnabled);
        break;
    default:
        break;
    }
}
