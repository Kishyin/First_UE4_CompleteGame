// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay_Characters\MyCharacter.h"

//Function tha initialize animation
//It's analogue to the begin play
void UMainAnimInstance::NativeInitializeAnimation()
{
	if(Pawn == nullptr)
	{
		//Assign the owner of the animation instance to "Pawn"
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Main = Cast<AMyCharacter>(Pawn);
		}
	}
}


void UMainAnimInstance::UpdateAnimationsProperties()
{
	if (Pawn == nullptr)
	{
		//This control is not really necessary beacuse the NativeInitializeAnimation 
		//is called first, but it's a way to be more secure
		Pawn = TryGetPawnOwner();
	}

	//If the pawn is valid
	if (Pawn)
	{
		//I want to control every frame, the speed and if the character is in the air or not
		FVector Speed = Pawn->GetVelocity();

		//I don't want to update animation if the velocity is in any other direction
		//than the horizontal direction
		//if he's on air we car about the jump or falling animation but if he's on ground
		//we only care about the velocity in the horizontal direction

		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		//UE_LOG(LogTemp, Warning, TEXT(" %f\n"), MovementSpeed);

		//Set BIsInAir true if the character is falling
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (Main == nullptr)
		{
			Main = Cast<AMyCharacter>(Pawn);
		}
		Main->SetAnimInstance(this);
		//



	}
}