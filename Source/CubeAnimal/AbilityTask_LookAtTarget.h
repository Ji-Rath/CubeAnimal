// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_LookAtTarget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLookAtComplete);

/**
 * 
 */
UCLASS()
class CUBEANIMAL_API UAbilityTask_LookAtTarget : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAbilityTask_LookAtTarget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FLookAtComplete OnLookAtComplete;
	
	UPROPERTY(Replicated)
	AActor* Target;

	UPROPERTY(Replicated)
	float BlendSpeed;

	UPROPERTY(Replicated)
	bool bChangePitch;

	UPROPERTY(Replicated)
	bool bEndTaskOnFinish;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_LookAtTarget* LookAtTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName,
	                                               AActor* LookAtTarget,
	                                               float InterpSpeed, bool bRotatePitch, bool bEndOnFinish);
	
	virtual void TickTask(float DeltaTime) override;
};
