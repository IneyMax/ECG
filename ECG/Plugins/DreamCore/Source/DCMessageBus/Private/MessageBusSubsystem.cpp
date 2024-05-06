// Copyright DreamVR. All Rights Reserved.


#include "MessageBusSubsystem.h"
#include "Algo/Transform.h"
#include "GameFramework/GameStateBase.h"
#include "MessageBusNetComponent.h"
#include "MessageBusListenerComponent.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogMessageBus);

DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("MessageBus listeners registered"),STAT_ListenersRegistered,STATGROUP_MessageBus);
DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("MessageBus events sent"),STAT_EventsSent,STATGROUP_MessageBus);
DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("MessageBus current temporal events num"),STAT_TemporalEvents,STATGROUP_MessageBus);
DECLARE_MEMORY_STAT(TEXT("MessageBus memory used by keeping temp events"), STAT_TempEventsMemory, STATGROUP_MessageBus);
DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("MessageBus payload types"),STAT_PayloadTypes,STATGROUP_MessageBus);
DECLARE_DWORD_ACCUMULATOR_STAT(TEXT("MessageBus events replicated"), STAT_EventsReplicated,STATGROUP_MessageBus);
DECLARE_MEMORY_STAT(TEXT("MessageBus events replicated memory"), STAT_EventsReplicatedMemory, STATGROUP_MessageBus);

TArray<UScriptStruct*> UMessageBusSubsystem::AllMessageBusTypes = TArray<UScriptStruct*>();


void FMessageBusSubsystemTick::Tick(float DeltaTime)
{
	if (Subsystem.IsValid())
	{
		Subsystem->SubsystemTick(DeltaTime);
	}
}

TStatId FMessageBusSubsystemTick::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FMessageBusSubsystemTick, STATGROUP_Tickables);
}

void UMessageBusSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	SET_DWORD_STAT(STAT_ListenersRegistered,0);
	SET_DWORD_STAT(STAT_EventsSent,0);
	SET_DWORD_STAT(STAT_TemporalEvents,0);
	SET_MEMORY_STAT(STAT_TempEventsMemory,0);
	SET_DWORD_STAT(STAT_EventsReplicated,0);
	SET_MEMORY_STAT(STAT_EventsReplicatedMemory,0);
	SET_DWORD_STAT(STAT_PayloadTypes,0);
	
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(this, [this](UWorld* NewWorld)
		{
			ActualGameWorld = NewWorld;
			if (ActualGameWorld.IsValid())
			{
				if (ActualGameWorld->GetGameState())
				{
					OnGameStateRecieved(ActualGameWorld->GetGameState());
				}
				else
				{
					ActualGameWorld->GameStateSetEvent.AddWeakLambda(this, [this](AGameStateBase* GameState)
						{
							OnGameStateRecieved(GameState);
						});
				}

			}
		});

	SubsystemTickableObject = MakeShared<FMessageBusSubsystemTick>(this);
	CurrentMessageId = 0;
	PopulateAllMessageBusTypes();

	GetMessageBusInitialized().Broadcast();
}


void UMessageBusSubsystem::PopulateAllMessageBusTypes()
{
	UMessageBusSubsystem::AllMessageBusTypes.Empty();

	const UScriptStruct* BaseStruct = FMessagePayloadBase::StaticStruct();

	UE_LOG(LogMessageBus, Log, TEXT("Populate all message bus event types with base %s"), *GetNameSafe(BaseStruct));

	TSet<const UScriptStruct*> Collected;
	TSet<const UScriptStruct*> Visited;
	for (TObjectIterator<UScriptStruct> StructIt; StructIt; ++StructIt)
	{
		const UScriptStruct* CurrentStruct = *StructIt;
		if (Visited.Contains(CurrentStruct))
		{
			continue;
		}
		Visited.Add(CurrentStruct);
		if (CurrentStruct->IsChildOf(BaseStruct))
		{
			Collected.Add(CurrentStruct);
		}

	}

	Algo::Transform(Collected, UMessageBusSubsystem::AllMessageBusTypes, []
	(const UScriptStruct* Item) ->UScriptStruct*
		{
			UE_LOG(LogMessageBus, Log, TEXT("Message bus stript struct %s"), *GetNameSafe(Item));
			return const_cast<UScriptStruct*>(Item);
		});

	Algo::Sort(UMessageBusSubsystem::AllMessageBusTypes, []
	(const UScriptStruct* Lhs, const UScriptStruct* Rhs) -> bool
		{
			const FString LName = Lhs->GetStructCPPName();
			const FString RName = Rhs->GetStructCPPName();

			return LName < RName;
		});

	UE_LOG(LogMessageBus, Log, TEXT("Populate all message bus event finished, %i types"), UMessageBusSubsystem::AllMessageBusTypes.Num());
	SET_DWORD_STAT(STAT_PayloadTypes,UMessageBusSubsystem::AllMessageBusTypes.Num());
}

void UMessageBusSubsystem::Deinitialize()
{
	ActualGameWorld.Reset();
	SubsystemTickableObject.Reset();
	
	GetMessageBusDeinitialized().Broadcast();
}

int UMessageBusSubsystem::GetStructIndex(UScriptStruct* Struct)
{
	checkf(UMessageBusSubsystem::AllMessageBusTypes.Num(), TEXT("Try GetStructIndex before message bus initialize"));
	return UMessageBusSubsystem::AllMessageBusTypes.IndexOfByKey(Struct);
}

UScriptStruct* UMessageBusSubsystem::GetStructByIndex(int Index)
{
	checkf(UMessageBusSubsystem::AllMessageBusTypes.Num(), TEXT("Try GetStructByIndex before message bus initialize"));
	return UMessageBusSubsystem::AllMessageBusTypes.IsValidIndex(Index) ? UMessageBusSubsystem::AllMessageBusTypes[Index] : nullptr;
}

FMessageBusInitialized& UMessageBusSubsystem::GetMessageBusInitialized()
{
	static FMessageBusInitialized MessageBusInitialized;
	return MessageBusInitialized;
}

FMessageBusDeinitialized& UMessageBusSubsystem::GetMessageBusDeinitialized()
{
	static FMessageBusDeinitialized MessageBusDeinitialized;
	return MessageBusDeinitialized;
}

int UMessageBusSubsystem::GetPayloadSize(const FMessagePayloadEventFullInformation& Event)
{
	if (!Event.Event.PayloadContainer.Payload.IsValid()) return 0;
	if (const UScriptStruct* ScriptStruct = Event.Event.PayloadContainer.Payload.Get()->GetPayloadType())
	{
		return ScriptStruct->GetStructureSize();
	}
	return 0;
}

int UMessageBusSubsystem::GetEventSizeWithPayload(const FMessagePayloadEventFullInformation& Event)
{
	return Event.StaticStruct()->GetStructureSize()+GetPayloadSize(Event);
}

UMessageBusSubsystem* UMessageBusSubsystem::Get(const UObject* WorldContext)
{
	if (!(WorldContext && IsValid(WorldContext)))
	{
		return nullptr;
	}
	
	return WorldContext->GetWorld()->GetSubsystem<UMessageBusSubsystem>();
}

void UMessageBusSubsystem::RegisterNetConnection(UMessageBusNetComponent* InConnection)
{
	RemoveInvalidNetConnections();
	if (IsValid(InConnection))
	{
		if (!RegisteredConnections.Contains(InConnection))
		{
			RegisteredConnections.AddUnique(InConnection);
			OnConnectionAdded(InConnection);
		}

	}
}

void UMessageBusSubsystem::UnregisterNetConnection(UMessageBusNetComponent* InConnection)
{
	RemoveInvalidNetConnections();
	if (IsValid(InConnection))
	{
		RegisteredConnections.Remove(InConnection);
		OnConnectionRemoved(InConnection);
	}
}

void UMessageBusSubsystem::RegisterListener(UMessageListenerBase* InListener)
{
	RemoveInvalidListeners();
	if (IsValid(InListener))
	{
		UE_LOG(LogMessageBus, Verbose, TEXT("Register listener %s for event %s, existed temporal events %i"), *GetNameSafe(InListener), *InListener->GetDebugString(), TemporalAndPersistentEvents.Num());
		RegisteredListeners.AddUnique(InListener);
		SET_DWORD_STAT(STAT_ListenersRegistered, RegisteredListeners.Num());

		//listen all temporal events

		bool bShouldDeactivateAfterCall = false;

		for (auto EventIt = TemporalAndPersistentEvents.CreateConstIterator(); EventIt; ++EventIt)
		{
			FMessageBusEvent Event = EventIt->Event;
			if (InListener->ShouldReactOnEvent(Event))
			{
				UE_LOG(LogMessageBus, Verbose, TEXT("Listener %s reacted on event %s"), *GetNameSafe(InListener), *Event.EventKey.ToString());

				InListener->ReceiveMessageBusEvent(Event, EEventAction::Appear);

				if (InListener->IsFireOnce())
				{
					bShouldDeactivateAfterCall = InListener->IsFireOnce();
					break;
				}
			}
			else
			{
				UE_LOG(LogMessageBus, VeryVerbose, TEXT("Listener %s skip reaction for %s"), *GetNameSafe(InListener), *Event.EventKey.ToString());
			}
		}
		if (bShouldDeactivateAfterCall)
		{
			InListener->Deactivate();
		}
	}
}

void UMessageBusSubsystem::UnregisterListener(UMessageListenerBase* InListener)
{
	RemoveInvalidListeners();
	if (IsValid(InListener))
	{
		UE_LOG(LogMessageBus, Verbose, TEXT("Unregister listener %s for event %s"), *GetNameSafe(InListener), *InListener->GetDebugString());
		RegisteredListeners.RemoveSingleSwap(InListener);
		SET_DWORD_STAT(STAT_ListenersRegistered, RegisteredListeners.Num());
	}
}

void UMessageBusSubsystem::CancelTemporalEvent(int32 EventId, const FMessagePayloadEventFullInformation& EventToCancel)
{
	if (EventToCancel.IsReplicated())
	{
		TArray<UMessageBusNetComponent*> ConnectionsToCancel = RegisteredConnections.FilterByPredicate([EventId](UMessageBusNetComponent* Connection)
		{
			return Connection->IsEventPushed(EventId);
		});
		if (ConnectionsToCancel.Num())
		{
			for (UMessageBusNetComponent* Connection : ConnectionsToCancel)
			{
				Connection->CancelEvent(EventId);
			}
		}
	}
			
	CancelTemporalEvent_Internal(EventToCancel);
}

void UMessageBusSubsystem::CancelEvent(int32 EventId)
{
	const bool bHasEvent = AllEvents.Contains(EventId);
	UE_LOG(LogMessageBus, Verbose, TEXT("Cancel event by id %i, exists %i"), EventId, bHasEvent);


	if (bHasEvent)
	{
		FMessagePayloadEventFullInformation EventToCancel;
		if(FindTemporalEventInfoById(EventId, EventToCancel))
		{
			CancelTemporalEvent(EventId, EventToCancel);
		}
	}
}

int UMessageBusSubsystem::CancelAllMatchedEvents(const FGameplayTag& BusKey, const FGameplayTag& Key)
{
	TArray<int32> Events = FindTemporalEventIdsByKey(BusKey, Key);
	for (int32 EventId : Events)
	{
		CancelEvent(EventId);
	}
	return Events.Num();
}



void UMessageBusSubsystem::OnEventCanceledFromReplication(UMessageBusNetComponent* Connection, int32 EventId)
{
	UE_LOG(LogMessageBus, Verbose, TEXT("Replicate [%s] cancel event id %i"), *GetNameSafe(Connection), EventId);

	FMessagePayloadEventFullInformation EventToCancel;
	if (FindTemporalEventInfoById(EventId, EventToCancel))
	{
		CancelTemporalEvent_Internal(EventToCancel);
	}
}

FMessagePayloadEventFullInformation& UMessageBusSubsystem::BuildEventFullInformation(FMessageBusEvent const& Event)
{
	// TODO Critical: we have mixed ID's of local events and events from the server: the server and the client can generate different events with the same ID. A ID redirector or something like that should be implemented
	const int32 EventId = ++CurrentMessageId + GetEventIdShift();
	
	FMessageBusEvent NewEvent = Event;
	NewEvent.MessageId = EventId;
	NewEvent.EventTime = GetServerWorldTime();

	FMessagePayloadEventFullInformation EventFullInformation;
	EventFullInformation.Event = NewEvent;

	AllEvents.Emplace(EventId, EventFullInformation);
	SET_DWORD_STAT(STAT_EventsSent,AllEvents.Num());
	return AllEvents[EventId];
}

int32 UMessageBusSubsystem::BroadcastEvent_Internal(const FMessagePayloadEventFullInformation& EventInformation)
{
	UE_LOG(LogMessageBus, Log, TEXT("Broadcast event internal %s:%i [%s]"), *EventInformation.GetKey().ToString(), EventInformation.GetEventId(), * UEnum::GetValueAsString(EventInformation.RoutingInfo.ReplicationRule));

	INC_DWORD_STAT(STAT_EventsSent);
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("MessageBus routing time"),STAT_RoutingTime,STATGROUP_MessageBus);
	
	switch (EventInformation.RoutingInfo.ReplicationRule)
	{
	case EMessageReplicationRule::NoReplicate:
	{
		if (EventInformation.Event.LifeType == EMessageLifeType::Temporal || EventInformation.Event.LifeType == EMessageLifeType::Persistent)
		{
			RegisterTemporalEvent(EventInformation);
		}
		OnIncomeNewEvent(EventInformation.Event);
	}break;
	case EMessageReplicationRule::NetMulticast:
	{
		ReplicateEventForAllClients(EventInformation);
		//Any server or standalone game should route event to himself withot connection
		if (GetWorld() && (GetWorld()->GetNetMode() != NM_Client))
		{
			if (EventInformation.Event.LifeType == EMessageLifeType::Temporal || EventInformation.Event.LifeType == EMessageLifeType::Persistent)
			{
				RegisterTemporalEvent(EventInformation);
			}
			OnIncomeNewEvent(EventInformation.Event);
;		}
	}break;
	case EMessageReplicationRule::ForSpecificClient:
	{
		if (EventInformation.RoutingInfo.SpecificClient.IsValid())
		{
			ReplicateEventForSpecificClient(EventInformation);
		}
	}break;
	default:
	{

	}break;
	}

	return EventInformation.GetEventId();
}

void UMessageBusSubsystem::CancelTemporalEvent_Internal(const FMessagePayloadEventFullInformation& EventInformation)
{
	UE_LOG(LogMessageBus, Log, TEXT("Cancel event internal %s [%s]"), *EventInformation.GetKey().ToString(), *UEnum::GetValueAsString(EventInformation.RoutingInfo.ReplicationRule));

	FMessagePayloadEventFullInformation CachedEvent = EventInformation;
	UnregisterTemporalEvent(EventInformation);
	OnEventCanceled(CachedEvent.Event);
}

float UMessageBusSubsystem::GetEventRemainingTime(const FMessageBusEvent& InEvent) const
{
	const float AppearTime = InEvent.GetEventTime();
	const float ServerTime = GetServerWorldTime();
	const float LifeTime = InEvent.GetLifeTime();
	
	return (AppearTime + LifeTime) - ServerTime;
}

float UMessageBusSubsystem::GetEventRemainingTime(int32 EventId) const
{
	FMessagePayloadEventFullInformation Info;
	if (FindTemporalEventInfoById(EventId, Info))
	{
		return GetEventRemainingTime(Info.Event);
	}

	return 0.0f;
}

float UMessageBusSubsystem::GetEventElapsedTime(const FMessageBusEvent& InEvent) const
{
	const float AppearTime = InEvent.GetEventTime();
	const float ServerTime = GetServerWorldTime();

	return ServerTime - (AppearTime);
}

float UMessageBusSubsystem::GetEventElapsedTime(int32 EventId) const
{
	FMessagePayloadEventFullInformation Info;
	if (FindTemporalEventInfoById(EventId, Info))
	{
		return GetEventElapsedTime(Info.Event);
	}

	return 0.0f;
}

TArray<int32> UMessageBusSubsystem::FindTemporalEventIdsByKey(const FGameplayTag& BusKey, const FGameplayTag& Key)
{
	TArray<int32> Result;
	
	for (const auto& Elem : TemporalAndPersistentEvents)
	{
		if (Elem.GetKey() == Key && Elem.Event.EventBusKey == BusKey)
		{
			Result.Add(Elem.GetEventId());
		}
	}
	
	return Result;
}



void UMessageBusSubsystem::RemoveInvalidNetConnections()
{
	const int Removed = RegisteredConnections.RemoveAll([](UMessageBusNetComponent* Item)
	{
		return !IsValid(Item);
	});
	
	if (Removed > 0)
	{
		UE_LOG(LogMessageBus, Log, TEXT("Removed %i invalid net connections"), Removed);
	}
}

void UMessageBusSubsystem::RemoveInvalidListeners()
{
	const int Removed = RegisteredListeners.RemoveAllSwap([](UMessageListenerBase* Item)
	{
		return !IsValid(Item);
	}, true);
	
	if (Removed > 0)
	{
		UE_LOG(LogMessageBus, Log, TEXT("Removed %i invalid listeners"), Removed);
	}
}

void UMessageBusSubsystem::OnConnectionAdded(UMessageBusNetComponent* NewConnection)
{
	UE_LOG(LogMessageBus, Log, TEXT("New connection added %s, events to push %i"), *GetNameSafe(NewConnection), TemporalAndPersistentEvents.Num());

	//Push actual events into connection
	if ((GetWorld()->GetNetMode() != NM_Client) || GetWorld()->GetNetMode() == NM_Standalone)
	{
		for (auto EventIt = TemporalAndPersistentEvents.CreateConstIterator(); EventIt; ++EventIt)
		{
			//Push only net multicast, specific player connection can't exist for unregistered player
			if (EventIt->RoutingInfo.ReplicationRule == EMessageReplicationRule::NetMulticast)
			{
				NewConnection->PushNewEvent(*EventIt);
			}
		}
	}
}

void UMessageBusSubsystem::OnConnectionRemoved(UMessageBusNetComponent* NewConnection)
{
	
}

void UMessageBusSubsystem::OnIncomeNewEvent(const FMessageBusEvent& Event)
{
	RemoveInvalidListeners();
	UE_LOG(LogMessageBus, Verbose, TEXT("Income event new %s:%i, active listeners %i"), *Event.EventKey.ToString(), Event.GetEventId(), RegisteredListeners.Num());
	int ReactionsNum = 0;
	TArray<UMessageListenerBase*> ToRemove;

	//Iterate per listeners for event reactions
	for (auto ListenerIt = RegisteredListeners.CreateIterator(); ListenerIt; ++ListenerIt)
	{
		UMessageListenerBase* CurrentListener = (*ListenerIt);
		bool bShouldDeactivateAfterCall = false;

		if (CurrentListener->ShouldReactOnEvent(Event))
		{
			CurrentListener->ReceiveMessageBusEvent(Event, EEventAction::Appear);
			
			UE_LOG(LogMessageBus, Verbose, TEXT("Listener %s reacted on event %s"), *GetNameSafe(CurrentListener), *Event.EventKey.ToString());
			
			ReactionsNum++;
			if (CurrentListener->IsFireOnce())
			{
				bShouldDeactivateAfterCall = CurrentListener->IsFireOnce();
			}
		}
		else
		{
			UE_LOG(LogMessageBus, VeryVerbose, TEXT("Listener %s skip reaction for %s"), *GetNameSafe(CurrentListener), *Event.EventKey.ToString());
		}
		
		if (bShouldDeactivateAfterCall)
		{
			ToRemove.Add(CurrentListener);
		}
	}

	UE_LOG(LogMessageBus, Display, TEXT("Finish event routing %s, reacted %i/%i listeners"), *Event.EventKey.ToString(), ReactionsNum, RegisteredListeners.Num());


	for (UMessageListenerBase* Listener : ToRemove)
	{
		Listener->Deactivate();
	}
}

void UMessageBusSubsystem::OnEventCanceled(const FMessageBusEvent& Event)
{
	RemoveInvalidListeners();
	UE_LOG(LogMessageBus, Verbose, TEXT("Canceled event %s, active listeners %i"), *Event.EventKey.ToString(), RegisteredListeners.Num());
	
	
	//Filter only listeners for cancel events
	TArray<UMessageListenerBase*> ListenForCancel = RegisteredListeners.FilterByPredicate([](UMessageListenerBase* Listener)
	{
		return Listener->IsListenForCancelEvents();
	});
	
	int ReactionsNum = 0;
	
	//Iterate per listeners for event reactions
	for (auto ListenerIt = ListenForCancel.CreateIterator(); ListenerIt; ++ListenerIt)
	{
		UMessageListenerBase* CurrentListener = (*ListenerIt);
		
		if (CurrentListener->ShouldReactOnEvent(Event))
		{
			CurrentListener->ReceiveMessageBusEvent(Event, EEventAction::Cancel);
			
			UE_LOG(LogMessageBus, Verbose, TEXT("Listener %s reacted on event %s"), *GetNameSafe(CurrentListener), *Event.EventKey.ToString());
			
			ReactionsNum++;
		}
		else
		{
			UE_LOG(LogMessageBus, VeryVerbose, TEXT("Listener %s skip reaction for %s"), *GetNameSafe(CurrentListener), *Event.EventKey.ToString());
		}
	}
	
	UE_LOG(LogMessageBus, Display, TEXT("Finish cancel event routing %s, reacted %i/%i listeners"), *Event.EventKey.ToString(), ReactionsNum, RegisteredListeners.Num());
}

void UMessageBusSubsystem::OnIncomeNewEventFromReplication(UMessageBusNetComponent* Connection, const FMessagePayloadEventFullInformation& Event)
{
	UE_LOG(LogMessageBus, Verbose, TEXT("Event %s:%i replicated to client, active listeners %i"), *Event.GetKey().ToString(), Event.GetEventId(), RegisteredListeners.Num());

	checkf(RegisteredConnections.Contains(Connection), TEXT("Income new event %s from unregistered connection %s"), *Event.GetKey().ToString(), *GetNameSafe(Connection));

	INC_DWORD_STAT(STAT_EventsReplicated);
	INC_MEMORY_STAT_BY(STAT_EventsReplicatedMemory,Event.StaticStruct()->GetStructureSize());
	INC_MEMORY_STAT_BY(STAT_EventsReplicatedMemory,GetPayloadSize(Event));

	
	//Register event from replication
	if (!AllEvents.Contains(Event.Event.MessageId))
	{
		AllEvents.Emplace(Event.Event.MessageId, Event);
		
		SET_DWORD_STAT(STAT_EventsSent,AllEvents.Num());
	}

	if (Event.Event.LifeType != EMessageLifeType::Instant)
	{
		RegisterTemporalEvent(Event);
	}

	OnIncomeNewEvent(Event.Event);
}


int UMessageBusSubsystem::ReplicateEventForAllClients(const FMessagePayloadEventFullInformation& Event)
{
	int Sends = 0;
	for (auto ConnectionIt = RegisteredConnections.CreateIterator(); ConnectionIt; ++ConnectionIt)
	{
		(*ConnectionIt)->PushNewEvent(Event);
		Sends++;
		
		INC_DWORD_STAT(STAT_EventsReplicated);
		INC_MEMORY_STAT_BY(STAT_EventsReplicatedMemory,Event.StaticStruct()->GetStructureSize());
		INC_MEMORY_STAT_BY(STAT_EventsReplicatedMemory,GetPayloadSize(Event));
	}
	
	return Sends;
}

void UMessageBusSubsystem::ReplicateEventForSpecificClient(const FMessagePayloadEventFullInformation& Event)
{
	APlayerState* ClientPlayerState = Event.RoutingInfo.SpecificClient.Get();
	checkf(ClientPlayerState, TEXT("ReplicateEventForSpecificClient fail, client actor is invalid"));
	
	APlayerController* ClientController = Cast<APlayerController>(ClientPlayerState->GetOwner());
	checkf(ClientController, TEXT("ReplicateEventForSpecificClient fail, player controller is invalid"));
	
	UMessageBusNetComponent** ClientConnection = RegisteredConnections.FindByPredicate([ClientController](UMessageBusNetComponent* Item)
		{
			return Item->GetOwner() == ClientController;
		});
	checkf(ClientConnection, TEXT("ReplicateEventForSpecificClient fail, connot find connection for player controller"));

	(*ClientConnection)->PushNewEvent(Event);
	
	INC_DWORD_STAT(STAT_EventsReplicated);
	
	INC_MEMORY_STAT_BY(STAT_EventsReplicatedMemory,Event.StaticStruct()->GetStructureSize());
	INC_MEMORY_STAT_BY(STAT_EventsReplicatedMemory,GetPayloadSize(Event));
}

float UMessageBusSubsystem::GetServerWorldTime() const
{
	return ActualGameState.IsValid() ? ActualGameState->GetServerWorldTimeSeconds() : 0.f;
}


void UMessageBusSubsystem::SubsystemTick(float DeltaTime)
{
	//skip if game state is not valid
	if (!ActualGameState.IsValid()) return;
	
	
	//filer all expired temporal events, server can expire all event, client only NoReplicated events
	TArray<FMessagePayloadEventFullInformation> ExpiredEvents = TemporalAndPersistentEvents.FilterByPredicate([this](const FMessagePayloadEventFullInformation& Item)
		{
			return Item.Event.LifeType == EMessageLifeType::Temporal && GetEventRemainingTime(Item.Event) <= 0.f && ((GetWorld()->GetNetMode() != NM_Client) || !Item.IsReplicated());
		});

	for (auto It = ExpiredEvents.CreateConstIterator(); It; ++It)
	{
		CancelTemporalEvent(It->GetEventId(), *It);
	}
	//try cancel temporal events
}

void UMessageBusSubsystem::OnGameStateRecieved(AGameStateBase* GameState)
{
	ActualGameState = GameState;
}

void UMessageBusSubsystem::RegisterTemporalEvent(const FMessagePayloadEventFullInformation& EventInformation)
{
	TemporalAndPersistentEvents.Emplace(EventInformation);
	SET_DWORD_STAT(STAT_TemporalEvents,TemporalAndPersistentEvents.Num());
	INC_MEMORY_STAT_BY(STAT_TempEventsMemory,GetEventSizeWithPayload(EventInformation));
}

void UMessageBusSubsystem::UnregisterTemporalEvent(
	const FMessagePayloadEventFullInformation& EventInformation)
{
	TemporalAndPersistentEvents.RemoveAllSwap([EventId = EventInformation.GetEventId()]
	(const FMessagePayloadEventFullInformation& Item)
	{
		return Item.GetEventId() == EventId;
	});
	SET_DWORD_STAT(STAT_TemporalEvents,TemporalAndPersistentEvents.Num());
	DEC_MEMORY_STAT_BY(STAT_TempEventsMemory,GetEventSizeWithPayload(EventInformation));
}

int32 UMessageBusSubsystem::GetEventIdShift() const
{
	constexpr int32 ClientEventIdShift = 1000000000;
	const bool bIsClient = GetWorld()->GetNetMode() == NM_Client;
	return bIsClient ? ClientEventIdShift : 0;
}

bool UMessageBusSubsystem::FindTemporalEventInfoById(int32 MessageId, FMessagePayloadEventFullInformation& OutInfo) const
{
	if(const FMessagePayloadEventFullInformation* EventToCancel = TemporalAndPersistentEvents.FindByPredicate([MessageId](const FMessagePayloadEventFullInformation& Item)
		{
			return Item.GetEventId() == MessageId;
		}))
	{
		OutInfo = *EventToCancel;
		return true;
	}
	return false;
}
