// Copyright DreamVR. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MessageBusTypes.h"
#include "GameFramework/PlayerState.h"

#include "MessageBusSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMessageBus, Log, All);

DECLARE_STATS_GROUP(TEXT("MessageBus"), STATGROUP_MessageBus, STATCAT_Advanced);


class UMessageBusNetComponent;
class UMessageBusSubsystem;
class UMessageListenerBase;


struct FMessageBusSubsystemTick : public FTickableGameObject
{
	FMessageBusSubsystemTick(UMessageBusSubsystem* InSubsystem)
	{
		Subsystem = InSubsystem;
	}
	/** FTickableGameObject interface */
	virtual void Tick(float DeltaTime);
	virtual ETickableTickType GetTickableTickType() const { return ETickableTickType::Always; }
	virtual TStatId GetStatId() const override;

	TWeakObjectPtr<UMessageBusSubsystem> Subsystem;
};

DECLARE_MULTICAST_DELEGATE(FMessageBusInitialized);
DECLARE_MULTICAST_DELEGATE(FMessageBusDeinitialized);

/**
 * 
 */
UCLASS()
class DCMESSAGEBUS_API UMessageBusSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	friend class UMessageBusNetComponent;
	friend struct FMessageBusSubsystemTick;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	static int GetStructIndex(UScriptStruct* Struct);
	static UScriptStruct* GetStructByIndex(int Index);

	static FMessageBusInitialized& GetMessageBusInitialized();
	static FMessageBusDeinitialized& GetMessageBusDeinitialized();

	static int GetPayloadSize(const FMessagePayloadEventFullInformation& Event);
	static int GetEventSizeWithPayload(const FMessagePayloadEventFullInformation& Event);

public:
	static UMessageBusSubsystem* Get(const UObject* WorldContext);

	void RegisterNetConnection(UMessageBusNetComponent* InConnection);
	void UnregisterNetConnection(UMessageBusNetComponent* InConnection);

	void RegisterListener(UMessageListenerBase* InListener);
	void UnregisterListener(UMessageListenerBase* InListener);

public:

	UFUNCTION(BlueprintCallable)
	int32 BroadcastEvent(FMinimalEventInfo const& MinimalInfo, EMessageReplicationRule Replication = EMessageReplicationRule::NoReplicate, APlayerState* ClientPlayerState = nullptr)
	{
		FMessagePayloadEventFullInformation& Event = BuildEventWithoutPayload(MinimalInfo);
		Event.RoutingInfo.ReplicationRule = Replication;
		if (Replication == EMessageReplicationRule::ForSpecificClient)
		{
			Event.RoutingInfo.SpecificClient = ClientPlayerState;
		}

		return BroadcastEvent_Internal(Event);
	}

	template<typename PayloadType>
	int32 BroadcastEvent_WithPayload(FMinimalEventInfo const& MinimalInfo, PayloadType Payload, EMessageReplicationRule Replication = EMessageReplicationRule::NoReplicate, APlayerState* ClientPlayerState = nullptr)
	{
		FMessagePayloadEventFullInformation& Event = BuildEventWithPayload(MinimalInfo, Payload);
		Event.RoutingInfo.ReplicationRule = Replication;
		if (Replication == EMessageReplicationRule::ForSpecificClient)
		{
			Event.RoutingInfo.SpecificClient = ClientPlayerState;
		}
		return BroadcastEvent_Internal(Event);
	}


	void CancelEvent(int32 EventId);

	UFUNCTION(BlueprintCallable)
	int CancelAllMatchedEvents(const FGameplayTag& BusKey, const FGameplayTag& Key);
	
	float GetServerWorldTime() const;
	bool FindTemporalEventInfoById(int32 MessageId, FMessagePayloadEventFullInformation& OutInfo) const;
	
	float GetEventRemainingTime(const FMessageBusEvent& InEvent) const;
	float GetEventRemainingTime(int32 EventId) const;

	float GetEventElapsedTime(const FMessageBusEvent& InEvent) const;
	float GetEventElapsedTime(int32 EventId) const;
	TArray<int32> FindTemporalEventIdsByKey(const FGameplayTag& BusKey, const FGameplayTag& Key);
protected:
	template<typename PayloadType>
	FMessagePayloadEventFullInformation& BuildEventWithPayload(FMinimalEventInfo const& MinimalInfo, PayloadType Payload)
	{
		FMessageBusEvent NewEvent = FMessageBusEvent(MinimalInfo);
		TryAttachPayloadToEvent(NewEvent, Payload);

		return BuildEventFullInformation(NewEvent);
	}

	FMessagePayloadEventFullInformation& BuildEventWithoutPayload(FMinimalEventInfo const& MinimalInfo)
	{
		FMessageBusEvent NewEvent = FMessageBusEvent(MinimalInfo);
		return BuildEventFullInformation(NewEvent);
	}

	template<typename PayloadType>
	void TryAttachPayloadToEvent(FMessageBusEvent& Event, PayloadType Payload)
	{
		checkf(PayloadType::StaticStruct()->IsChildOf(FMessagePayloadBase::StaticStruct()), TEXT("Try attach invalid payload to event, payload type %s"), *GetNameSafe(PayloadType::StaticStruct()));


		Event.PayloadContainer.Payload = MakeShared<PayloadType>(MoveTemp(Payload));

	}

	FMessagePayloadEventFullInformation& BuildEventFullInformation(FMessageBusEvent const& Event);

	int32 BroadcastEvent_Internal(const FMessagePayloadEventFullInformation& EventInformation);
	void CancelTemporalEvent_Internal(const FMessagePayloadEventFullInformation& EventInformation);

protected:

	void PopulateAllMessageBusTypes();
	void RemoveInvalidNetConnections();
	void RemoveInvalidListeners();
	void OnConnectionAdded(UMessageBusNetComponent* NewConnection);
	void OnConnectionRemoved(UMessageBusNetComponent* NewConnection);

	virtual void OnIncomeNewEvent(const FMessageBusEvent& Event);
	virtual void OnEventCanceled(const FMessageBusEvent& Event);


	void OnIncomeNewEventFromReplication(UMessageBusNetComponent* Connection, const FMessagePayloadEventFullInformation& Event);
	void OnEventCanceledFromReplication(UMessageBusNetComponent* Connection, int32 EventId);

	int ReplicateEventForAllClients(const FMessagePayloadEventFullInformation& Event);
	void ReplicateEventForSpecificClient(const FMessagePayloadEventFullInformation& Event);

	virtual void SubsystemTick(float DeltaTime);
	void OnGameStateRecieved(AGameStateBase* GameState);

private:
	void RegisterTemporalEvent(const FMessagePayloadEventFullInformation& EventInformation);
	void UnregisterTemporalEvent(const FMessagePayloadEventFullInformation& EventInformation);

	void CancelTemporalEvent(int32 EventId, const FMessagePayloadEventFullInformation& EventToCancel);

	// TODO Critical: we have mixed ID's of local events and events from the server: the server and the client can generate different events with the same ID. A ID redirector or something like that should be implemented
	int32 GetEventIdShift() const;
	
	//bool ExecuteEventsForListener(UMessageListenerBase*)
private:

	TSharedPtr<FMessageBusSubsystemTick> SubsystemTickableObject;
	TWeakObjectPtr<UWorld> ActualGameWorld;

	//need for call GetServerTime()
	TWeakObjectPtr<AGameStateBase> ActualGameState;

	friend class FDCMessageBusModule;
	static TArray<UScriptStruct*> AllMessageBusTypes;

	UPROPERTY(Transient, VisibleAnywhere)
	int32 CurrentMessageId;

	TMap<int32, FMessagePayloadEventFullInformation> AllEvents;

	TArray<FMessagePayloadEventFullInformation> TemporalAndPersistentEvents;

	UPROPERTY(Transient, VisibleAnywhere)
	TArray<UMessageBusNetComponent*> RegisteredConnections;
	
	UPROPERTY(Transient, VisibleAnywhere)
	TArray<UMessageListenerBase*> RegisteredListeners;
};
