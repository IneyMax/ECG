// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "entity/registry.hpp"
#include "Subsystems/WorldSubsystem.h"
#include "GameEntitySubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameEntitySubsystem, Log, All);


UCLASS()
class ENTITYSTORAGE_API UGameEntitySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static ThisClass* Get(const UObject* WorldContext)
	{
		return WorldContext->GetWorld()->GetSubsystem<ThisClass>();
	}

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	static entt::registry& GetRegistry(const UObject* WorldContext)
	{
		return Get(WorldContext)->Registry;
	};

protected:
	entt::registry Registry;
};
