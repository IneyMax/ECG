// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityWrapper.h"
#include "GameEntityTypes.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct COMMON_API FGameEntity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Index = INDEX_NONE;
};


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


USTRUCT(Blueprintable, BlueprintType)
struct FPlaceability
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FEntityWrapper> OccupiedPlaces;
};


USTRUCT(Blueprintable, BlueprintType)
struct FEntityPlace
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CellMesh", meta = (AllowPrivateAccess = "true"))
	TArray<FEntityWrapper> EntityContains;
};

struct FPlayed {};
