// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityWrapper.h"
#include "Components/StaticMeshComponent.h"
#include "CellMeshComponent.generated.h"


class UCellMeshComponent;


USTRUCT(Blueprintable, BlueprintType)
struct FGridCell
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Index_X;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Index_Y;

	UPROPERTY()
	UCellMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CellMesh", meta = (AllowPrivateAccess = "true"))
	TArray<FEntityWrapper> EntityContains;
};

USTRUCT(Blueprintable, BlueprintType)
struct FOnGrid
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FEntityWrapper OccupiedCell;
};


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRID_API UCellMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

	UCellMeshComponent();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CellMesh", meta = (AllowPrivateAccess = "true"))
	int32 Entity = INDEX_NONE;
};
