﻿

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TestSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ECG_API UTestSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};