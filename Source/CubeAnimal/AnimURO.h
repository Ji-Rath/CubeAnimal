// Copyright Wicked Games AG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimURO.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CUBEANIMAL_API UAnimURO : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAnimURO();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = URO, meta = (AllowPrivateAccess = "true"))
	bool bUseLOD = false;
	
	UPROPERTY(EditAnywhere, Category = URO, meta = (AllowPrivateAccess = "true"))
	TArray<int> LODFrameRate;

	UPROPERTY(EditAnywhere, Category = URO, meta = (AllowPrivateAccess = "true"))
	TArray<float> VisibleDistanceFactorThesholds;
};
