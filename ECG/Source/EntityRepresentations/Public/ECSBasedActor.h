// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECSBasedActor.generated.h"


UCLASS()
class ENTITYREPRESENTATIONS_API AECSBasedActor : public AActor
{
	GENERATED_BODY()

public:
	AECSBasedActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
