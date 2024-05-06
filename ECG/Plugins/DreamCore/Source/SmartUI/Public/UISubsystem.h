// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/WidgetAnimation.h"
#include "UISubsystem.generated.h"

UENUM(BlueprintType)
enum class EUISoundType : uint8
{
	Forward,
	Back,
	Set,
	Hover,
	KeyClick
};

UCLASS()
class SMARTUI_API UUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UUISubsystem();

	UFUNCTION(BlueprintCallable)
	void PlaySound2D(EUISoundType SoundType, float VolumeMultiplier = 1.0f, float PitchMultiplie = 1.0f, float StartTime = 0.0f, USoundConcurrency* ConcurrencySettings = nullptr, bool bPersistAcrossLevelTransition = false);

	UPROPERTY(EditDefaultsOnly)
	TMap<EUISoundType, TSoftObjectPtr<USoundBase>> SoundPresets;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	static void PlayHoverAnimation(UUserWidget* Widget, UWidgetAnimation* Animation, bool bPlaySound = true, float PlayRate = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "UI")
	static void PlayUnhoverAnimation(UUserWidget* Widget, UWidgetAnimation* Animation, float PlayRate = 1.0f);

protected:
	UPROPERTY()
	TArray<UAudioComponent*> Sounds;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxCountSounds;

	UPROPERTY(EditDefaultsOnly)
	bool SpawnSoundBlock;

	UPROPERTY(EditDefaultsOnly)
	float SpawnSoundDelay;

	FTimerHandle BlockTimer;
};
