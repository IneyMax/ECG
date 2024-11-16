// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EntityWrapper.h"
#include "GameFramework/PlayerState.h"
#include "entity/registry.hpp"
#include "PlayerStateBase.generated.h"

class UGameEntityData;

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerStateBase, Log, All);


namespace Cards
{
	bool SelectRandomCard(entt::registry& InRegistry, FEntityWrapper& OutEntity);
	void AddEntityToPlace(entt::registry& InRegistry, entt::entity InEntity, entt::entity InPlace);
}


namespace Creatures
{
	void CalculateResultDamage(const entt::registry& InRegistry);
}


UCLASS()
class COMMON_API APlayerStateBase : public APlayerState
{
	GENERATED_BODY()

	APlayerStateBase();

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
