
// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBox.generated.h"

UCLASS()
class SHOOTERMULTI_API AInteractableBox : public AActor
{
	GENERATED_BODY()
	
public:	
	FTimerHandle TimerHandle;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	FVector transformSpawn;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TSubclassOf<AActor> boxBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta = (ClampMin = 0.1f))
	float SecondPerSpawn = 5.0f;

	// Sets default values for this actor's properties
	AInteractableBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SpawnTick();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
