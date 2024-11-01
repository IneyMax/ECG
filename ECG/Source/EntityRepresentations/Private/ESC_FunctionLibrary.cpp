// Fill out your copyright notice in the Description page of Project Settings.


#include "ESC_FunctionLibrary.h"

#include "ECSBasedActor.h"


AECSBasedActor* UESC_FunctionLibrary::SpawnEntityBasedActor(UObject* WorldContext, const TSubclassOf<AActor> Class,
                                                            const FTransform& Transform)
{
	AECSBasedActor* Actor = WorldContext->GetWorld()->SpawnActorDeferred<AECSBasedActor>(Class, Transform,
		static_cast<AActor*>(WorldContext), static_cast<APawn*>(WorldContext));
	return Actor;
}

