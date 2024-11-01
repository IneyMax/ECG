// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Card.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct CARDS_API FCard
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Index;
};


USTRUCT(Blueprintable, BlueprintType)
struct CARDS_API FCreature
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = 1;
};