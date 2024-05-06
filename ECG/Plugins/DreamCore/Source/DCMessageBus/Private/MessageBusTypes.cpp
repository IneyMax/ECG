// Copyright DreamVR. All Rights Reserved.


#include "MessageBusTypes.h"
#include "MessageBusSubsystem.h"

FMessageBusEvent FMessageBusEvent::EmptyEvent = FMessageBusEvent();

bool FMessageBusEvent::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	Ar << EventKey;
	Ar << EventBusKey;
	Ar << LifeType;
	if (LifeType == EMessageLifeType::Temporal)
	{
		Ar << LifeTime;
	}
	else if(Ar.IsLoading())
	{
		LifeTime = 0.f;
	}

	Ar << EventTime;
	Ar << Instigator;
	Ar << MessageId;

	return PayloadContainer.NetSerialize(Ar, PackageMap, bOutSuccess);
}

bool FMessagePayloadContainer::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	bool bHasPayload = Payload.IsValid();
	Ar << bHasPayload;

	if (bHasPayload)
	{
		int32 TypeIndex = 0;
		if (Ar.IsSaving())
		{
			UScriptStruct* PayloadType = Payload->GetPayloadType();
			TypeIndex = UMessageBusSubsystem::GetStructIndex(PayloadType);
			Ar << TypeIndex;

			return Payload->NetSerialize(Ar, PackageMap, bOutSuccess);
		}
		else
		{
			Ar << TypeIndex;
			const UScriptStruct* PayloadType = UMessageBusSubsystem::GetStructByIndex(TypeIndex);

			const int32 StructSize = PayloadType->GetCppStructOps()->GetSize();
			const int32 StructAlign = PayloadType->GetCppStructOps()->GetAlignment();

			//Allocate memory
			FMessagePayloadBase* StructMemory = (FMessagePayloadBase*)FMemory::Malloc(StructSize, StructAlign);

			//Call constructor
			PayloadType->GetCppStructOps()->Construct(StructMemory);

			Payload = MakeShareable(StructMemory);

			//Direct call net serrialize
			return Payload->NetSerialize(Ar, PackageMap, bOutSuccess);
		}
	}
	return true;
}
