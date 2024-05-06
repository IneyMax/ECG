// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessageBusTypes.h"

#include "MessageBusNetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DCMESSAGEBUS_API UMessageBusNetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMessageBusNetComponent();

	virtual void OnRegister() override;
	virtual void OnUnregister() override;
public:


	void PushNewEvent(FMessagePayloadEventFullInformation const& NewEvent);
	void CancelEvent(int32 EventId);

	bool IsEventPushed(int32 EventId) const;

protected:
	UFUNCTION(Client, Reliable)
	void ReplicateEventClient(FMessagePayloadEventFullInformation const& NewEvent);
	
	UFUNCTION(Client, Reliable)
	void CancelEventClient(int32 EventId);

protected:

	UPROPERTY(VisibleAnywhere)
	TArray<int32> PushedEventIds;
};

FORCEINLINE bool UMessageBusNetComponent::IsEventPushed(int32 EventId) const
{
	return PushedEventIds.Contains(EventId);
}
