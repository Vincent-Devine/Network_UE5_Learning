#pragma once

#include "Pickup.h"
#include "../GameFramework/Resetable.h"
#include "GameFramework/Actor.h"
#include "PickupDirector.generated.h"

UCLASS()
class SHOOTERMULTI_API APickupDirector : public AActor, public IResetable
{
	GENERATED_BODY()
	
private:
	TArray<bool> IsSpawnFullArray;
	int CurrentPickupIndex = 0;

	FTimerHandle TimerHandle;

	bool bIsFull = false;

protected:

	virtual void BeginPlay() override;

public:
	FTimerHandle TimerHandle2;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	FVector transformSpawn;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TSubclassOf<AActor> boxBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta = (ClampMin = 0.1f))
	float SecondPerSpawnBox = 5.0f;

	APickupDirector();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta = (ClampMin = 0.1f))
	float SecondPerSpawn = 15.0f;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<TSubclassOf<APickup>> PickupBPs;
	
	void SpawnTick();
	void FreePickup(FSpawnKey Key);

	void SpawnPickup(int pickupIndex, int spawnPointIndex);

	void SetFull(bool isFull);

	//void UpdateFrequencies(class ADeathMatchGS* GameState);

	virtual void Reset() override;

	//// Spawn pick up 
	//UFUNCTION(Server, Reliable, WithValidation) void ServerSpawnPickUp();
	//UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastServerSpawnPickUp();

	// Spawn boxes
	UFUNCTION(Server, Reliable, WithValidation) void ServerSpawnCubes();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastServerSpawnCubes();
};