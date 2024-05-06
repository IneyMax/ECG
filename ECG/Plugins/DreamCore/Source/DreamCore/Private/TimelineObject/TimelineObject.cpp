// Fill out your copyright notice in the Description page of Project Settings.


#include "TimelineObject/TimelineObject.h"

DEFINE_LOG_CATEGORY(LogTimelineObject);


void UTimelineObject::Tick(float DeltaTime)
{
	if (LastFrameNumberWeTicked == GFrameCounter)
	{
		return;
	}
	
	if (bIsPlaying)
	{
		UpdateTimeline(DeltaTime);
	}
	
	LastFrameNumberWeTicked = GFrameCounter;
}

UTimelineObject::UTimelineObject()
{
	CurveFloat = nullptr;

	bShouldLoop = false;
	bShouldChangeDirectionOnLoop = false;

	World = nullptr;

	CurrentDirection = ETimelineDirection::Type::Forward;

	CurrentTime = 0.f;
	CurrentValue = 0.f;

	CurveStartTime = 0.f;
	CurveEndTime = 0.f;

	CurrentPlayRate = 1.f;
	
	bIsPlaying = false;
	bHasFinished = false;
}

UTimelineObject* UTimelineObject::CreateTimeline(
	const UObject* WorldContextObject,
	UCurveFloat* InCurveFloat,
	bool bInShouldLoop,	bool bInShouldChangeDirectionOnLoop,
	FTimelineObjectDelegate TimelineUpdateEvent,
	FTimelineObjectDelegate TimelineFinishedEvent)
{
	UTimelineObject* NewTimelineObject = NewObject<UTimelineObject>();

	NewTimelineObject->World = WorldContextObject->GetWorld();
	NewTimelineObject->CurveFloat = InCurveFloat;
	NewTimelineObject->bShouldLoop = bInShouldLoop;
	NewTimelineObject->bShouldChangeDirectionOnLoop = bInShouldChangeDirectionOnLoop;
	NewTimelineObject->OnTimelineUpdate = TimelineUpdateEvent;
	NewTimelineObject->OnTimelineFinished = TimelineFinishedEvent;

	if (NewTimelineObject->CurveFloat)
	{
		NewTimelineObject->CurveFloat->GetTimeRange(NewTimelineObject->CurveStartTime, NewTimelineObject->CurveEndTime);
	}

	return NewTimelineObject;
}

void UTimelineObject::PlayForward(bool bFromStart/* = true*/)
{
	if (!World || !CurveFloat)
	{
		UE_CLOG(!World,				LogTimelineObject, Error, TEXT("Cannot play timeline object, World is nullptr."));
		UE_CLOG(!CurveFloat,		LogTimelineObject, Error, TEXT("Cannot play timeline object, CurveFloat is nullptr."));
		
		return;
	}

	CurrentDirection = ETimelineDirection::Type::Forward;

	if (bFromStart)
	{
		CurrentTime = CurveStartTime;
	}

	bIsPlaying = true;
}

void UTimelineObject::Reverse(bool bFromEnd/* = true*/)
{
	if (!World || !CurveFloat)
	{
		UE_CLOG(!World,				LogTimelineObject, Error, TEXT("Cannot reverse timeline object, World is nullptr."));
		UE_CLOG(!CurveFloat,		LogTimelineObject, Error, TEXT("Cannot reverse timeline object, CurveFloat is nullptr."));
		
		return;
	}

	CurrentDirection = ETimelineDirection::Type::Backward;

	if (bFromEnd)
	{
		CurrentTime = CurveEndTime;
	}
	
	bIsPlaying = true;
}

void UTimelineObject::Stop()
{
	if (!World || !bIsPlaying)
	{
		UE_CLOG(!World,			LogTimelineObject, Error, TEXT("Cannot stop timeline object, World is nullptr."));
		UE_CLOG(!bIsPlaying,	LogTimelineObject, Error, TEXT("Cannot stop timeline object, it's not playing."));
		
		return;
	}

	bIsPlaying = false;
}

void UTimelineObject::Continue()
{
	if (!World || bIsPlaying)
	{
		UE_CLOG(!World,			LogTimelineObject, Error, TEXT("Cannot continue timeline object, World is nullptr."));
		UE_CLOG(!bIsPlaying,	LogTimelineObject, Error, TEXT("Cannot continue timeline object, it's already playing."));
		
		return;
	}

	bIsPlaying = true;
}

void UTimelineObject::ChangeDirection()
{
	switch (CurrentDirection)
	{
	case ETimelineDirection::Type::Forward:
		{
			CurrentDirection = ETimelineDirection::Type::Backward;
		}
		break;
	case ETimelineDirection::Type::Backward:
		{
			CurrentDirection = ETimelineDirection::Type::Forward;
		}
		break;
	}
}

void UTimelineObject::SetPlayRate(const float InPlayRate/* = 1.f*/)
{
	if (InPlayRate <= 0.f)
	{
		UE_LOG(LogTimelineObject, Error, TEXT("UTimelineObject::SetPlayRate / Trying to set PlayRate to %f, it must be >= 0. Setting it to 1."), InPlayRate);
		CurrentPlayRate = 1.f;
	}
	else
	{
		CurrentPlayRate = InPlayRate;
	}
}

void UTimelineObject::SetNewTime(const float InNewTime)
{
	if (InNewTime < CurveStartTime || InNewTime > CurveEndTime)
	{
		UE_LOG(LogTimelineObject, Error, TEXT("UTimelineObject::SetNewTime / InNewTime %f must be inside range [%f; %f]."), InNewTime, CurveStartTime, CurveEndTime);
	}
	else
	{
		CurrentTime = InNewTime;
	}
}

void UTimelineObject::SetNewDirection(ETimelineDirection::Type InNewDirection)
{
	CurrentDirection = InNewDirection;
}

ETimelineDirection::Type UTimelineObject::GetDirection() const
{
	return CurrentDirection;
}

float UTimelineObject::GetTimePassed() const
{
	float TimePassed = 0.f;

	switch (CurrentDirection)
	{
	case ETimelineDirection::Type::Forward:
		{
			TimePassed = CurrentTime - CurveStartTime;
		}
		break;
	case ETimelineDirection::Type::Backward:
		{
			TimePassed = CurveEndTime - CurrentTime;
		}
		break;
	}

	if (CurrentPlayRate != 0)
	{
		TimePassed /= CurrentPlayRate;
	}
	
	return TimePassed;
}

float UTimelineObject::GetTimeLeft() const
{
	float TimeLeft = 0.f;

	switch (CurrentDirection)
	{
	case ETimelineDirection::Type::Forward:
		{
			TimeLeft = CurveEndTime - CurrentTime;
		}
		break;
	case ETimelineDirection::Type::Backward:
		{
			TimeLeft = CurrentTime - CurveStartTime;
		}
		break;
	}

	if (CurrentPlayRate != 0)
	{
		TimeLeft /= CurrentPlayRate;
	}
	
	return TimeLeft;
}

void UTimelineObject::UpdateTimeline(const float DeltaTime)
{
	bHasFinished = false;

	if (CurrentPlayRate <= 0.f)
	{
		UE_LOG(LogTimelineObject, Error, TEXT("UTimelineObject::UpdateTimeline / CurrentPlayRate is %f, it must be >= 0. Setting it to 1."), CurrentPlayRate);
		CurrentPlayRate = 1.f;
	}

	const float Delta = DeltaTime * CurrentPlayRate;
	
	switch (CurrentDirection)
	{
	case ETimelineDirection::Type::Forward:
		{
			CurrentTime += Delta;
			if (CurrentTime >= CurveEndTime)
			{
				CurrentTime = CurveEndTime;
				bHasFinished = true;
			}
		}
		break;
	case ETimelineDirection::Type::Backward:
		{
			CurrentTime -= Delta;
			if (CurrentTime <= CurveStartTime)
			{
				CurrentTime = CurveStartTime;
				bHasFinished = true;
			}
		}
		break;
	}

	CurrentValue = CurveFloat->GetFloatValue(CurrentTime);	
	OnTimelineUpdate.ExecuteIfBound(CurrentValue, CurrentDirection);

	// Uncomment if you need to log update values.
	// UE_LOG(LogTimelineObject, Display, TEXT("UTimelineObject::Update / Direction: %s / PlayRate: %f / CurrentTime: %f / CurrentValue: %f"),
	// 	*UEnum::GetValueAsString(CurrentDirection), CurrentPlayRate,  CurrentTime, CurrentValue);

	if (bHasFinished)
	{
		if (!bShouldLoop)
		{
			Stop();
		}
		else
		{
			// Restart timeline
			
			if (bShouldChangeDirectionOnLoop)
			{
				ChangeDirection();
			}

			switch (CurrentDirection)
			{
			case ETimelineDirection::Type::Forward:
				{
					PlayForward(true);
				}
				break;
			case ETimelineDirection::Type::Backward:
				{
					Reverse(true);
				}
				break;;
			}
		}
		
		OnTimelineFinished.ExecuteIfBound(CurrentValue, CurrentDirection);
	}
}
