#include "ECG/Public/TestingTemp/TestSubsystem.h"
#include "ThirdParty/EnTT/entt/src/entt/entt.hpp"

void UTestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	entt::registry registry;
}

void UTestSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UTestSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TStatId UTestSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTestSubsystem, STATGROUP_Tickables);
}
