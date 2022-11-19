// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GIOCO_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	//We override the method(tha contains nothing int the parent class), that initialize the all
	//In fact the AnimInstance is not an actor, but it's a object, so it hasn't the BeginPlay method
	virtual	void NativeInitializeAnimation() override;


	//The animinstance is a object, so we it doesn't have a tick function
	//We create a custom tick function to update our animation every frame
	UFUNCTION(BluePrintCallable, Category = AnimationProperties)
	void UpdateAnimationsProperties();





	//Set the movement speed of every animation mixed in the blendspace
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =Movement)
	float MovementSpeed;

	//true when in air
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsInAir;

	//Initially this pawn is pointing to NULL
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	class AMyCharacter* Main;

	
};
