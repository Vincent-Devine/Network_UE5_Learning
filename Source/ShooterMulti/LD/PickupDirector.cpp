#include "PickupDirector.h"
#include "Engine/World.h"
#include "../GameFramework/DeathMatchGS.h"

APickupDirector::APickupDirector()
{
}

void APickupDirector::BeginPlay()
{
	Super::BeginPlay();

	IsSpawnFullArray.SetNum(SpawnPoints.Num());
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APickupDirector::SpawnTick, SecondPerSpawn, true);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, this, &APickupDirector::MultiCastServerSpawnCubes, SecondPerSpawnBox, true);

	//ADeathMatchGS* GameState = Cast<ADeathMatchGS>(GetWorld()->GetGameState());
	//GameState->OnPlayerNum.AddLambda([this](ADeathMatchGS* GS) { UpdateFrequencies(GS); }); // ??
}

void APickupDirector::SpawnTick()
{
	if (bIsFull)
		return;
	
	if (GetLocalRole() == ROLE_Authority)
	{
		int MaxPoints = SpawnPoints.Num() - 1;
		int RandomPoint = FMath::RandRange(0, MaxPoints);
		int PrevPoint = RandomPoint;

		while (IsSpawnFullArray[RandomPoint])
		{
			RandomPoint = (RandomPoint + 1) % MaxPoints;
			if (RandomPoint == PrevPoint)
			{
				bIsFull = true;
				return;
			}
		}

		IsSpawnFullArray[RandomPoint] = true;
		SpawnPickup(CurrentPickupIndex, RandomPoint);
		CurrentPickupIndex = (CurrentPickupIndex + 1) % PickupBPs.Num();
	}
}

void APickupDirector::SpawnPickup(int pickupIndex, int spawnPointIndex)
{
	auto pickupBP = PickupBPs[pickupIndex];
	auto pickupLocation = SpawnPoints[spawnPointIndex]->GetActorLocation();
	auto pickupRotation = SpawnPoints[spawnPointIndex]->GetActorRotation();

	auto Pickup = GetWorld()->SpawnActor<APickup>(pickupBP, pickupLocation, pickupRotation);

	if (Pickup)
	{
		Pickup->SpawnKey.ClassKey = pickupIndex;
		Pickup->SpawnKey.SpawnPointKey = spawnPointIndex;
		Pickup->Director = this;
	}
}

void APickupDirector::FreePickup(FSpawnKey Key)
{
	IsSpawnFullArray[Key.SpawnPointKey] = false;
}

void APickupDirector::SetFull(bool isFull)
{
	bIsFull = isFull;
}

void APickupDirector::Reset()
{
	bIsFull = false;

	for (int i = 0; i < IsSpawnFullArray.Num(); i++)
		IsSpawnFullArray[i] = false;
}

// SetTeam
void APickupDirector::ServerSpawnCubes_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
		MultiCastServerSpawnCubes();
}

bool APickupDirector::ServerSpawnCubes_Validate()
{
	return true;
}

void APickupDirector::MultiCastServerSpawnCubes_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FRotator rotation;
		AActor* temp = GetWorld()->SpawnActor<AActor>(boxBP, transformSpawn, rotation);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("MEH")));
	}
}

bool APickupDirector::MultiCastServerSpawnCubes_Validate()
{
	return true;
}
