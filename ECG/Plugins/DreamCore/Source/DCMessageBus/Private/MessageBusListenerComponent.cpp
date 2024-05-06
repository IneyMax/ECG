// Copyright DreamVR. All Rights Reserved.


#include "MessageBusListenerComponent.h"
#include "MessageBusSubsystem.h"

UMessageListenerBase::UMessageListenerBase()
{
	bShouldBeDestroyOnDeactivate = false;
}

void UMessageListenerBase::BeginDestroy()
{
	if (IsActive())
	{
		Deactivate();
	}

	if (UMessageBusListenerComponent* Owner = GetOwningComponent())
	{
		Owner->RemoveOwnedListener(this);
	}

	Super::BeginDestroy();
}

bool UMessageListenerBase::ShouldReactOnEvent_Implementation(const FMessageBusEvent& Event) const
{
	const bool bBusKeyMatch = !BindingInfo.EventBusKey.IsValid() || Event.EventBusKey == BindingInfo.EventBusKey;
	const bool bEventKeyMatch = !BindingInfo.EventKey.IsValid() || Event.EventKey == BindingInfo.EventKey;
	const bool bInstigatorMatch = !BindingInfo.FromInstigator.IsValid() || Event.Instigator == BindingInfo.FromInstigator.Get();

	return HasValidBindingInfo() && bBusKeyMatch && bEventKeyMatch && bInstigatorMatch;
}

void UMessageListenerBase::ReceiveMessageBusEvent_Implementation(const FMessageBusEvent& Event, EEventAction Action)
{

}

UMessageListenerCallback* UMessageListenerCallback::CreateMessageBusDynamicCallback(UObject* Outer, FEventBindingInfo const& Info, FMessageBusEventSignatureDynamic DynamicCallback)
{
	UMessageListenerCallback* NewListener = NewObject<UMessageListenerCallback>(Outer, NAME_None, RF_Public | RF_Transient);
	
	NewListener->InternalCallbackDynamic = DynamicCallback;
	NewListener->InitializeListener(Info);

	return NewListener;
}

UMessageListenerCallback* UMessageListenerCallback::CreateMessageBusCallback(UObject* Outer, FEventBindingInfo const& Info, FMessageBusEventSignature Callback)
{
	UMessageListenerCallback* NewListener = NewObject<UMessageListenerCallback>(Outer, NAME_None, RF_Public | RF_Transient);
	
	NewListener->InternalCallback = Callback;
	NewListener->InitializeListener(Info);

	return NewListener;
}

void UMessageListenerCallback::ReceiveMessageBusEvent_Implementation(const FMessageBusEvent& Event, EEventAction Action)
{
	InternalCallback.ExecuteIfBound(Event, Action);
	InternalCallbackDynamic.ExecuteIfBound(Event, Action);
}

void UMessageListenerCallback::BeginDestroy()
{
	Super::BeginDestroy();

	InternalCallback.Unbind();
	InternalCallbackDynamic.Unbind();
}

void UMessageListenerBase::SelfInitialize()
{
	InitializeListener(BindingInfo);
}

void UMessageListenerBase::InitializeListener(FEventBindingInfo const& Info)
{
	BindingInfo = Info;
	checkf(HasValidBindingInfo(), TEXT("Message listener %s has no valid binding info after initialize"), *GetName());

	if (UMessageBusListenerComponent* Owner = GetOwningComponent())
	{
		Owner->AddOwnedListener(this);
	}

	OnInitializedListener();

	if (BindingInfo.bStartActive)
	{
		Activate();
	}
}

void UMessageListenerBase::OnInitializedListener_Implementation()
{

}

void UMessageListenerBase::Activate()
{
	if (bIsActive || !GetWorld())
	{
		return;
	}

	if (UMessageBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UMessageBusSubsystem>())
	{
		Subsystem->RegisterListener(this);
		bIsActive = true;
	}
}

void UMessageListenerBase::Deactivate()
{
	if (!bIsActive || !GetWorld())
	{
		return;
	}

	if (UMessageBusSubsystem* Subsystem = GetWorld()->GetSubsystem<UMessageBusSubsystem>())
	{
		Subsystem->UnregisterListener(this);
		bIsActive = false;
	}

	if (bShouldBeDestroyOnDeactivate)
	{
		MarkAsGarbage();
	}
}

void UMessageListenerBase::SetShouldBeDestroyAfterDeactivation(bool bValue)
{
	bShouldBeDestroyOnDeactivate = bValue;
}


AActor* UMessageListenerBase::GetActorOwner() const
{
	return GetOwningComponent() ? GetOwningComponent()->GetOwner() : nullptr;
}

UMessageBusListenerComponent* UMessageListenerBase::GetOwningComponent() const
{
	return Cast<UMessageBusListenerComponent>(GetOuter());
}

// Sets default values for this component's properties
UMessageBusListenerComponent::UMessageBusListenerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UMessageBusListenerComponent::OnRegister()
{
	Super::OnRegister();
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		//change outer to self and initialize
		for (UMessageListenerBase* DefaultListener : DefaultListeners)
		{
			if (DefaultListener)
			{
				DefaultListener->Rename(nullptr, this);
				DefaultListener->SelfInitialize();
			}
		}
	}
}

void UMessageBusListenerComponent::OnUnregister()
{
	Super::OnUnregister();

	//change outer to self and initialize
	for (UMessageListenerBase* Listener : OwnedListeners)
	{
		Listener->Deactivate();
	}

	OwnedListeners.Empty();
}

UMessageListenerBase* UMessageBusListenerComponent::ConstructOwnedListener(TSubclassOf<UMessageListenerBase> InClass, bool bForceActive)
{
	UMessageListenerBase* NewListener = NewObject<UMessageListenerCallback>(this, InClass, NAME_None, RF_Public | RF_Transient);
	NewListener->SelfInitialize();



	if (bForceActive && !NewListener->IsActive())
	{
		NewListener->Activate();
	}

	checkf(HasOwnedListener(NewListener), TEXT("ConstructOwnedListener FAIL %s, %s"), *GetName(), *GetNameSafe(NewListener));

	return NewListener;
}

UMessageListenerBase* UMessageBusListenerComponent::ConstructOwnedListenerWithBinding(TSubclassOf<UMessageListenerBase> InClass, FEventBindingInfo const& BindingInfo)
{
	UMessageListenerBase* NewListener = NewObject<UMessageListenerCallback>(this, InClass, NAME_None, RF_Public | RF_Transient);
	NewListener->InitializeListener(BindingInfo);

	checkf(HasOwnedListener(NewListener), TEXT("ConstructOwnedListener FAIL %s, %s"), *GetName(), *GetNameSafe(NewListener));

	return NewListener;
}

void UMessageBusListenerComponent::AddOwnedListener(UMessageListenerBase* InListener)
{
	if (IsValid(InListener) && !HasOwnedListener(InListener))
	{
		OwnedListeners.Add(InListener);
	}
}

void UMessageBusListenerComponent::RemoveOwnedListener(UMessageListenerBase* InListener)
{
	if (IsValid(InListener) && HasOwnedListener(InListener))
	{
		OwnedListeners.Remove(InListener);
	}
}

bool UMessageBusListenerComponent::HasOwnedListener(UMessageListenerBase* ListenerToCheck) const
{
	return OwnedListeners.Contains(ListenerToCheck);
}

UMessageListenerCallbackDelayed* UMessageListenerCallbackDelayed::CreateMessageBusDynamicCallbackWithDelay(UObject* Outer, FEventBindingInfo const& Info, float Delay, FMessageBusEventSignatureDynamic DynamicCallback)
{
	UMessageListenerCallbackDelayed* NewListener = NewObject<UMessageListenerCallbackDelayed>(Outer, NAME_None, RF_Public | RF_Transient);

	NewListener->InternalCallbackDynamic = DynamicCallback;
	NewListener->Delay = Delay;
	NewListener->InitializeListener(Info);
	NewListener->SetShouldBeDestroyAfterDeactivation(false);

	return NewListener;
}

UMessageListenerCallbackDelayed* UMessageListenerCallbackDelayed::CreateMessageBusCallbackWithDelay(UObject* Outer, FEventBindingInfo const& Info, float Delay, FMessageBusEventSignature Callback)
{
	UMessageListenerCallbackDelayed* NewListener = NewObject<UMessageListenerCallbackDelayed>(Outer, NAME_None, RF_Public | RF_Transient);

	check(NewListener->GetWorld());

	NewListener->InternalCallback = Callback;
	NewListener->Delay = Delay;
	NewListener->InitializeListener(Info);
	NewListener->SetShouldBeDestroyAfterDeactivation(false);


	return NewListener;
}

void UMessageListenerCallbackDelayed::ReceiveMessageBusEvent_Implementation(const FMessageBusEvent& Event, EEventAction Action)
{
	GetWorld()->GetTimerManager().SetTimer(DelayedCallback, FTimerDelegate::CreateUObject(this, &UMessageListenerCallbackDelayed::OnReadyToFire, Event, Action), Delay, false);
}

void UMessageListenerCallbackDelayed::BeginDestroy()
{
	if (DelayedCallback.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DelayedCallback);
	}
	Super::BeginDestroy();
}

void UMessageListenerCallbackDelayed::OnReadyToFire(const FMessageBusEvent Event, EEventAction Action)
{
	InternalCallback.ExecuteIfBound(Event, Action);
	InternalCallbackDynamic.ExecuteIfBound(Event, Action);
}
