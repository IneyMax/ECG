// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UISubsystem.h"
#include "Engine/DeveloperSettings.h"
#include "UISoundSettings.generated.h"

class UUISubsystem;

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "UI sounds Settings"))
class SMARTUI_API UUISoundSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UUISoundSettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Sounds")
	TMap<EUISoundType, TSoftObjectPtr<USoundBase>> SoundPresets;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Sounds")
	int32 MaxCountSounds;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Sounds")
	float SpawnSoundDelay;
};
