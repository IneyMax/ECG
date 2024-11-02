// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"

#include "entity/registry.hpp"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase.generated.h"

class UCardData;
struct FUntypedEmplacer;
struct FUntypedEmplaceRegistry;

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerStateBase, Log, All);


// vvv Объявляем шаблонный тип "поместителя" данных (с помощью него будет происходить типизированное помещение в один из массивов)
// template<typename T>
// struct Emplacer;

// Пошла магия со стиранием типа
// Объявляем базовый класс "нетипизированного" поместителя данных
struct GAME_API FUntypedEmplacer
{
	// Ключевая особенность - виртуальный метод, принимающий нетипизированную структуру
	virtual void DoUntypedEmplace(entt::registry& Registry, entt::entity Entity,  const FInstancedStruct& Value) = 0;
	virtual ~FUntypedEmplacer() {};
};

// vvv Реализация нетипизированного поместителя под совершенно любой тип (указывается в шаблоне)
template<typename T>
struct GAME_API TUntypedEmplacer : FUntypedEmplacer
{
	// Виртуальный метод кастует нетипизированные данные и вызывает обычный типизированный emplace
	virtual void DoUntypedEmplace(entt::registry& Registry, entt::entity Entity,  const FInstancedStruct& Value) override
	{
		T TypedValue = Value.Get<T>();
		Registry.emplace<T>(Entity, TypedValue); // Здесь используем Emplace из EnTT
	}
};


// Реестр всех нетипизированных "поместителей"
struct GAME_API FUntypedEmplaceRegistry
{
	static TMap<UScriptStruct*, FUntypedEmplacer*> UntypedEmplacers;

	// Метод регистрации динамически создаёт нетипизированный эмплейсер по типу
	template<typename T>
	static void RegisterUntypedEmplacerType()
	{
		UntypedEmplacers.Add(T::StaticStruct(), new TUntypedEmplacer<T>);
	}
};


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

	virtual void BeginPlay() override;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void CreateNewCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void AddCreatureToRandomCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void PlayCard();
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void CalculateResultDamage();

	// В реализации нетипизированного эмплейса мы ищем эмплейсер и дёргаем тот самый виртуальный метод
	static void Emplace(entt::registry& Registry, const entt::entity Entity,  const FInstancedStruct& Value)
	{
		if (Value.IsValid())
		{
			if (auto EmplacerPtr = FUntypedEmplaceRegistry::UntypedEmplacers.Find(Value.GetScriptStruct()))
			{
				if (EmplacerPtr != nullptr)
				{
					// Вызывая этот виртуальный метод мы выбираем реализацию автоматически, подходящую по UScriptStruct
					(*EmplacerPtr)->DoUntypedEmplace(Registry, Entity, Value);
				}
			}
		}
	}

	UPROPERTY(EditAnywhere)
    UCardData* CardData;
};
