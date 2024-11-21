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


bool GameEntities::SelectRandomCard(entt::registry& InRegistry, FEntityWrapper& OutEntity)
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

void GameEntities::AddEntityToPlace(entt::registry& InRegistry, const entt::entity InEntity, const entt::entity InPlace)
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

void GameEntities::CalculateResultDamage(const entt::registry& InRegistry)
{
	int32 ResultDamage = 0;
	InRegistry.view<FCreature, FPlayed>().each([&ResultDamage](const FCreature& Creature)
	{
		ResultDamage += Creature.Attack;
	});
	UE_LOGFMT(LogPlayerStateBase, Log, "ResultDamage: {ResultDamage}", ResultDamage);
}

void GameEntities::AddAllPlaceabilityEntitiesToGrid(const entt::registry& Registry)
{
	FEntityWrapper SelectedRandomCard;
	// SelectRandomCard(Registry, SelectedRandomCard);
	FEntityWrapper SelectedRandomEmptyCell;
	// Grid::SelectRandomEmptyCell(Registry, SelectedRandomEmptyCell);
	Ent
	Registry.view<FCreature, FPlaceability>(entt::exclude<FPlayed>).each([](const FCreature& Creature)
	{
		ResultDamage += Creature.Attack;
	});
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

void APlayerStateBase::CreateNewGameEntity()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	FEntityWrapper NewGameEntity = Registry.create();
	int32 RandDataIndex = FMath::RandRange(0, GameEntityData.Num() - 1);
	auto RandData = GameEntityData[RandDataIndex];
	for (auto Data : RandData->DefaultEntityData)
	{
		RegistryWrapper::Emplace(Registry, NewGameEntity, Data);
		UE_LOGFMT(LogPlayerStateBase, Log, "StructName:{Struct}", Data.GetScriptStruct()->GetName());
	}
}

void APlayerStateBase::PlayCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	FEntityWrapper SelectedRandomCard;
	GameEntities::SelectRandomCard(Registry, SelectedRandomCard);
	FEntityWrapper SelectedRandomEmptyCell;
	Grid::SelectRandomEmptyCell(Registry, SelectedRandomEmptyCell);
	if (SelectedRandomCard && SelectedRandomEmptyCell)
	{
		GameEntities::AddEntityToPlace(Registry, SelectedRandomCard, SelectedRandomEmptyCell);
	}
}

void APlayerStateBase::PlayAllCreatures()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	GameEntities::AddAllPlaceabilityEntitiesToGrid(Registry);
}

void APlayerStateBase::CalculateResultDamage()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	GameEntities::CalculateResultDamage(Registry);
}


