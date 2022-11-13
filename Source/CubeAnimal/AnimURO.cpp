// Copyright Wicked Games AG. All Rights Reserved.


#include "AnimURO.h"


UAnimURO::UAnimURO()
{
	PrimaryComponentTick.bCanEverTick = false;

	VisibleDistanceFactorThesholds.Add(0.24f);
	VisibleDistanceFactorThesholds.Add(0.12f);
}

void UAnimURO::BeginPlay()
{
	Super::BeginPlay();
	if (LODFrameRate.Num() <= 0) return;

	UActorComponent* pCpt = GetOwner()->GetComponentByClass(USkinnedMeshComponent::StaticClass());
	if (pCpt != nullptr)
	{
		USkinnedMeshComponent* pSKMesh = Cast<USkinnedMeshComponent>(pCpt);
		pSKMesh->bEnableUpdateRateOptimizations = true;
		pSKMesh->AnimUpdateRateParams->bShouldUseLodMap = bUseLOD;

		if (bUseLOD)
		{
			for (int i = 0; i < LODFrameRate.Num(); i++)
			{
				pSKMesh->AnimUpdateRateParams->LODToFrameSkipMap.Add(i, LODFrameRate[i]);
			}
		}
		else
		{
			pSKMesh->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Empty();
			for (int i = 0; i < VisibleDistanceFactorThesholds.Num(); i++)
			{
				pSKMesh->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Add(VisibleDistanceFactorThesholds[i]);
			}
		}
	}
}

