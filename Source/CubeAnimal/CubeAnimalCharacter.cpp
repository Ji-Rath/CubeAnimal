// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeAnimalCharacter.h"

#include "AbilitySystemComponent.h"
#include "AttributeSetBase.h"
#include "CubeGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ACubeAnimalCharacter

ACubeAnimalCharacter::ACubeAnimalCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System"));
	AttributeSetBase = CreateDefaultSubobject<UAttributeSetBase>(TEXT("Attribute Set"));
}

float ACubeAnimalCharacter::SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
	const FTransform& Viewpoint)
{
	if(ObjectInfo->GetTag() == TEXT("Character"))
	{
		if (AActor* Character = Cast<AActor>(ObjectInfo->GetObject()))
		{
			const float Distance = (Character->GetActorLocation() - Viewpoint.GetLocation()).Size();
			const float Dot = Viewpoint.GetRotation().GetForwardVector().Dot((Character->GetActorLocation() - Viewpoint.GetLocation()));

			return GetSignificanceByDistance(Distance) * GetSignificanceByDot(Dot);
		}
	}

	return 0.f;
}

void ACubeAnimalCharacter::PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
	float OldSignificance, float Significance, bool bFinal)
{
	if (!IsValid(this)) { return; }
	
	if (ObjectInfo->GetTag() == TEXT("Character"))
	{
		PostSignificance(OldSignificance, Significance);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACubeAnimalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ACubeAnimalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ACubeAnimalCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ACubeAnimalCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ACubeAnimalCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACubeAnimalCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACubeAnimalCharacter::TouchStopped);

	BindASCInput();
}

UAbilitySystemComponent* ACubeAnimalCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACubeAnimalCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ACubeAnimalCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ACubeAnimalCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACubeAnimalCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACubeAnimalCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsNetMode(NM_DedicatedServer))
	{
		if (USignificanceManager* SignificanceManager = USignificanceManager::Get(GetWorld()))
		{
			auto Significance = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint) -> float
			{
				return SignificanceFunction(ObjectInfo, Viewpoint);
			};

			auto PostSignificance = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal)
			{
				PostSignificanceFunction(ObjectInfo, OldSignificance, Significance, bFinal);
			};

			SignificanceManager->RegisterObject(this, TEXT("Character"), Significance, USignificanceManager::EPostSignificanceType::Sequential, PostSignificance);
		}
		
	}
}

float ACubeAnimalCharacter::GetSignificanceByDistance(float Distance)
{
	const int32 NumThresholds = SignificanceThresholds.Num();
	if (Distance >= SignificanceThresholds[NumThresholds - 1].MaxDistance)
	{
		return SignificanceThresholds[NumThresholds - 1].Significance;
	}
	else
	{
		for (int32 Idx = 0; Idx < NumThresholds; Idx++)
		{
			FSignificanceThresholds& Thresholds = SignificanceThresholds[Idx];
			if (Distance <= Thresholds.MaxDistance)
			{
				return Thresholds.Significance;
			}
		}
	}

	return 0.f;
}

float ACubeAnimalCharacter::GetSignificanceByDot(float Dot) const
{
	return Dot > DotSignificance;
}

void ACubeAnimalCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!IsNetMode(NM_DedicatedServer))
	{
		if (USignificanceManager* SignificanceManager = USignificanceManager::Get(GetWorld()))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}
}

void ACubeAnimalCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BindASCInput();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ACubeAnimalCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeAttributes();
	AddCharacterAbilities();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ACubeAnimalCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACubeAnimalCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACubeAnimalCharacter::InitializeAttributes()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
	}
}

int32 ACubeAnimalCharacter::GetCharacterLevel() const
{
	if (AttributeSetBase)
	{
		return static_cast<int32>(AttributeSetBase->GetCharacterLevel());
	}

	return 0;
}

float ACubeAnimalCharacter::GetHealth() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float ACubeAnimalCharacter::GetMaxHealth() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float ACubeAnimalCharacter::GetMana() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMana();
	}

	return 0.0f;
}

float ACubeAnimalCharacter::GetMaxMana() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMaxMana();
	}

	return 0.0f;
}

float ACubeAnimalCharacter::GetStamina() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetStamina();
	}

	return 0.0f;
}

float ACubeAnimalCharacter::GetMaxStamina() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMaxStamina();
	}

	return 0.0f;
}

float ACubeAnimalCharacter::GetMoveSpeed() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
}

float ACubeAnimalCharacter::GetMoveSpeedBaseValue() const
{
	if (AttributeSetBase)
	{
		return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->GetBaseValue();
	}

	return 0.0f;
}

void ACubeAnimalCharacter::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent)
	{
		return;
	}
	AbilitySystemComponent->ClearAllAbilities();
	
	for (TSubclassOf<UCubeGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		EGDAbilityInputID InputID = StartupAbility.GetDefaultObject()->AbilityInputID;
		FGameplayAbilitySpecHandle SpecHandle;
		SpecHandle = AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(InputID), this));
		
		AbilityHandles.Add(InputID, SpecHandle);
	} 
}

void ACubeAnimalCharacter::BindASCInput()
{
	if (AbilitySystemComponent && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("EGDAbilityInputID"), static_cast<int32>(EGDAbilityInputID::Confirm), static_cast<int32>(EGDAbilityInputID::Cancel)));
	}
}
