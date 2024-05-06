// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageBusNetComponent.h"
#include "MessageBusSubsystem.h"

// Sets default values for this component's properties
UMessageBusNetComponent::UMessageBusNetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
}

void UMessageBusNetComponent::OnRegister()
{
	Super::OnRegister();

	checkf(Cast<APlayerController>(GetOwner()), TEXT("Only player controller can be Owner for UMessageBusNetComponent, current owner is %s"), *GetNameSafe(GetOwner()));

	if (UMessageBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UMessageBusSubsystem>())
	{
		Subsystem->RegisterNetConnection(this);
	}
}

void UMessageBusNetComponent::OnUnregister()
{
	Super::OnUnregister();
	if (UMessageBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UMessageBusSubsystem>())
	{
		Subsystem->UnregisterNetConnection(this);
	}
}

void UMessageBusNetComponent::PushNewEvent(FMessagePayloadEventFullInformation const& NewEvent)
{
	const int32 NewEventId = NewEvent.GetEventId();
	checkf(!IsEventPushed(NewEventId), TEXT("Try again push event %s with id"), *NewEvent.GetKey().ToString(), NewEventId);

	UE_LOG(LogMessageBus, Verbose, TEXT("Push event %s:%i to client connection"), *NewEvent.GetKey().ToString(), NewEventId);


	PushedEventIds.Add(NewEventId);
	ReplicateEventClient(NewEvent);
}

void UMessageBusNetComponent::CancelEvent(int32 EventId)
{
	checkf(IsEventPushed(EventId), TEXT("Try cancel non pushed for this connection event %i"), EventId);
	UE_LOG(LogMessageBus, Verbose, TEXT("Cancel event %i for client connection"), EventId);

	CancelEventClient(EventId);
}

void UMessageBusNetComponent::ReplicateEventClient_Implementation(FMessagePayloadEventFullInformation const& NewEvent)
{
	UE_LOG(LogMessageBus, Verbose, TEXT("Event %s:%i replicated to client"), *NewEvent.GetKey().ToString(), NewEvent.GetEventId());

	if (UMessageBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UMessageBusSubsystem>())
	{
		Subsystem->OnIncomeNewEventFromReplication(this, NewEvent);
	}
}


void UMessageBusNetComponent::CancelEventClient_Implementation(int32 EventId)
{
	UE_LOG(LogMessageBus, Verbose, TEXT("Cancel event replicated %i for client connection"), EventId);

	if (UMessageBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UMessageBusSubsystem>())
	{
		Subsystem->OnEventCanceledFromReplication(this, EventId);
	}
}
