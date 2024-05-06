// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "TimelineObject.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTimelineObject, Display, All);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FTimelineObjectDelegate, float, CurrentValue, ETimelineDirection::Type, Direction);

/**
 * 
 */
UCLASS(BlueprintType)
class DREAMCORE_API UTimelineObject : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

	// FTickableGameObject Begin
public:
	virtual void Tick(float DeltaTime) override;
	
	virtual ETickableTickType GetTickableTickType() const override
	{
		return HasAnyFlags(RF_ClassDefaultObject) ? ETickableTickType::Never : ETickableTickType::Conditional;
	}
	
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT( FMyTickableThing, STATGROUP_Tickables );
	}
	
	virtual bool IsTickable() const override
	{
		return bIsPlaying;
	}
	
	virtual bool IsTickableWhenPaused() const override
	{
		return true;
	}
	
	virtual bool IsTickableInEditor() const override
	{
		return false;
	}

private:
	uint32 LastFrameNumberWeTicked = INDEX_NONE;
	// FTickableGameObject End

public:
	UTimelineObject();

	UFUNCTION(BlueprintCallable, Category = "TimelineObject|Construction", meta = (WorldContext = "WorldContextObject"))
	static UTimelineObject* CreateTimeline(
		const UObject* WorldContextObject,
		UCurveFloat* InCurveFloat,
		bool bInShouldLoop,
		bool bInShouldChangeDirectionOnLoop,
		FTimelineObjectDelegate TimelineUpdateEvent,
		FTimelineObjectDelegate TimelineFinishedEvent);

	/**
	 * Timeline starts playing forward along curve.
	 * 
	 * @param bFromStart True if should start from the start of the curve, false if starts from current position.
	 */
	UFUNCTION(BlueprintCallable, Category = "TimelineObject")
	void PlayForward(bool bFromStart = true);

	/**
	 * Timeline starts playing backward along curve.
	 * 
	 * @param bFromEnd True if should start from the end of the curve, false if starts from current position.
	 */
	UFUNCTION(BlueprintCallable, Category = "TimelineObject")
	void Reverse(bool bFromEnd = true);

	/**
	 * Timeline stops playing.
	 */
	UFUNCTION(BlueprintCallable, Category = "TimelineObject")
	void Stop();

	/**
	 * Timeline continues playing from his current time in his current direction.
	 */
	UFUNCTION(BlueprintCallable, Category = "TimelineObject")
	void Continue();

	/**
	 * Changes direction of timeline to the opposite.
	 */
	UFUNCTION(BlueprintCallable, Category = "TimelineObject")
	void ChangeDirection();

	/**
	 * Setting timeline's play rate.
	 * The higher the play rate - the faster timeline plays
	 * (eg. if play rate = 2, then timeline plays 2 times faster,
	 * but if play rate = 0.5, then timeline plays 2 times slower).
	 * 
	 * @param InPlayRate New play rate value (must be >= 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "TimelineObject|Setters")
	void SetPlayRate(const float InPlayRate = 1.f);

	/**
	 * Setting new time for timeline.
	 * 
	 * @param InNewTime New time value (must be inside curve's time range). 
	 */
	UFUNCTION(BlueprintCallable, Category = "TimelineObject|Setters")
	void SetNewTime(const float InNewTime);

	/**
	 * Setting new direction of the timeline.
	 * 
	 * @param InNewDirection New direction of the timeline.
	 */
	UFUNCTION(BlueprintCallable, Category = "TimelineObject|Setters")
	void SetNewDirection(ETimelineDirection::Type InNewDirection);

	/**
	 * @return Current direction of the timeline. 
	 */
	UFUNCTION(BlueprintCallable, Category = "TimelineObject|Getters")
	ETimelineDirection::Type GetDirection() const;

	/**
	 * @return Time in seconds between starting of timeline and his current position (considering direction). 
	 */
	UFUNCTION(BlueprintPure, Category = "TimelineObject|Getters")
	float GetTimePassed() const;

	/**
	 * @return Time in seconds between ending of timeline and his current position (considering direction). 
	 */
	UFUNCTION(BlueprintPure, Category = "TimelineObject|Getters")
	float GetTimeLeft() const;

private:	
	void UpdateTimeline(const float DeltaTime);
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimelineObject", meta = (ExposeOnSpawn))
	UCurveFloat* CurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimelineObject", meta = (ExposeOnSpawn))
	bool bShouldLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TimelineObject", meta = (ExposeOnSpawn))
	bool bShouldChangeDirectionOnLoop;
	
	FTimelineObjectDelegate OnTimelineUpdate;
	FTimelineObjectDelegate OnTimelineFinished;

private:
	UPROPERTY()
	UWorld* World;

	ETimelineDirection::Type CurrentDirection;

	float CurrentTime;
	float CurrentValue;

	float CurveStartTime;
	float CurveEndTime;

	float CurrentPlayRate;

	bool bIsPlaying;
	bool bHasFinished;
};
