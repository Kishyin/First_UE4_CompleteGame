// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));

	//I choose the triggerBox as root component because is it that cause the movement of the door and the floor switch when collide with our player
	RootComponent = TriggerBox;

	//Control what kind of collision is enabled dor the actor
	//I want only overlapping in collision(queryOnly), no physicOnly
	//PhysicOnly-> type of collision that will not check for overlapping events, it's for types of collisions related of physic
	//QueryOnly-> for overlap events but not for phisic
	//The TriggerBox will not collide with anything, it will overlap and do overlapping events
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//Set the collision objectType
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	//We don't want to trigger any events with collision with every actors
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	//We only want to trigger event when the botton overlap with our pawn
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//Size of collision box
	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));




	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	Door = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Door"));
	Door-> SetupAttachment(GetRootComponent());

	SwitchTime = 2.f;

	bCharacterOnSwitch = false;


}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	//Bind function for OnComponentBeginOverlap and OnComponentEndOverlap for overlaping event
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);
	
	//Get the initial location of floor switch and door
	InitialDoorLocation = Door->GetComponentLocation();
	InitialSwitchLocation = FloorSwitch->GetComponentLocation();

}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp,Warning, TEXT("Overlap Begin."));
	if (!bCharacterOnSwitch) bCharacterOnSwitch = true;
	RaiseDoor();
	LowerFloorSwitch();

}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp,Warning, TEXT("Overlap End."));
	if (bCharacterOnSwitch) bCharacterOnSwitch = false;
	//Set timer
	//The closing door event will arrive only after 2 seconds(SwitchTime)

	GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwitch::CloseDoor,SwitchTime);

}

void AFloorSwitch::UpdateDoorLocation(float Z)
{
	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z += Z;
	Door->SetWorldLocation(NewLocation);
}

void AFloorSwitch::UpdateFloorSwitchLocation(float Z)
{
	FVector NewLocation = InitialSwitchLocation;
	NewLocation.Z += Z;
	FloorSwitch->SetWorldLocation(NewLocation);
}

void AFloorSwitch::CloseDoor()
{
	if (!bCharacterOnSwitch)
	{
		LowerDoor();
		RaiseFloorSwitch();
	}
}