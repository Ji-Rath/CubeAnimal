// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "CubeDamageCalculation.generated.h"

/**
 * 
 */
UCLASS()
class CUBEANIMAL_API UCubeDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	UCubeDamageCalculation();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
