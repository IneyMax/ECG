// Copyright DreamVR. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessageBusTypes.h"

#include "MessageBusListenerComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FMessageBusEventSignatureDynamic, const FMessageBusEvent&, EventData, EEventAction, Action);
DECLARE_DELEGATE_TwoParams(FMessageBusEventSignature, const FMessageBusEvent&, EEventAction);

class UMessageBusListenerComponent;

UCLASS(EditInlineNew, DefaultToInstanced, Abstract, ClassGroup = (Custom))
class DCMESSAGEBUS_API UMessageListenerBase : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMessageListenerBase();

	virtual void BeginDestroy() override;
public:

	void SelfInitialize();
	void InitializeListener(FEventBindingInfo const& Info);

	UFUNCTION(BlueprintCallable)
	void SetShouldBeDestroyAfterDeactivation(bool bValue);

public:

	UFUNCTION(BlueprintNativeEvent)
	void OnInitializedListener();

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveMessageBusEvent(const FMessageBusEvent& Event, EEventAction Action);

	virtual void Activate();
	virtual void Deactivate();

public:
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldReactOnEvent(const FMessageBusEvent& Event) const;

	bool IsActive() const;

	bool IsFireOnce() const;

	bool IsListenForCancelEvents() const;

	bool HasValidBindingInfo() const;

	UFUNCTION(BlueprintCallable)
	AActor* GetActorOwner() const;

	UFUNCTION(BlueprintCallable)
	UMessageBusListenerComponent* GetOwningComponent() const;

	FString GetDebugString() const
	{
		return BindingInfo.EventKey.ToString();
	}

protected:

	UPROPERTY(EditDefaultsOnly)
	FEventBindingInfo BindingInfo;
	
	UPROPERTY(VisibleAnywhere)
	bool bIsActive;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldBeDestroyOnDeactivate;
};


UCLASS(EditInlineNew, DefaultToInstanced, ClassGroup = (Custom))
class DCMESSAGEBUS_API UMessageListenerCallback : public UMessageListenerBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	static UMessageListenerCallback* CreateMessageBusDynamicCallback(UObject* Outer, FEventBindingInfo const& Info, FMessageBusEventSignatureDynamic DynamicCallback);

	static UMessageListenerCallback* CreateMessageBusCallback(UObject* Outer, FEventBindingInfo const& Info, FMessageBusEventSignature Callback);

public:

	virtual void ReceiveMessageBusEvent_Implementation(const FMessageBusEvent& Event, EEventAction Action) override;
	virtual void BeginDestroy() override;
protected:
	FMessageBusEventSignatureDynamic InternalCallbackDynamic;
	FMessageBusEventSignature InternalCallback;

};

UCLASS(EditInlineNew, DefaultToInstanced, ClassGroup = (Custom))
class DCMESSAGEBUS_API UMessageListenerCallbackDelayed : public UMessageListenerCallback
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	static UMessageListenerCallbackDelayed* CreateMessageBusDynamicCallbackWithDelay(UObject* Outer, FEventBindingInfo const& Info, float Delay, FMessageBusEventSignatureDynamic DynamicCallback);

	static UMessageListenerCallbackDelayed* CreateMessageBusCallbackWithDelay(UObject* Outer, FEventBindingInfo const& Info, float Delay, FMessageBusEventSignature Callback);

public:

	virtual void ReceiveMessageBusEvent_Implementation(const FMessageBusEvent& Event, EEventAction Action) override;
	virtual void BeginDestroy() override;
private:
	void OnReadyToFire(const FMessageBusEvent Event, EEventAction Action);
	FTimerHandle DelayedCallback;

	UPROPERTY(VisibleAnywhere)
	float Delay;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DCMESSAGEBUS_API UMessageBusListenerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMessageBusListenerComponent();

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	UMessageListenerBase* ConstructOwnedListener(TSubclassOf<UMessageListenerBase> InClass, bool bForceActive = false);
	UMessageListenerBase* ConstructOwnedListenerWithBinding(TSubclassOf<UMessageListenerBase> InClass, FEventBindingInfo const& BindingInfo);

	void AddOwnedListener(UMessageListenerBase* InListener);
	void RemoveOwnedListener(UMessageListenerBase* InListener);

	bool HasOwnedListener(UMessageListenerBase* ListenerToCheck) const;

	TArray<UMessageListenerBase*> GetDefaultListeners() const
	{
		return DefaultListeners;
	}

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Instanced)
	TArray<UMessageListenerBase*> DefaultListeners;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient)
	TArray<UMessageListenerBase*> OwnedListeners;
};



FORCEINLINE bool UMessageListenerBase::IsActive() const
{
	return bIsActive;
}

FORCEINLINE bool UMessageListenerBase::IsFireOnce() const
{
	return BindingInfo.bFireOnce;
}

FORCEINLINE bool UMessageListenerBase::IsListenForCancelEvents() const
{
	return !BindingInfo.bOnlyAppear;
}

FORCEINLINE bool UMessageListenerBase::HasValidBindingInfo() const
{
	return BindingInfo.EventKey.IsValid();
}

