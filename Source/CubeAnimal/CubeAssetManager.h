// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "CubeAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class CUBEANIMAL_API UCubeAssetManager : public UAssetManager
{
	GENERATED_BODY()

	virtual void StartInitialLoading() override;
};
