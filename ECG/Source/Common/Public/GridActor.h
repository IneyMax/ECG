// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CellMeshComponent.h"
#include "EntityWrapper.h"
#include "GameEntityTypes.h"
#include "entity/registry.hpp"
#include "GridActor.generated.h"


class UCellMeshComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogGridActor, Log, All);


namespace Grid
{
	bool SelectRandomEmptyCell(entt::registry& InRegistry, FEntityWrapper& OutEntity);
}


UCLASS(Blueprintable, BlueprintType)
class COMMON_API AGridActor : public AActor
{
	GENERATED_BODY()

	AGridActor();
	
	virtual void BeginPlay() override;
	
protected:
	UFUNCTION(BlueprintCallable)
	void CreateNewCell(int32 CellIndexX, int32 CellIndexY, const FVector& InLocation, const FRotator& InRotation,
		const FVector& InScale);
	
	UFUNCTION(BlueprintCallable)
	void GetCellData(int32 InEntity, FGridCell& OutData);
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void OnUpdate_EntityPlace(const FEntityWrapper& InEntity);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void K2_OnUpdateEntityPlace(const FEntityWrapper& InEntity, const FGridCell& GridCell);
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UCellMeshComponent> CellClass;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cell, meta = (AllowPrivateAccess = "true"))
	TArray<UCellMeshComponent*> Cells;
};
