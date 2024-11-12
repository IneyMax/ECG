// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActor.h"

#include "CellMeshComponent.h"
#include "GameEntitySubsystem.h"
#include "Logging/StructuredLog.h"


DEFINE_LOG_CATEGORY(LogGridActor);


FEntityWrapper Grid::ChooseRandomEmptyCell(entt::registry& InRegistry)
{
	TArray<FEntityWrapper> EmptyCells;
	auto View = InRegistry.view<FGridCell>();
	for(const auto& [Entity, GridCell]: View.each())
	{
		if (GridCell.EntityContains.IsEmpty())
		{
			EmptyCells.Add(Entity);
		}
	}
	return EmptyCells[FMath::RandRange(0, EmptyCells.Num() - 1)];
}

FEntityWrapper Grid::ChooseRandomEvenEmptyCell(entt::registry& InRegistry)
{
	TArray<FEntityWrapper> EvenEmptyCells;
	auto View = InRegistry.view<FGridCell>();

	// use a callback
	View.each([](const auto &GridCell) { /* ... */ });

	// use an extended callback
	View.each([](const auto Entity, const auto &GridCell) { /* ... */ });
	
	// use a range-for
	for(const auto& [Entity, GridCell]: View.each())
	{
		if (GridCell.Index_X &1 && GridCell.Index_Y &1)
		{
			EvenEmptyCells.Add(Entity);
		}
	}
	return EvenEmptyCells[FMath::RandRange(0, EvenEmptyCells.Num() - 1)];
}

AGridActor::AGridActor()
{
	
}

void AGridActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGridActor::CreateNewCell(const int32 CellIndexX, const int32 CellIndexY, const FVector& InLocation,
	const FRotator& InRotation, const FVector& InScale)
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	FEntityWrapper NewCellEntity = Registry.create();
	UCellMeshComponent* CellComponent = NewObject<UCellMeshComponent>(this, CellClass);
	CellComponent->SetupAttachment(RootComponent);
	CellComponent->RegisterComponent();
	CellComponent->bEditableWhenInherited = true;
	CellComponent->SetRelativeLocation(InLocation);
	CellComponent->SetRelativeRotation(InRotation);
	CellComponent->SetRelativeScale3D(InScale);
	CellComponent->Entity = NewCellEntity;
	Cells.Add(CellComponent);
	Registry.emplace<FGridCell>(NewCellEntity, CellIndexX, CellIndexY, CellComponent);
}

void AGridActor::GetCellData(int32 InEntity, FGridCell& OutData)
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	if (FGridCell* CellData = Registry.try_get<FGridCell>(static_cast<entt::entity>(InEntity)))
	{
		UE_LOGFMT(LogGridActor, Log, "GetCellData:\n "
							"X:{X},\n Y:{Y}"",\n IsEmpty:{IsEmpty}",
			CellData->Index_X, CellData->Index_Y, CellData->EntityContains.IsEmpty());
		OutData = *CellData;
	}
}

int32 AGridActor::ChooseRandomEvenEmptyCell()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	return Grid::ChooseRandomEvenEmptyCell(Registry);
}
