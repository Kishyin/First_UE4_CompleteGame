// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Gameplay_Characters\MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AExplosive::AExplosive()
{
	Damage = 15.f;

}

void AExplosive::onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::onOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	//UE_LOG(LogTemp, Warning, TEXT("Explosive::OnOverlapBegin"));

	if(OtherActor)
	{ 
		AMyCharacter* Main = Cast<AMyCharacter>(OtherActor);
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if(Main || Enemy)
		{ 

				
				if (OverlapParticles)
				{

					//Spawn the particle effect on the actor before destroying it
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true);


				}
				if (OverlapSound)
				{

					UGameplayStatics::PlaySound2D(this, OverlapSound);

				}
				UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);

				//Destroy explosive
				Destroy();
			
		}


	}

}


void AExplosive::onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::onOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	//UE_LOG(LogTemp, Warning, TEXT("Explosive::OnOverlapEnd"));
}