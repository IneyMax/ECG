// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MessageBusTypes.h"
#include "DefaultAttributePayloads.generated.h"

USTRUCT(BlueprintType)
struct DCMESSAGEBUS_API FDirectionPayload : public FMessagePayloadBase
{
	GENERATED_BODY()
public:
	IMPLEMENT_PAYLOAD_TYPE(FDirectionPayload);

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess) override;
public:

	UPROPERTY(BlueprintReadOnly)
	FVector_NetQuantizeNormal Direction;

	UPROPERTY(BlueprintReadOnly)
	bool bOverrideXY = false;

	UPROPERTY(BlueprintReadOnly)
	bool bOverrideZ = false;
};

USTRUCT(BlueprintType)
struct DCMESSAGEBUS_API FVectorPayload : public FMessagePayloadBase
{
	GENERATED_BODY()
public:
	IMPLEMENT_PAYLOAD_TYPE(FVectorPayload);

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess) override;
public:

	UPROPERTY(BlueprintReadOnly)
	FVector Vector = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct DCMESSAGEBUS_API FHitPayload : public FMessagePayloadBase
{
	GENERATED_BODY()
public:
	IMPLEMENT_PAYLOAD_TYPE(FHitPayload);

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess) override;
public:

	UPROPERTY(BlueprintReadOnly)
	FVector_NetQuantizeNormal Direction;

	UPROPERTY(BlueprintReadOnly)
	FVector_NetQuantize100 HitPoint;

	UPROPERTY(BlueprintReadOnly)
	FVector_NetQuantizeNormal ImpactNormal = FVector::ZeroVector;
};

UCLASS()
class DCMESSAGEBUS_API UDefaultPayloadsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	template <typename PayloadT>
	static PayloadT GetPayloadFromContainer(FMessagePayloadContainer const& Container)
	{
		TSharedPtr<PayloadT> CastedType = StaticCastSharedPtr<PayloadT>(Container.Payload);
		if (CastedType.IsValid())
		{
			return *CastedType;
		}
		else
		{
			return PayloadT();
		}
	}

	UFUNCTION(BlueprintCallable, CustomThunk, meta=(CustomStructureParam = "Result", BlueprintInternalUseOnly="true"))//meta=(AutoCreateRefTerm="Container"))
	static bool GetPayloadFromPayloadContainer(FMessagePayloadContainer const& Container, FMessagePayloadBase& Result);

	static bool Generic_GetPayloadFromPayloadContainer(FMessagePayloadContainer const& Container, void* Result);
	
	DECLARE_FUNCTION(execGetPayloadFromPayloadContainer);
};