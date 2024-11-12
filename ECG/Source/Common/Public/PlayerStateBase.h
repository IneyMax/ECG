// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "entity/registry.hpp"
#include "PlayerStateBase.generated.h"

class UGameEntityData;

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerStateBase, Log, All);

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
class COMMON_API APlayerStateBase : public APlayerState
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreateNewCard, FEntityWrapper&, NewCellEntity, TArray<FInstancedStruct>&, EntityData);
	FOnCreateNewCard OnCreateNewCard;
	
public:
	virtual void BeginPlay() override;

protected:
	UFUNCTION(CallInEditor, BlueprintCallable)
	void CreateNewCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void PlayCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void CalculateResultDamage();

	UPROPERTY(EditAnywhere)
    TArray<UGameEntityData*> GameEntityData;
};
