// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeGameViewportClient.h"

#include "SignificanceManager.h"
#include "Kismet/GameplayStatics.h"

UCubeGameViewportClient::UCubeGameViewportClient()
{
}

void UCubeGameViewportClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (World)
	{
		if (!CachedPlayerController.IsValid())
		{
			CachedPlayerController = UGameplayStatics::GetPlayerController(World, 0);
		}
		else
		{
			if (USignificanceManager* SignificanceManager = FSignificanceManagerModule::Get(World))
			{
				FVector ViewLocation;
				FRotator ViewRotation;
				CachedPlayerController.Get()->GetPlayerViewPoint(ViewLocation, ViewRotation);
				
				SignificanceManager->Update({FTransform(ViewRotation,ViewLocation, FVector::One())});
			}
		}
	}
}
