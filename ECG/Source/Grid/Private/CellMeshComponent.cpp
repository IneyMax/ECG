// Fill out your copyright notice in the Description page of Project Settings.


#include "CellMeshComponent.h"


UCellMeshComponent::UCellMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
	bAutoActivate = true;
}
