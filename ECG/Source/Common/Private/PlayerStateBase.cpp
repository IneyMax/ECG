// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "Card.h"
#include "CellMeshComponent.h"
#include "EntityWrapper.h"
#include "GameEntityData.h"
#include "GameEntitySubsystem.h"
#include "GridActor.h"
#include "RegistryWrapper.h"
#include "Logging/StructuredLog.h"
#include "entity/registry.hpp"


DEFINE_LOG_CATEGORY(LogPlayerStateBase);


TMap<UScriptStruct*, FUntypedWrapper*> FUntypedRegistryWrapper::UntypedWrappers {};


entt::entity Cards::SelectRandomCardEntity(entt::registry& InRegistry)
{
	TArray<entt::entity> AllCards;
	auto View = InRegistry.view<FCard>();
	
	// use a range-for
	for (const auto& [Entity, Card]: View.each())
	{
		AllCards.Add(Entity);
	}
	if (!AllCards.IsEmpty())
	{
		return AllCards[FMath::RandRange(0, AllCards.Num() - 1)];
	}
	return entt::null;
}

void Cards::PlayCardToCell(entt::registry& InRegistry, const entt::entity InCard, const entt::entity InCell)
{
	const TFunction<void(FGridCell& Cell)> CellPatch = [&](FGridCell& Cell)
	{
		UE_LOGFMT(LogPlayerStateBase, Log, "Patch {InPlayedEntity}, Index_X:{Index_X}, Index_Y:{Index_Y}",
				  static_cast<uint32_t>(InCard), Cell.Index_X, Cell.Index_Y);
		
		Cell.EntityContains.AddUnique(FEntityWrapper(InCard));
		InRegistry.emplace<FOnGrid>(InCard, InCell);
	};
	InRegistry.patch<FGridCell>(InCell, CellPatch);
}

void Creatures::CalculateResultDamage(const entt::registry& InRegistry)
{
	// use a callback
	auto View = InRegistry.view<FCreature, FOnGrid>();
	int32 ResultDamage = 0;
	View.each([&ResultDamage](const FCreature& Creature, const FOnGrid& Grid)
	{
		ResultDamage += Creature.Attack;
	});
	UE_LOGFMT(LogPlayerStateBase, Error, "ResultDamage: {ResultDamage}", ResultDamage);
}


void APlayerStateBase::BeginPlay()
{
	Super::BeginPlay();
	FUntypedRegistryWrapper::RegisterUntypedWrapperType<FCard>();
	FUntypedRegistryWrapper::RegisterUntypedWrapperType<FCreature>();
}

void APlayerStateBase::CreateNewCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	FEntityWrapper NewCellEntity = Registry.create();
	int32 RandDataIndex = FMath::RandRange(0, GameEntityData.Num() - 1);
	auto RandData = GameEntityData[RandDataIndex];
	for (auto Data : RandData->InstancedStructData)
	{
		RegistryWrapper::Emplace(Registry, NewCellEntity, Data);
		OnCreateNewCard.Broadcast(NewCellEntity, RandData->InstancedStructData);
		UE_LOGFMT(LogPlayerStateBase, Log, "StructName:{Struct}", Data.GetScriptStruct()->GetName());
	}
}

void APlayerStateBase::PlayCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	entt::entity RandomCard = Cards::SelectRandomCardEntity(Registry);
	entt::entity RandomCell = Grid::ChooseRandomEmptyCell(Registry);
	Cards::PlayCardToCell(Registry, RandomCard, RandomCell);
}

void APlayerStateBase::CalculateResultDamage()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	Creatures::CalculateResultDamage(Registry);
}


