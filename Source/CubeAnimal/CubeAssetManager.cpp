// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeAssetManager.h"

#include "AbilitySystemGlobals.h"

void UCubeAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
