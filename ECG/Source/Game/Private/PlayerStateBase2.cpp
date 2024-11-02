// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase2.h"
#include "Card.h"
#include "CellMeshComponent.h"
#include "EntityWrapper.h"
#include "GameEntitySubsystem.h"
#include "GridActor.h"
#include "PlayerStateBase.h"
#include "Logging/StructuredLog.h"


DEFINE_LOG_CATEGORY(LogPlayerStateBase2);


// using KnownTypeList = TKnownTypeList<FCard>;
void APlayerStateBase2::BeginPlay()
{
	Super::BeginPlay();

	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	entt::entity RandomCard = Cards::SelectRandomCardEntity(Registry);
	FInstancedStruct Qwert = FInstancedStruct::Make<FCard>();
	// KnownTypeList::DispatchFor(Qwert, [&] (auto Value)
	// {
	// 	Registry.emplace<decltype(Value)>(RandomCard, Value);
	// });
}

void APlayerStateBase2::CreateNewCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	FEntityWrapper NewCellEntity = Registry.create();
	Registry.emplace<FCard>(NewCellEntity);
}

void APlayerStateBase2::AddCreatureToRandomCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	entt::entity RandomCard = Cards::SelectRandomCardEntity(Registry);
	if (!Registry.any_of<FCreature>(RandomCard))
	{
		Registry.emplace<FCreature>(RandomCard);
	}
}

void APlayerStateBase2::PlayCard()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	entt::entity RandomCard = Cards::SelectRandomCardEntity(Registry);
	entt::entity RandomCell = Grid::ChooseRandomEvenEmptyCell(Registry);
	Cards::PlayCardToCell(UGameEntitySubsystem::GetRegistry(this), RandomCard, RandomCell);
}

void APlayerStateBase2::CalculateResultDamage()
{
	entt::registry& Registry = UGameEntitySubsystem::GetRegistry(this);
	Creatures::CalculateResultDamage(Registry);
}


