// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartUI/Public/UISubsystem.h"
#include "UISoundSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Blueprint/UserWidget.h"

UUISubsystem::UUISubsystem()
{
	MaxCountSounds = 3; //81% 
	SpawnSoundBlock = false;
	SpawnSoundDelay = 0.05f;
}

void UUISubsystem::PlaySound2D(EUISoundType SoundType, float VolumeMultiplier, float PitchMultiplie, float StartTime, USoundConcurrency* ConcurrencySettings, bool bPersistAcrossLevelTransition)
{
	if (SpawnSoundBlock) return;

	USoundBase* SoundBase = nullptr;
		
	if (SoundPresets.Contains(SoundType))
	{
		SoundBase = SoundPresets[SoundType].LoadSynchronous();
	}

	if (!SoundBase) return;

	UAudioComponent* Audio = UGameplayStatics::SpawnSound2D(GetWorld(), SoundBase, VolumeMultiplier, PitchMultiplie, StartTime, ConcurrencySettings, bPersistAcrossLevelTransition, true);
	Audio->OnAudioFinishedNative.AddWeakLambda(this,
		[this](UAudioComponent* Audio)
		{
			if (Sounds.Contains(Audio))
			{
				Sounds.Remove(Audio);
			}
		});
	Sounds.Add(Audio);

	//Remove all invalid entries
	Sounds.RemoveAll([](UAudioComponent* Item)
		{
			return !IsValid(Item);
		});

	if (Sounds.Num() > MaxCountSounds && Sounds.IsValidIndex(0))
	{
		if (UAudioComponent* Sound = Sounds[0])
		{
			Sound->FadeOut(0.1f, 0);
			Sound->OnAudioFinishedNative.RemoveAll(this);
			Sounds.Remove(Sound);
		}

	}

	SpawnSoundBlock = true;

	GetWorld()->GetTimerManager().SetTimer(BlockTimer, FTimerDelegate::CreateWeakLambda(this,
		[this]
		{
			SpawnSoundBlock = false;
		}
	), SpawnSoundDelay, false);
}

void UUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UUISoundSettings* SoundSettings = GetDefault<UUISoundSettings>();
	SoundPresets = SoundSettings->SoundPresets;
	MaxCountSounds = SoundSettings->MaxCountSounds;
	SpawnSoundDelay = SoundSettings->SpawnSoundDelay;
}

void UUISubsystem::PlayHoverAnimation(UUserWidget* Widget, UWidgetAnimation* Animation, bool bPlaySound, float PlayRate)
{
	float AnimationTime = Widget->GetAnimationCurrentTime(Animation);
	Widget->PlayAnimation(Animation, Animation->GetStartTime() + AnimationTime, 1, EUMGSequencePlayMode::Forward, PlayRate);

	if (bPlaySound)
	{
		UUISubsystem* UISubsystem = Widget->GetWorld()->GetGameInstance()->GetSubsystem<UUISubsystem>();

		if (IsValid(UISubsystem))
		{
			UISubsystem->PlaySound2D(EUISoundType::Hover);
		}
	}
}

void UUISubsystem::PlayUnhoverAnimation(UUserWidget* Widget, UWidgetAnimation* Animation, float PlayRate)
{
	float AnimationTime = Widget->GetAnimationCurrentTime(Animation);

	if (AnimationTime == 0.0f) AnimationTime = Animation->GetEndTime();

	Widget->PlayAnimation(Animation, Animation->GetEndTime() - AnimationTime, 1, EUMGSequencePlayMode::Reverse, PlayRate);
}