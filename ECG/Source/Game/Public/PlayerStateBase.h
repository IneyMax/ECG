// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card.h"

#include "entity/registry.hpp"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogPlayerStateBase, Log, All);

class UCardData;

namespace Cards
{
	entt::entity SelectRandomCardEntity(entt::registry& InRegistry);
	void PlayCardToCell(entt::registry& InRegistry, entt::entity InCard, entt::entity InCell);
}

namespace Creatures
{
	void CalculateResultDamage(const entt::registry& InRegistry);
}

UCLASS()
class GAME_API APlayerStateBase : public APlayerState
{
	GENERATED_BODY()

	UFUNCTION(CallInEditor, BlueprintCallable)
	void CreateNewCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void AddCreatureToRandomCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void PlayCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void CalculateResultDamage();

	entt::registry* TestRegistry;

	UPROPERTY(EditAnywhere)
	UCardData* CardData;
};
