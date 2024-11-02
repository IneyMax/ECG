// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"

#include "entity/registry.hpp"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase2.generated.h"

class UCardData;
struct FUntypedEmplacer;
struct FUntypedEmplaceRegistry;

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerStateBase2, Log, All);

#pragma region TRAITS
		template<typename...>
		struct TArgsPack {};
#pragma endregion

template<typename... Types>
struct TKnownTypeList
{
private:
	template<typename T, typename F>
	static void TryDispatch(FInstancedStruct UntypedStruct, F Functor)
	{
		if (T::StaticStruct() == UntypedStruct.GetScriptStruct())
		{
			Functor(UntypedStruct.Get<T>());
		}
	}
public:
	template<typename F>
	static void DispatchFor(FInstancedStruct UntypedStruct, F Functor)
	{
		(TryDispatch<Types>(UntypedStruct, Functor), ...);
	}
};


UCLASS()
class GAME_API APlayerStateBase2 : public APlayerState
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void CreateNewCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void AddCreatureToRandomCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void PlayCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void CalculateResultDamage();

	UPROPERTY(EditAnywhere)
	UCardData* CardData;
};
