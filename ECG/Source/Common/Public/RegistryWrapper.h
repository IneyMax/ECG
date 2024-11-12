// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "entity/registry.hpp"
#include "InstancedStruct.h"


// Объявляем базовый класс нетипизированной обёртки
struct COMMON_API FUntypedWrapper
{
	// Ключевая особенность - виртуальный метод, принимающий нетипизированную структуру
	virtual void DoEmplace(entt::registry& Registry, entt::entity Entity, const FInstancedStruct& Value) = 0;
	virtual ~FUntypedWrapper() {};
};

// Реализация нетипизированной обёртки под совершенно любой тип (указывается в шаблоне)
template<typename T>
struct COMMON_API TUntypedWrapper : FUntypedWrapper
{
	// Виртуальный метод кастует нетипизированные данные и вызывает обычный типизированный emplace
	virtual void DoEmplace(entt::registry& Registry, entt::entity Entity, const FInstancedStruct& Value) override
	{
		T TypedValue = Value.Get<T>();
		Registry.emplace<T>(Entity, TypedValue); // Здесь используем emplace из EnTT
	}
};

// Реестр всех нетипизированных обёрток
struct COMMON_API FUntypedRegistryWrapper
{
	static TMap<UScriptStruct*, FUntypedWrapper*> UntypedWrappers;

	// Метод регистрации динамически создаёт нетипизированную обёртку по типу
	template<typename T>
	static void RegisterUntypedWrapperType()
	{
		UntypedWrappers.Add(T::StaticStruct(), new TUntypedWrapper<T>);
	}
};

namespace RegistryWrapper
{
	// В реализации нетипизированной обёртки мы ищем реализацию и дёргаем тот самый виртуальный метод
	static void Emplace(entt::registry& Registry, const entt::entity Entity,  const FInstancedStruct& Value)
	{
		if (Value.IsValid())
		{
			if (auto UntypedWrapperPtr = FUntypedRegistryWrapper::UntypedWrappers.Find(Value.GetScriptStruct()))
			{
				if (UntypedWrapperPtr)
				{
					// Вызывая этот виртуальный метод мы выбираем реализацию автоматически, подходящую по UScriptStruct
					(*UntypedWrapperPtr)->DoEmplace(Registry, Entity, Value);
				}
			}
		}
	}
}

