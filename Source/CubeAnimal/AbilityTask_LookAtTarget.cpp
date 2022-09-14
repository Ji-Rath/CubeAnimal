// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask_LookAtTarget.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

UAbilityTask_LookAtTarget::UAbilityTask_LookAtTarget(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bTickingTask = true;
	bSimulatedTask = true;
}
 
UAbilityTask_LookAtTarget* UAbilityTask_LookAtTarget::LookAtTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, AActor* LookAtTarget, float InterpSpeed, bool bRotatePitch, bool bEndOnFinish)
{
	UAbilityTask_LookAtTarget* MyObj = NewAbilityTask<UAbilityTask_LookAtTarget>(OwningAbility, TaskInstanceName);

	MyObj->Target = LookAtTarget;
	MyObj->BlendSpeed = InterpSpeed;
	MyObj->bChangePitch = bRotatePitch;
	MyObj->bEndTaskOnFinish = bEndOnFinish;

	return MyObj;
}

void UAbilityTask_LookAtTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	AActor* MyActor = GetAvatarActor();
	
	if(APawn* Pawn = Cast<APawn>(MyActor))
	{
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(MyActor->GetActorLocation(), Target->GetActorLocation());
		FRotator NewRotation = FMath::RInterpTo(Pawn->GetControlRotation(), Rotation, DeltaTime, BlendSpeed);
		NewRotation.Pitch = bChangePitch ? NewRotation.Pitch : Pawn->GetControlRotation().Pitch;

		if (AController* Controller = Pawn->GetController())
		{
			Controller->SetControlRotation(NewRotation);

			Rotation.Pitch = bChangePitch ? Rotation.Pitch : Pawn->GetControlRotation().Pitch;
			if (bEndTaskOnFinish && Controller->GetControlRotation().Equals(Rotation))
			{
				OnLookAtComplete.Broadcast();
				EndTask();
			}
		}
		
	}
}

void UAbilityTask_LookAtTarget::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(UAbilityTask_LookAtTarget, BlendSpeed);
	DOREPLIFETIME(UAbilityTask_LookAtTarget, Target);
}