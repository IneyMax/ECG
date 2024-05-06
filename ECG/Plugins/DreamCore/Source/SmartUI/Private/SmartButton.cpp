// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartButton.h"
#include "UISubsystem.h"

USmartButton::USmartButton(const FObjectInitializer& Initializer) : Super(Initializer)
{
	bActiveStatus = true;
	AlwaysEnabled = false;
	SoundType = EUISoundType::Forward;
	CosmeticDelay = 0.3f;
	HitboxScale = 1.0f;
	bPlayInactiveAnimation = false;
	CurrentActivationTime = 0.f;
	ActivationTime = 0.f;
}

void USmartButton::ForceUnhover()
{
	if (AnimationList.Contains(AnimationType) && (bPlayInactiveAnimation || bActiveStatus))
	{
		UUISubsystem::PlayUnhoverAnimation(this, AnimationList[AnimationType], 10.f);
	}
}

void USmartButton::NativeConstruct()
{
	BaseButton->OnReleased.AddDynamic(this, &USmartButton::OnClickedNative);
	BaseButton->OnHovered.AddDynamic(this, &USmartButton::OnHoveredNative);
	BaseButton->OnUnhovered.AddDynamic(this, &USmartButton::OnUnhoveredNative);

	Super::NativeConstruct();

	if(bUseActivationTimer)
	{
		StartActivationTimer();
	}
}

void USmartButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetActiveStatus(bActiveStatus);
	SetRenderScale(FVector2D(HitboxScale, HitboxScale));
}

void USmartButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(CurrentActivationTime > 0.f)
	{
		CurrentActivationTime -= InDeltaTime;

		if(CurrentActivationTime <= 0.f)
		{
			SetActiveStatus(true);
		}
	}
}

void USmartButton::OnHoveredNative()
{
	if (AnimationList.Contains(AnimationType) && (bPlayInactiveAnimation || bActiveStatus))
	{
		StopAllAnimations();
		UUISubsystem::PlayHoverAnimation(this, AnimationList[AnimationType]);
	}

	OnHovered.Broadcast();
}

void USmartButton::OnUnhoveredNative()
{
	if (AnimationList.Contains(AnimationType) && (bPlayInactiveAnimation || bActiveStatus))
	{
		UUISubsystem::PlayUnhoverAnimation(this, AnimationList[AnimationType]);
	}

	OnUnhovered.Broadcast();
}

void USmartButton::OnClickedNative()
{
	if (!IsHovered()) return;

	UUISubsystem* UISubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UUISubsystem>();

	if (IsValid(UISubsystem))
	{
		UISubsystem->PlaySound2D(SoundType);
	}

	GetWorld()->GetTimerManager().ClearTimer(CosmeticTimer);
	//Delay for sound playing before level steaming
	GetWorld()->GetTimerManager().SetTimer(CosmeticTimer, FTimerDelegate::CreateWeakLambda(this, 
		[this] {
			(bActiveStatus ? OnClickedActive : OnClickedInactive).Broadcast();
		}), CosmeticDelay + 0.01, false);
}

void USmartButton::StartActivationTimer()
{
	SetActiveStatus(false);
	CurrentActivationTime = ActivationTime;
}

void USmartButton::SetActiveStatus(bool bStatus)
{
	BaseButton->SetStyle(bStatus ? ActiveStyle : InactiveStyle);

	if (bStatus == bActiveStatus) return;

	bActiveStatus = bStatus;

	if (BaseButton->IsHovered() && !bStatus && !bPlayInactiveAnimation && AnimationList.Contains(AnimationType))
	{
		UUISubsystem::PlayUnhoverAnimation(this, AnimationList[AnimationType]);
	}

	OnChangeActiveStatus.Broadcast(bActiveStatus);
}

