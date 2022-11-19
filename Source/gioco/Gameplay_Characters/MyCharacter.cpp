// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "gioco/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "gioco/Enemy.h"
#include "gioco/MainPlayerController.h"
#include "gioco/MainAnimInstance.h"
#include "gioco/FirstSaveGame.h"
#include "gioco/ItemStorage.h"

// Sets default values/
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Camera Boom(pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	//Camera follows at the distance
	CameraBoom->TargetArmLength = 600.f;
	//Rotate arm based on controller
	CameraBoom->bUsePawnControlRotation = true;

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(40.f, 85.f);

	//Create follow camera
	FollowCamera= CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//The camera is not dependent from the controller
	//We don't want the camera to follow the controller rotation
	//Attach the camera to the end of the boom and let the boom to match
	//the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	//Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//Don't rotate when the controller rotates
	//Let that just affect the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
		 

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; //Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 530.f, 0.0f); //.. at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 550.f;
	GetCharacterMovement()->AirControl = 0.2f;


	MaxHealth = 100.f;
	Health =  100.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	Coins = 0;

	RunningSpeed = 550.f;

	SprintingSpeed = 960.f;


	bShiftKeyDown = false;

	bLMBDown = false;

	bESCDown = false;

	//Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;

	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;

	MinSprintsStamina = 50.f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;



}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	/*
	if (Map != "SunTemple")
	{
		LoadGameNoSwitch();
		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();
		}
	}
	*/

	//LoadGameNoSwitch();
	if (MainPlayerController)
	{
		MainPlayerController->GameModeOnly();
	}


}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
			if(bShiftKeyDown && (GetAnimInstance()->MovementSpeed != 0))
			{ 
				if (Stamina - DeltaStamina <= MinSprintsStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else
				{
					Stamina -= DeltaStamina;
				}

					SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else //Shift key up
			{
				if (Stamina >= MaxStamina)
				{
					Stamina = MaxStamina;
				}
				else
				{
				Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;

	case EStaminaStatus::ESS_BelowMinimum:
			if (bShiftKeyDown && (GetAnimInstance()->MovementSpeed != 0))
			{
				if (Stamina - DeltaStamina <= 0.f)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else
				{
					Stamina -= DeltaStamina;
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
			}
			else
			{
				if (Stamina + DeltaStamina >= MinSprintsStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}

			break;

	case EStaminaStatus::ESS_Exhausted:
			if (bShiftKeyDown && (GetAnimInstance()->MovementSpeed != 0))
			{
				Stamina = 0.f;
			}
			else
			{
				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
			if (Stamina + DeltaStamina >= MinSprintsStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
				
			break;
		default:
			;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator Interpolation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(Interpolation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

}

FRotator AMyCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMyCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMyCharacter::ShiftKeyUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMyCharacter::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMyCharacter::ESCUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMyCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMyCharacter::LMBUp);



	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::LookUp);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyCharacter::LookUpAtRate);

}

void AMyCharacter::Turn(float value)
{
	if (CanMove(value))
	{
		AddControllerYawInput(value);
	}
}

void AMyCharacter::LookUp(float value)
{
	if (CanMove(value))
	{
		AddControllerPitchInput(value);
	}
}

bool AMyCharacter::CanMove(float value)
{
	if (MainPlayerController)
	{
		return (value != 0.0f) && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead && !(MainPlayerController->bPauseMenuVisible));

	}
	return false;

}

void AMyCharacter::MoveForward(float value)
{
	//Controller is a physic actor that posses a pawn to move 
	if(CanMove(value))
	{
		//Find out which way is forward
		//Get the angle rotation that the controller is now facing
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);

	}
}

void AMyCharacter::MoveRight(float value)
{
	
	if(CanMove(value))
	{
		//Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);

	}
}


void AMyCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}



void AMyCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void AMyCharacter::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0.f)
	{
		Health -= Amount;
		Die();
	}
	else
	{
		Health -= Amount;
	}

	
}

void AMyCharacter::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.5f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMyCharacter::Jump()
{
	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;

	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}

void AMyCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMyCharacter::IncrementCoins(int32 Amount)
{
	
	Coins += Amount;
}

void AMyCharacter::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
}


void AMyCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
};



void AMyCharacter::ShiftKeyDown()
{
	bShiftKeyDown = true;
}


void AMyCharacter::ShiftKeyUp()
{
	bShiftKeyDown = false;
}


void AMyCharacter::ShowPickupLocations()
{

	for (auto Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 0.5f);
	}
}



void AMyCharacter::LMBUp()
{
	bLMBDown = false;
}

void AMyCharacter::ESCDown()
{
	bESCDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMyCharacter::ESCUp()
{
	bESCDown = false;
}


void AMyCharacter::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;
	
		if (ActiveOverlappingItem)
		{
			AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
			if (Weapon)
			{
				Weapon->Equip(this);
				SetActiveOverlappingItem(nullptr);
			}
		}
		else if (EquippedWeapon)
		{
			Attack();
		}

}


void AMyCharacter::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}


void AMyCharacter::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Section = FMath::RandRange(0, 1);
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 1.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			default:
				;
			}
		}
	}
	
}

void AMyCharacter::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

void AMyCharacter::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMyCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

 float AMyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	 if (Health - DamageAmount <= 0.f)
	 {
		 Health -= DamageAmount;
		 Die();
		 if (DamageCauser)
		 {
			 AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			 if (Enemy)
			 {
				 Enemy->bHasValidTarget = false;
			 }
		 }
	 }
	 else
	 {
		 Health -= DamageAmount;
	 }

	 return DamageAmount;


}


 void AMyCharacter::UpdateCombatTarget()
 {
	 TArray<AActor*> OverlappingActors;
	 GetOverlappingActors(OverlappingActors, EnemyFilter);
	 if (OverlappingActors.Num() == 0)
	 {
		 if (MainPlayerController)
		 {
			 MainPlayerController->RemoveEnemyHealthBar();
		 }
		 return;
	 }
	 AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	 if (ClosestEnemy)
	 {
		 FVector Location = GetActorLocation();

		 float MinDistance = (ClosestEnemy->GetActorLocation() - GetActorLocation()).Size();

		 for (auto Actor : OverlappingActors)
		 {
			 AEnemy* Enemy = Cast<AEnemy>(Actor);
			 if (Enemy)
			 {
				 float DistanceToActor = (Enemy->GetActorLocation() - GetActorLocation()).Size();
				 if (DistanceToActor < MinDistance)
				 {
					 MinDistance = DistanceToActor;
					 ClosestEnemy = Enemy;
				 }
			 }
		 }
		 if (MainPlayerController)
		 {
			 MainPlayerController->DisplayEnemyHealthBar();
		 }
		 SetCombatTarget(ClosestEnemy);
		 bHasCombatTarget = true;

	 }



 }

 void AMyCharacter::SwitchLevel(FName LevelName)
 {
	 UWorld* World = GetWorld();
	 if (World)
	 {
		 FString CurrentLevel = World->GetMapName();

		 FName CurrentLevelName(*CurrentLevel);

		 if (CurrentLevelName != LevelName)
		 {
			 UGameplayStatics::OpenLevel(World, LevelName);
		 }
	 }
 }

 void AMyCharacter::SaveGame()
 {
	 UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	 SaveGameInstance->CharacterStats.Health = Health;
	 SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	 SaveGameInstance->CharacterStats.Stamina = Stamina;
	 SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	 SaveGameInstance->CharacterStats.Coins = Coins;

	 FString MapName = GetWorld()->GetMapName();
	 MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	 //UE_LOG(LogTemp, Warning, TEXT("MapName : %s"), *MapName);

	 SaveGameInstance->CharacterStats.LevelName = MapName;

	 if (EquippedWeapon)
	 {
		 SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	 }


	 SaveGameInstance->CharacterStats.Location = GetActorLocation();
	 SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	 UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);


 }

 void AMyCharacter::LoadGame(bool SetPosition)
 {
	 UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	 LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	 Health = LoadGameInstance->CharacterStats.Health;
	 MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	 Stamina = LoadGameInstance->CharacterStats.Stamina;
	 MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	 Coins = LoadGameInstance->CharacterStats.Coins;

	 if (WeaponStorage)
	 {
		AItemStorage* Weapons =  GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;


			if (WeaponName != TEXT(""))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
			}

		}
	 }


	 if (SetPosition)
	 {
		 SetActorLocation(LoadGameInstance->CharacterStats.Location);
		 SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	 }

	 SetMovementStatus(EMovementStatus::EMS_Normal);
	 GetMesh()->bPauseAnims = false;
	 GetMesh()->bNoSkeletonUpdate = false;

	 if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
	 {
		 FName LevelName(*LoadGameInstance->CharacterStats.LevelName);
		 SwitchLevel(LevelName);
	 }


 }

 void AMyCharacter::LoadGameNoSwitch()
 {
	 UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	 
	 LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	 Health = LoadGameInstance->CharacterStats.Health;
	 MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	 Stamina = LoadGameInstance->CharacterStats.Stamina;
	 MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	 Coins = LoadGameInstance->CharacterStats.Coins;

	 if (WeaponStorage)
	 {
		 AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		 if (Weapons)
		 {
			 FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
			 
			 if (WeaponName != TEXT(""))
			 {
				 AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				 WeaponToEquip->Equip(this);
			 }

		 }
	 }



	 SetMovementStatus(EMovementStatus::EMS_Normal);
	 GetMesh()->bPauseAnims = false;
	 GetMesh()->bNoSkeletonUpdate = false;
 }