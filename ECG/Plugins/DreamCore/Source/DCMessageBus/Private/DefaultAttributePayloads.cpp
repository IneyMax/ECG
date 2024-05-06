// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultAttributePayloads.h"

#include "Blueprint/BlueprintExceptionInfo.h"

bool FDirectionPayload::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	return Direction.NetSerialize(Ar, PackageMap, bOutSuccess);
}

bool UDefaultPayloadsLibrary::GetPayloadFromPayloadContainer(FMessagePayloadContainer const& Container,
	FMessagePayloadBase& Result)
{
	// We should never hit this!  stubs to avoid NoExport on the class.
	check(0);
	return false;
}

bool FHitPayload::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	bool bLocalSuccess = true;

	Direction.NetSerialize(Ar, PackageMap, bLocalSuccess);
	bOutSuccess &= bLocalSuccess;

	HitPoint.NetSerialize(Ar, PackageMap, bLocalSuccess);
	bOutSuccess &= bLocalSuccess;

	return true;
}

bool FVectorPayload::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	bool bLocalSuccess = true;

	Vector.NetSerialize(Ar, PackageMap, bLocalSuccess);
	bOutSuccess &= bLocalSuccess;

	return true;
}

bool UDefaultPayloadsLibrary::Generic_GetPayloadFromPayloadContainer(FMessagePayloadContainer const& Container, void* Result)
{
	bool bSuccess = false;

	if (Container.Payload)
	{
		const UScriptStruct* StructType = Container.Payload->GetPayloadType();

		if (StructType != nullptr)
		{
			StructType->CopyScriptStruct(Result, Container.Payload.Get());
			bSuccess = true;
		}
	}

	return bSuccess;
}

DEFINE_FUNCTION(UDefaultPayloadsLibrary::execGetPayloadFromPayloadContainer)
{
	P_GET_STRUCT_REF(FMessagePayloadContainer, Container);
        
	Stack.StepCompiledIn<FStructProperty>(NULL);
	void* Result = Stack.MostRecentPropertyAddress;

	P_FINISH;
	bool bSuccess = false;
		
	FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	if (!Container.Payload)
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetPayloadType", "MissingPayload", "Failed to resolve. Be sure the Payload is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if(StructProp && Result)
	{
		UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* PayloadType  = Container.Payload->GetPayloadType();
		
		const bool bCompatible = (OutputType == PayloadType) || 
			(OutputType->IsChildOf(PayloadType) && FStructUtils::TheSameLayout(OutputType, PayloadType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
			bSuccess = Generic_GetPayloadFromPayloadContainer(Container, Result);
			P_NATIVE_END;
		}
		else
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("GetPayloadType", "IncompatibleProperty", "Incompatible output parameter; the payloads's type is not the same as the return type.")
				);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetPayloadType", "MissingOutputProperty", "Failed to resolve the output parameter for GetPayloadFromPayloadContainer.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	*(bool*)RESULT_PARAM = bSuccess;
}