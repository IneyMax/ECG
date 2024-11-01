// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CellMeshComponent.h"
#include "entity/registry.hpp"
#include "GridActor.generated.h"


class UCellMeshComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogGridActor, Log, All);

namespace Grid
{
	FEntityWrapper GRID_API ChooseRandomEmptyCell(entt::registry& InRegistry);
	FEntityWrapper GRID_API ChooseRandomEvenEmptyCell(entt::registry& InRegistry);
}


UCLASS(Blueprintable, BlueprintType)
class GRID_API AGridActor : public AActor
{
	GENERATED_BODY()

	AGridActor();
	
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable)
	void CreateNewCell(int32 CellIndexX, int32 CellIndexY, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale);
	
	UFUNCTION(BlueprintCallable)
	void GetCellData(int32 InEntity, FGridCell& OutData);

	UFUNCTION(CallInEditor, BlueprintCallable)
	int32 ChooseRandomEvenEmptyCell();

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UCellMeshComponent> CellClass;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cell, meta = (AllowPrivateAccess = "true"))
	TArray<UCellMeshComponent*> Cells;
};
