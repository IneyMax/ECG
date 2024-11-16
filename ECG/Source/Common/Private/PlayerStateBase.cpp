// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "CellMeshComponent.h"
#include "EntityWrapper.h"
#include "GameEntityData.h"
#include "GameEntitySubsystem.h"
#include "GameEntityTypes.h"
#include "GridActor.h"
#include "RegistryWrapper.h"
#include "Logging/StructuredLog.h"
#include "entity/registry.hpp"


DEFINE_LOG_CATEGORY(LogPlayerStateBase);


TMap<UScriptStruct*, FUntypedWrapper*> FUntypedRegistryWrapper::UntypedWrappers {};


bool Cards::SelectRandomCard(entt::registry& InRegistry, FEntityWrapper& OutEntity)
{
	TArray<FEntityWrapper> AllCards;
	InRegistry.view<FCard>().each([&AllCards](auto Entity, FCard& Card)  
	{  
		AllCards.Add(Entity);
	});
	
	if (!AllCards.IsEmpty())
	{
		OutEntity = AllCards[FMath::RandRange(0, AllCards.Num() - 1)];
		return true;
	}
	return false;
}

void Cards::AddEntityToPlace(entt::registry& InRegistry, const entt::entity InEntity, const entt::entity InPlace)
{
	if (FPlaceability* Placeability = InRegistry.try_get<FPlaceability>(InEntity))
	{
		const TFunction<void(FEntityPlace& InEntityPlace)> PlacePatch = [&](FEntityPlace& InEntityPlace)
		{
			// UE_LOGFMT(LogPlayerStateBase, Log, "Patch {InPlayedEntity}, Index_X:{Index_X}, Index_Y:{Index_Y}",
			// 		  static_cast<uint32_t>(InEntity), Cell.Index_X, Cell.Index_Y);
		
			InEntityPlace.EntityContains.AddUnique(InEntity);
		};
		InRegistry.patch<FEntityPlace>(InPlace, PlacePatch);
	
		const TFunction<void(FPlaceability& InPlaceability)> PlaceabilityPatch = [&](FPlaceability& InPlaceability)
		{
			// UE_LOGFMT(LogPlayerStateBase, Log, "Patch {InPlayedEntity}, Index_X:{Index_X}, Index_Y:{Index_Y}",
			// 		  static_cast<uint32_t>(InEntity), Cell.Index_X, Cell.Index_Y);
		
			InPlaceability.OccupiedPlaces.AddUnique(InPlace);
		};
		InRegistry.patch<FPlaceability>(InEntity, PlaceabilityPatch);
	
		InRegistry.emplace<FPlayed>(InEntity);
	}
	else
	{
		UE_LOGFMT(LogPlayerStateBase, Error, "Fail AddEntityToPlace: Entity: {InEntity} not has FPlaceability",
				  static_cast<uint32_t>(InEntity));
	}
}

void Creatures::CalculateResultDamage(const entt::registry& InRegistry)
{
	// use a callback
	auto View = InRegistry.view<FCreature, FGridCell>();
	int32 ResultDamage = 0;
	View.each([&ResultDamage](const FCreature& Creature, const FGridCell& Grid)
	{
		ResultDamage += Creature.Attack;
	});
	UE_LOGFMT(LogPlayerStateBase, Log, "ResultDamage: {ResultDamage}", ResultDamage);
}


APlayerStateBase::APlayerStateBase()
{
	FUntypedRegistryWrapper::RegisterUntypedWrapperType<FCard>();
	FUntypedRegistryWrapper::RegisterUntypedWrapperType<FGameEntity>();
	FUntypedRegistryWrapper::RegisterUntypedWrapperType<FCreature>();
	FUntypedRegistryWrapper::RegisterUntypedWrapperType<FPlaceability>();
	FUntypedRegistryWrapper::RegisterUntypedWrapperType<FEntityPlace>();
}

void APlayerStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerStateBase::CreateNewCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	FEntityWrapper NewCellEntity = Registry.create();
	int32 RandDataIndex = FMath::RandRange(0, GameEntityData.Num() - 1);
	auto RandData = GameEntityData[RandDataIndex];
	for (auto Data : RandData->DefaultEntityData)
	{
		RegistryWrapper::Emplace(Registry, NewCellEntity, Data);
		UE_LOGFMT(LogPlayerStateBase, Log, "StructName:{Struct}", Data.GetScriptStruct()->GetName());
	}
}

void APlayerStateBase::PlayCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	FEntityWrapper SelectedRandomCard;
	Cards::SelectRandomCard(Registry, SelectedRandomCard);
	FEntityWrapper SelectedRandomEmptyCell;
	Grid::SelectRandomEmptyCell(Registry, SelectedRandomEmptyCell);

	if (SelectedRandomCard && SelectedRandomEmptyCell)
	{
		Cards::AddEntityToPlace(Registry, SelectedRandomCard, SelectedRandomEmptyCell);
	}
}

void APlayerStateBase::CalculateResultDamage()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	Creatures::CalculateResultDamage(Registry);
}


