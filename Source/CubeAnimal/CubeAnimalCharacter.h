// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplaySystemInfo.h"
#include "GenericTeamAgentInterface.h"
#include "SignificanceManager.h"
#include "GameFramework/Character.h"
#include "CubeAnimalCharacter.generated.h"

class UCubeGameplayAbility;
class UGameplayAbility;
class UAttributeSetBase;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FSignificanceThresholds
{
	GENERATED_BODY()

	FSignificanceThresholds() { Significance = 0; MaxDistance = 0; }
	FSignificanceThresholds(float InSignificance, float InMaxDistance) : Significance(InSignificance), MaxDistance(InMaxDistance) { }
	UPROPERTY(EditAnywhere)
	float Significance;
	UPROPERTY(EditAnywhere)
	float MaxDistance;
};

UCLASS(config=Game)
class ACubeAnimalCharacter : public ACharacter, public IGenericTeamAgentInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ACubeAnimalCharacter();

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UCubeGameplayAbility>> CharacterAbilities;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	virtual void BeginPlay() override;

	// Client only
	virtual void OnRep_PlayerState() override;

	virtual void PossessedBy(AController* NewController) override;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);
	
	void InitializeAttributes();
	
	UFUNCTION(BlueprintCallable)
	int32 GetCharacterLevel() const;
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetMana() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxMana() const;
	UFUNCTION(BlueprintCallable)
	float GetStamina() const;
	UFUNCTION(BlueprintCallable)
	float GetMaxStamina() const;
	UFUNCTION(BlueprintCallable)
	float GetMoveSpeed() const;
	UFUNCTION(BlueprintCallable)
	float GetMoveSpeedBaseValue() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<EGDAbilityInputID, FGameplayAbilitySpecHandle> AbilityHandles;
	
	void AddCharacterAbilities();
	void BindASCInput();

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGenericTeamId TeamId;
	
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	UPROPERTY(VisibleAnywhere)
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSetBase> AttributeSetBase;

	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Significance Functions
	float SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint);
	
	void PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal);

	UPROPERTY(EditAnywhere, Category = "Signifcance")
	TArray<FSignificanceThresholds> SignificanceThresholds;

	UPROPERTY(EditAnywhere, Category = "Signifcance")
	float DotSignificance;

	UFUNCTION(BlueprintImplementableEvent)
	void PostSignificance(float OldSignificance, float Significance);

	float GetSignificanceByDistance(float Distance);

	float GetSignificanceByDot(float Dot) const;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};

