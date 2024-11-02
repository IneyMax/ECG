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

// #pragma region TRAITS
//
// 		template <typename T>
// 		struct TMemberTraits : public TMemberTraits<decltype(&T::operator())>
// 		{};
//
// 		template<typename...>
// 		struct TArgsPack {};
//
// 		
// 		template <typename R, typename... Args>
// 		struct TMemberTraits<R(*)(Args...)>
// 		{
// 			using ReturnType = R;
// 			using ArgsTuple = TTuple<Args...>;
//
// 			using ArgsPack = TArgsPack<Args...>;
// 			
// 			template <std::size_t Index>
// 			using Argument = std::tuple_element_t<
// 				Index,
// 				std::tuple<Args...>
// 			>;
// 			
// 			static constexpr auto Arity = sizeof...(Args);
// 		};
// 		
// 		template <typename ClassType, typename R, typename... Args>
// 		struct TMemberTraits<R(ClassType::*)(Args...)>
// 		{
// 			using ReturnType = R;
// 			using ArgsTuple = TTuple<Args...>;
// 			using ArgsPack = TArgsPack<Args...>;
// 			
// 			template <std::size_t Index>
// 			using Argument = std::tuple_element_t<
// 				Index,
// 				std::tuple<Args...>
// 			>;
//
// 			static constexpr auto Arity = sizeof...(Args);
// 		};
// 		
// 		template <typename ClassType, typename R, typename... Args>
// 		struct TMemberTraits<R(ClassType::*)(Args...) const>
// 		{
// 			using ReturnType = R;
// 			using ArgsTuple = TTuple<Args...>;
// 			using ArgsPack = TArgsPack<Args...>;
// 			
// 			template <std::size_t Index>
// 			using Argument = std::tuple_element_t<
// 				Index,
// 				std::tuple<Args...>
// 			>;
// 			static constexpr auto Arity = sizeof...(Args);
// 		};	
//
// 		template <typename ClassType, typename R, typename... Args>
// 		struct TMemberTraits<R(ClassType::*)(Args...) &>
// 		{
// 			using ReturnType = R;
// 			using ArgsTuple = TTuple<Args...>;
// 			using ArgsPack = TArgsPack<Args...>;
// 			
// 			template <std::size_t Index>
// 			using Argument = std::tuple_element_t<
// 				Index,
// 				std::tuple<Args...>
// 			>;
//
// 			static constexpr auto Arity = sizeof...(Args);
// 		};
//
// 		template <typename ClassType, typename R, typename... Args>
// 		struct TMemberTraits<R(ClassType::*)(Args...) &&>
// 		{
// 			using ReturnType = R;
// 			using ArgsTuple = TTuple<Args...>;
// 			using ArgsPack = TArgsPack<Args...>;
// 			
// 			template <std::size_t Index>
// 			using Argument = std::tuple_element_t<
// 				Index,
// 				std::tuple<Args...>
// 			>;
//
// 			static constexpr auto Arity = sizeof...(Args);
// 		};
//
// 		template <typename ClassType, typename R, typename... Args>
// 		struct TMemberTraits<R(ClassType::*)(Args...) const &>
// 		{
// 			using ReturnType = R;
// 			using ArgsTuple = TTuple<Args...>;
// 			using ArgsPack = TArgsPack<Args...>;
// 			
// 			template <std::size_t Index>
// 			using Argument = std::tuple_element_t<
// 				Index,
// 				std::tuple<Args...>
// 			>;
//
// 			static constexpr auto Arity = sizeof...(Args);
// 		};
//
// 		template <typename ClassType, typename R, typename... Args>
// 		struct TMemberTraits<R(ClassType::*)(Args...) const &&>
// 		{
// 			using ReturnType = R;
// 			using ArgsTuple = TTuple<Args...>;
// 			using ArgsPack = TArgsPack<Args...>;
// 			
// 			template <std::size_t Index>
// 			using Argument = std::tuple_element_t<
// 				Index,
// 				std::tuple<Args...>
// 			>;
//
// 			static constexpr auto Arity = sizeof...(Args);
// 		};
// #pragma endregion
//
// template<typename... Types>
// struct TKnownTypeList
// {
// private:
// 	template<typename T, typename F>
// 	static void TryDispatch(FInstancedStruct UntypedStruct, F Functor)
// 	{
// 		using Traits = TMemberTraits<std::decay_t<F>>;
// 		using ArgsTuple = typename Traits::ArgsTuple;
// 		using FirstArgumentType = std::tuple_element_t<0, ArgsTuple>;
// 		
// 		if (T::StaticStruct() == FirstArgumentType::StaticStruct()
// 			&& T::StaticStruct() == UntypedStruct.GetScriptStruct())
// 		{
// 			Functor(UntypedStruct.Get<T>());
// 		}
// 	}
// public:
// 	template<typename F>
// 	static void DispatchFor(FInstancedStruct UntypedStruct, F Functor)
// 	{
// 		(TryDispatch<Types>(UntypedStruct, Functor), ...);
// 	}
// };


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
