// Fill out your copyright notice in the Description page of Project Settings.


#include "ECSBasedActor.h"


// Sets default values
AECSBasedActor::AECSBasedActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AECSBasedActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AECSBasedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

