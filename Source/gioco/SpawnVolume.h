// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class GIOCO_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	//Volume where enemies will spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	class UBoxComponent* SpawningBox;

	//T is a type of variable that we can set in blueprint
	//We will populate this variable with the creature_BP blueprint
	//This is the way to declare a blueprint as variable in code
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawning")
	TSubclassOf<class AActor> Actor_1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<class AActor> Actor_2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<class AActor> Actor_3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<class AActor> Actor_4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<class AActor> Actor_5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
		TSubclassOf<class AActor> Actor_6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
		TSubclassOf<class AActor> Actor_7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
		TSubclassOf<class AActor> Actor_8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
		TSubclassOf<class AActor> Actor_9;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
		TSubclassOf<class AActor> Actor_10;

	TArray<TSubclassOf<AActor>> SpawnArray;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Point where the spawn will arrive
	UFUNCTION(BlueprintPure, Category="Spawning")
	FVector GetSpawnPoint();

	//Point where the spawn will arrive
	UFUNCTION(BlueprintPure, Category = "Spawning")
	TSubclassOf<AActor>GetSpawnActor();

	//We want to pass our creature, but a pwan is an uOBJECT
	//These are types of blueprint with c++ functionality and blueprint functionality->blueprint native event
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category= "Spawning")
	void SpawnOurActor(UClass* ToSpawn, const FVector& Location);

};
