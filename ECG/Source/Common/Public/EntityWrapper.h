// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "entity/entity.hpp"
#include "Templates/TypeHash.h"
#include "EntityWrapper.generated.h"


// #if CPP
// using FEntityWrapper = entt::entity;
// #else
// USTRUCT(NoExport)
USTRUCT(Blueprintable, BlueprintType)
struct COMMON_API FEntityWrapper
{
	GENERATED_BODY()
	
	FEntityWrapper(): entity(entt::null), EntityIndex(INDEX_NONE) {}
	FEntityWrapper(const FEntityWrapper& Other) = default;
	FEntityWrapper(FEntityWrapper&& Other) = default;
	
	FEntityWrapper(entt::entity InEntity)
	: entity(InEntity),
	  EntityIndex(static_cast<uint32>(InEntity))
	{
	}
		
	FEntityWrapper(int32 InEntityIndex)
	: entity(static_cast<entt::entity>(InEntityIndex)),
	  EntityIndex(InEntityIndex)
	{
	}
			
	FEntityWrapper(uint32 InEntityIndex)
	: entity(static_cast<entt::entity>(InEntityIndex)),
	  EntityIndex(InEntityIndex)
	{
	}
	
	entt::entity entity;

	UPROPERTY(VisibleAnywhere)
	uint32 EntityIndex;

	FEntityWrapper& operator=(const FEntityWrapper& Other)
	{
		if (this == &Other)
			return *this;
		entity = Other.entity;
		EntityIndex = Other.EntityIndex;
		return *this;
	}

	FEntityWrapper& operator=(FEntityWrapper&& Other)
	{
		if (this == &Other)
			return *this;
		entity = Other.entity;
		EntityIndex = Other.EntityIndex;
		return *this;
	}
	
	friend bool operator==(FEntityWrapper const& Lhs, FEntityWrapper const& Rhs)
	{
		return Lhs.entity == Rhs.entity;
	}
	
	friend uint32 GetTypeHash(const FEntityWrapper& Arg)
	{
		return HashCombine(GetTypeHash(Arg.entity), GetTypeHash(Arg.EntityIndex));
	}
	
	operator entt::entity() const
	{
		return entity;
	}

	operator int32() const
	{
		return static_cast<int32>(entity);
	}

	operator uint32() const
	{
		return static_cast<uint32>(entity);
	}
};
// #endif

