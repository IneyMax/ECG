// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ESC_FunctionLibrary.generated.h"

class AECSBasedActor;


UCLASS()
class ENTITYREPRESENTATIONS_API UESC_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "ESC")
	static AECSBasedActor* SpawnEntityBasedActor(UObject* WorldContext, TSubclassOf<AActor> Class,
	                                             const FTransform& Transform);
};
