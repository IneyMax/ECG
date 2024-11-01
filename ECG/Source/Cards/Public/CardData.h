// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card.h"
#include "InstancedStruct.h"
#include "Engine/DataAsset.h"
#include "CardData.generated.h"

/**
 * 
 */
UCLASS()
class CARDS_API UCardData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCard Data;

	UPROPERTY(EditAnywhere)
	TArray<FInstancedStruct> Test;
};
