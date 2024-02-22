#include "DeathMatchGM.h"
#include "Engine/World.h"
#include "ShooterPS.h"
#include "DeathMatchGS.h"
#include "Kismet/KismetSystemLibrary.h"

void ADeathMatchGM::Respawn(APlayerController* PlayerController)
{
	RestartPlayerAtPlayerStart(PlayerController, ChoosePlayerStart(PlayerController));
}

void ADeathMatchGM::Quit()
{
	FGenericPlatformMisc::RequestExit(false);
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void ADeathMatchGM::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	UE_LOG(LogTemp, Warning, TEXT("GetSeamlessTravelActorList custom"));

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	ActorList.Add(GetWorld()->GetGameState());

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator++)
	{
		const APlayerController* PlayerController = Iterator->Get();
		ActorList.Add(PlayerController->GetPlayerState<AShooterPS>());
	}

	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
}

void ADeathMatchGM::CheckPlayersAreReady()
{
	UE_LOG(LogTemp, Warning, TEXT("CheckPlayersAreReady"));
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	ADeathMatchGS* MyGameState = GetGameState<ADeathMatchGS>();
	if (MyGameState != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMyGameState ok"));

		if (MyGameState->ArePlayersReady())
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerTravel to Game map"));
			bUseSeamlessTravel = true;
			GetWorld()->ServerTravel("Highrise");
		}
	}
}

void ADeathMatchGM::LoadSeamless()
{
	UE_LOG(LogTemp, Warning, TEXT("ServerTravel to Game map"));
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel("Highrise");
}


void ADeathMatchGM::PostLogin(APlayerController* Player)
{
	Super::PostLogin(Player);
	UE_LOG(LogTemp, Warning, TEXT("PostLogin"));

	UE_LOG(LogTemp, Warning, TEXT("Calling RPC Client Test"));
	AShooterPS* PS = Cast<AShooterPS>(Player->PlayerState);
	PS->Client_Test();
}

void ADeathMatchGM::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	UE_LOG(LogTemp, Warning, TEXT("PostSeamlessTravel"));
}

void ADeathMatchGM::GenericPlayerInitialization(AController* Controller)
{
	Super::GenericPlayerInitialization(Controller);
	UE_LOG(LogTemp, Warning, TEXT("GenericPlayerInitialization"));
}

void ADeathMatchGM::Logout(AController* Exiting)
{
	UE_LOG(LogTemp, Warning, TEXT("Logout"));

	const int NbPlayers = GetGameState<ADeathMatchGS>()->PlayerArray.Num() - 1;
	UE_LOG(LogTemp, Warning, TEXT("%d player left"), NbPlayers);

	if (NbPlayers <= 0)
		OnAllClientLogout();
}