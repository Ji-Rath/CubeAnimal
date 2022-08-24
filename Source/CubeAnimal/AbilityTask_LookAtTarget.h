// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_LookAtTarget.generated.h"

/**
 * 
 */
UCLASS()
class CUBEANIMAL_API UAbilityTask_LookAtTarget : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAbilityTask_LookAtTarget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Replicated)
	AActor* Target;

	UPROPERTY(Replicated)
	float BlendSpeed;

	UPROPERTY(Replicated)
	bool bChangePitch;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_LookAtTarget* LookAtTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName,
	                                               AActor* LookAtTarget,
	                                               float InterpSpeed, bool bRotatePitch);
	
	virtual void TickTask(float DeltaTime) override;
};
