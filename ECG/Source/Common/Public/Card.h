// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Card.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct COMMON_API FCard
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Index = INDEX_NONE;
};


USTRUCT(Blueprintable, BlueprintType)
struct COMMON_API FCreature
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = 0;
};