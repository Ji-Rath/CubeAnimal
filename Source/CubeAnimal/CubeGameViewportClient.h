// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CubeGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class CUBEANIMAL_API UCubeGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

	UCubeGameViewportClient();
	
	virtual void Tick(float DeltaTime) override;
	
	TWeakObjectPtr<APlayerController> CachedPlayerController;
};
