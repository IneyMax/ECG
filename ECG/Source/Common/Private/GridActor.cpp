// Fill out your copyright notice in the Description page of Project Settings.


#include "GridActor.h"

#include "CellMeshComponent.h"
#include "GameEntitySubsystem.h"
#include "GameEntityTypes.h"
#include "Logging/StructuredLog.h"


DEFINE_LOG_CATEGORY(LogGridActor);


bool Grid::SelectRandomEmptyCell(entt::registry& InRegistry, FEntityWrapper& OutEntity)
{
	TArray<FEntityWrapper> EmptyCells;
	InRegistry.view<FGridCell, FEntityPlace>().each([&EmptyCells](auto Entity, FGridCell& GridCell, FEntityPlace& EntityPlace)  
	{  
		if (EntityPlace.EntityContains.IsEmpty())
		{
			EmptyCells.Add(Entity);
		}
	});

	if (!EmptyCells.IsEmpty())
	{
		OutEntity = EmptyCells[FMath::RandRange(0, EmptyCells.Num() - 1)];
		return true;
	}
	return false;
}


AGridActor::AGridActor()
{
	
}

void AGridActor::BeginPlay()
{
	Super::BeginPlay();

	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	Registry.on_update<FEntityPlace>().connect<&AGridActor::OnUpdate_EntityPlace>(this);
}

void AGridActor::OnUpdate_EntityPlace(const FEntityWrapper& InEntity)
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	if (FGridCell* GridCell = Registry.try_get<FGridCell>(InEntity))
	{
		K2_OnUpdateEntityPlace(InEntity, *GridCell);
	}
}

void AGridActor::K2_OnUpdateEntityPlace_Implementation(const FEntityWrapper& InEntity, const FGridCell& GridCell)
{
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
	Registry.emplace<FEntityPlace>(NewCellEntity);
}

void AGridActor::GetCellData(int32 InEntity, FGridCell& OutData)
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	if (FGridCell* CellData = Registry.try_get<FGridCell>(static_cast<entt::entity>(InEntity)))
	{
		UE_LOGFMT(LogGridActor, Log, "GetCellData:\n X:{X},\n Y:{Y}",
			CellData->Index_X, CellData->Index_Y);
		OutData = *CellData;
	}
}
