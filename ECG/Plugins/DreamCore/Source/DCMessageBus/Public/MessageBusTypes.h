// Copyright DreamVR. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "MessageBusTypes.generated.h"
/**
 * 
 */

class APlayerState;

UENUM()
enum class EMessageLifeType : uint8
{
	Any,
	Instant,
	Temporal,
	Persistent
};

UENUM()
enum class EMessageReplicationRule : uint8
{
	NoReplicate,
	NetMulticast,
	ForSpecificClient
};

UENUM(BlueprintType)
enum class EEventAction : uint8
{
	Appear,
	Cancel
};

USTRUCT(Blueprintable, Atomic)
struct DCMESSAGEBUS_API FMessageRoutingInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EMessageReplicationRule ReplicationRule = EMessageReplicationRule::NoReplicate;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TWeakObjectPtr<APlayerState> SpecificClient = nullptr;
};

USTRUCT(Blueprintable)
struct DCMESSAGEBUS_API FMessagePayloadBase
{
	GENERATED_BODY()
public:
	virtual ~FMessagePayloadBase() {};
	virtual UScriptStruct* GetPayloadType() const { check(false); return nullptr; };
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess) { return true; }
};

#define IMPLEMENT_PAYLOAD_TYPE(Type) virtual UScriptStruct* GetPayloadType() const override { return Type::StaticStruct(); };

template<>
struct TStructOpsTypeTraits<FMessagePayloadBase> : public TStructOpsTypeTraitsBase2<FMessagePayloadBase>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(Atomic, BlueprintType)
struct DCMESSAGEBUS_API FMessagePayloadContainer
{
	GENERATED_BODY()
public:
	TSharedPtr<FMessagePayloadBase> Payload;

	template<typename PayloadT>
	PayloadT GetPayload() const
	{
		TSharedPtr<PayloadT> CastedType = StaticCastSharedPtr<PayloadT>(Payload);
		if (CastedType.IsValid())
		{
			return *CastedType;
		}
		else
		{
			return PayloadT();
		}
	}

	int GetPayloadSize() const
	{
		if(Payload.Get())
		{
			return Payload.Get()->GetPayloadType()->GetStructureSize();
		}
		return 0;
	}

	bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess);
};
template<>
struct TStructOpsTypeTraits<FMessagePayloadContainer> : public TStructOpsTypeTraitsBase2<FMessagePayloadContainer>
{
	enum
	{
		WithNetSerializer = true,
	};
};


USTRUCT(Blueprintable)
struct DCMESSAGEBUS_API FMinimalEventInfo
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag EventKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag EventBusKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EMessageLifeType LifeType = EMessageLifeType::Instant;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "LifeType == EMessageLifeType::Temporal", EditConditionHides))
	float LifeTime = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TWeakObjectPtr<AActor> Instigator;

	bool IsValid() const
	{
		return EventKey.IsValid();
	}
};

USTRUCT(Blueprintable)
struct DCMESSAGEBUS_API FEventBindingInfo
{
	GENERATED_BODY()
public:

	FEventBindingInfo()
	{
		EventKey = FGameplayTag::EmptyTag;
		EventBusKey = FGameplayTag::EmptyTag;
		FromInstigator = nullptr;
		bFireOnce = false;
		bStartActive = true;
		bOnlyAppear = false;
	}

	FEventBindingInfo(const FMinimalEventInfo& MinimalInfo, bool bInFireOnce = false)
	{
		EventKey = MinimalInfo.EventKey;
		EventBusKey = MinimalInfo.EventBusKey;
		FromInstigator = MinimalInfo.Instigator;
		bFireOnce = bInFireOnce;
		bStartActive = true;
		bOnlyAppear = false;
	}

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FGameplayTag EventKey;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FGameplayTag EventBusKey;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TWeakObjectPtr<AActor> FromInstigator;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bFireOnce;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bStartActive;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bOnlyAppear;
};



USTRUCT(Blueprintable)
struct DCMESSAGEBUS_API FMessageBusEvent
{
	GENERATED_BODY()
public:
	FMessageBusEvent()
	{
		EventKey = FGameplayTag::EmptyTag;
		EventBusKey = FGameplayTag::EmptyTag;
		LifeType = EMessageLifeType::Any;
		LifeTime = -1.f;
		Instigator = nullptr;
		MessageId = -1;
		EventTime = -1.f;
		PayloadContainer.Payload = nullptr;
	}

	FMessageBusEvent(FMinimalEventInfo const& MinimalInfo)
	{
		EventKey = MinimalInfo.EventKey;
		EventBusKey = MinimalInfo.EventBusKey;
		LifeType = MinimalInfo.LifeType;
		if (LifeType == EMessageLifeType::Temporal)
		{
			LifeTime = MinimalInfo.LifeTime;
		}
		else
		{
			LifeTime = 0.f;
		}
		Instigator = MinimalInfo.Instigator;
	}
public:


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FGameplayTag EventKey;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FGameplayTag EventBusKey;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		EMessageLifeType LifeType;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		float LifeTime;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TWeakObjectPtr<AActor> Instigator;

	float GetEventTime() const
	{
		return EventTime;
	}

	float GetLifeTime() const
	{
		return LifeTime;
	}

	int GetEventId() const
	{
		return MessageId;
	}

	bool IsValid() const
	{
		return EventKey.IsValid();
	}

	template<typename PayloadT>
	PayloadT GetPayload() const
	{
		return PayloadContainer.GetPayload<PayloadT>();
	}

	static FMessageBusEvent EmptyEvent;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int MessageId;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float EventTime;

	friend class UMessageBusSubsystem;
public:

	bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess);

public:

	UPROPERTY(BlueprintReadOnly)
	FMessagePayloadContainer PayloadContainer;
};

template<>
struct TStructOpsTypeTraits<FMessageBusEvent> : public TStructOpsTypeTraitsBase2<FMessageBusEvent>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(Blueprintable)
struct DCMESSAGEBUS_API FMessagePayloadEventFullInformation
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FMessageBusEvent Event;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FMessageRoutingInfo RoutingInfo;

public:

	float GetEventTime() const
	{
		return Event.GetEventTime();
	}

	int32 GetEventId() const
	{
		return Event.GetEventId();
	}

	FGameplayTag GetKey() const
	{
		return Event.EventKey;
	}

	bool IsReplicated() const
	{
		return RoutingInfo.ReplicationRule != EMessageReplicationRule::NoReplicate;
	}
};