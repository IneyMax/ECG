// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "CardData.h"
#include "Card.h"
#include "CellMeshComponent.h"
#include "EntityWrapper.h"
#include "GameEntitySubsystem.h"
#include "GridActor.h"
#include "Logging/StructuredLog.h"


DEFINE_LOG_CATEGORY(LogPlayerStateBase);


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
	InRegistry.patch<FGridCell>(InCell, [&](FGridCell& Cell)
	{
		UE_LOGFMT(LogPlayerStateBase, Log, "Patch {InPlayedEntity}, Index_X{Index_X}, Index_Y{Index_Y}",
		          static_cast<uint32_t>(InCard), Cell.Index_X, Cell.Index_Y);
		
		Cell.EntityContains.AddUnique(FEntityWrapper(InCard));
		InRegistry.emplace<FOnGrid>(InCard, InCell);
	});
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

void APlayerStateBase::CreateNewCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	FEntityWrapper NewCellEntity = Registry.create();
	Registry.emplace<FCard>(NewCellEntity, CardData->Data);
}

void APlayerStateBase::AddCreatureToRandomCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	entt::entity RandomCard = Cards::SelectRandomCardEntity(Registry);
	if (!Registry.any_of<FCreature>(RandomCard))
	{
		Registry.emplace<FCreature>(RandomCard);
	}
}

void APlayerStateBase::PlayCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	entt::entity RandomCard = Cards::SelectRandomCardEntity(Registry);
	entt::entity RandomCell = Grid::ChooseRandomEvenEmptyCell(Registry);
	Cards::PlayCardToCell(UGameEntitySubsystem::GetRegistry(this), RandomCard, RandomCell);
}

void APlayerStateBase::CalculateResultDamage()
{
	TestRegistry = &UGameEntitySubsystem::GetRegistry(this);
	entt::entity RandomCard = Cards::SelectRandomCardEntity(*TestRegistry);
	entt::entity RandomCell = Grid::ChooseRandomEvenEmptyCell(*TestRegistry);
	
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	Creatures::CalculateResultDamage(Registry);
}


