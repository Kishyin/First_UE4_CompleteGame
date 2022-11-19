// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Gameplay_Characters\MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"


APickup::APickup()
{
	
}


void APickup::onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::onOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor)
	{
		AMyCharacter* Main = Cast<AMyCharacter>(OtherActor);
		if (Main)
		{
			OnPickupBP(Main);
			Main->PickupLocations.Add(GetActorLocation());

			if (OverlapParticles)
			{

				//Spawn the particle effect on the actor before destroying it
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);


			}
			if (OverlapSound)
			{

				UGameplayStatics::PlaySound2D(this, OverlapSound);

			}

			//Destroy coin
			Destroy();
		}
	}
}


void APickup::onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::onOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	//UE_LOG(LogTemp, Warning, TEXT("Pickup::OnOverlapEnd"));
}