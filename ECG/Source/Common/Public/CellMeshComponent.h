// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "EntityWrapper.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCellMeshComponent* MeshComponent;
};


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMMON_API UCellMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

	UCellMeshComponent();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CellMesh", meta = (AllowPrivateAccess = "true"))
	int32 Entity = INDEX_NONE;
};
