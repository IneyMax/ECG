#pragma once

struct FFraction
{
	
};

struct FRunConfig
{
	int32 RandSeed = INDEX_NONE;
	int32 HeroID = INDEX_NONE;
	int32 DifficultyID = INDEX_NONE;
	TArray<FFraction> StartingFractions;
};

struct FCurrentPhaseTag {};

struct FPreparationPhase
{
	
};

struct FActionPhase
{
	
};

struct FCombatPhase
{
	
};

struct FResultPhase
{
	
};

struct FLevelComponent
{
	int32 Level = INDEX_NONE;
};

struct FExperienceComponent
{
	float Experience = 0.f;
};

struct FHealthComponent
{
	float Health = 0.f;
};

struct FHeroState
{
	int32 HeroID = INDEX_NONE;
	// Abilities: AbilityInstance[]
	// Talents: TalentID[]
};

struct FMakePurchaseCommand
{
	
};

struct FEndPhaseCommand
{
	
};

struct FShopRerollCommand
{
	
};

struct FSelectAreaCommand
{
	
};


struct FShopSlotIndex
{
	int32 Index = INDEX_NONE;
};

struct FShopOffer
{
		
};
