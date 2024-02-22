#include "DeathMatchGS.h"
#include "ShooterPS.h"
#include "TimerManager.h"
#include "DeathMatchGM.h"
#include "../Characters/ShooterCharacter.h"
#include "../LD/Pickup.h"
#include "../Controllers/ShooterController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include <Net/UnrealNetwork.h>

void ADeathMatchGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADeathMatchGS, CurrentTime);
	DOREPLIFETIME(ADeathMatchGS, GameMode);
	DOREPLIFETIME(ADeathMatchGS, CurrentAICount);
	DOREPLIFETIME(ADeathMatchGS, BlueTeamScore);
	DOREPLIFETIME(ADeathMatchGS, RedTeamScore);

}

void ADeathMatchGS::BeginPlay()
{
	Super::BeginPlay();

	OnTeamWin.AddLambda([this](ETeam Team) { ShowTeamWinHUD(Team); });

	OnGameRestart.AddLambda([this]() { Reset(); });

	GameMode = Cast<ADeathMatchGM>(AuthorityGameMode);

	if (GameMode && GetLocalRole() == ROLE_Authority)
	{
		check(GameMode && "GameMode nullptr: Cast as ADeathMatchGM failed.");
		CurrentTime = GameMode->GameTime;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), GameMode->GameTime));
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);

		ResetAfterDelay();
	}


}

void ADeathMatchGS::AdvanceTimer()
{
	--CurrentTime;
	
	if (CurrentTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		if (RedTeamScore < BlueTeamScore)
			UpdateEndHud(ETeam::Blue);
		else if (RedTeamScore > BlueTeamScore)
			UpdateEndHud(ETeam::Red);
		else
			UpdateEndHud(ETeam::None);
	}
}

void ADeathMatchGS::AddScore(ETeam Team)
{
	if (GameMode && GetLocalRole() == ROLE_Authority)
	{
		if (Team == ETeam::Red && ++RedTeamScore == GameMode->MaxKill)
			UpdateEndHud(ETeam::Red);
		else if (Team == ETeam::Blue && ++BlueTeamScore == GameMode->MaxKill)
			UpdateEndHud(ETeam::Blue);
	}
}

void ADeathMatchGS::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	OnPlayerNum.Broadcast(this);
}

void ADeathMatchGS::RemovePlayerState(APlayerState* PlayerState)
{
	OnPlayerNum.Broadcast(this);

	Super::RemovePlayerState(PlayerState);
}

bool ADeathMatchGS::CanAddAI()
{
	// TODO
	if(GetLocalRole() == ROLE_Authority)
		return Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->MaxAIPerPlayer* PlayerArray.Num() > CurrentAICount;

	return false;
}

void ADeathMatchGS::AddAI()
{
	CurrentAICount++;
}

void ADeathMatchGS::RemoveAI()
{
	CurrentAICount--;
}

int ADeathMatchGS::GetNbplayer()
{
	return PlayerArray.Num();
}

void ADeathMatchGS::UpdateEndHud(ETeam Team)
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	OnTeamWin.Broadcast(Team);
}

void ADeathMatchGS::Reset()
{
	TArray<AActor*> Resetables;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UResetable::StaticClass(), Resetables);

	for (auto& res : Resetables)
		Cast<IResetable>(res)->Reset();
}

void ADeathMatchGS::ResetAfterDelay()
{
	CurrentTime = GameMode->GameTime;
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);

	RedTeamScore = 0;
	BlueTeamScore = 0;
	CurrentAICount = 0;

	OnResetAfterDelay.Broadcast();
}

void ADeathMatchGS::EndGameTrigg()
{
	OnGameRestart.Broadcast();
}

int ADeathMatchGS::NewFrequency(int sec)
{
	return sec / sqrt(0.61 * PlayerArray.Num());
}

bool ADeathMatchGS::ArePlayersReady()
{
	if (GetLocalRole() != ROLE_Authority)
		return false;

	UE_LOG(LogTemp, Warning, TEXT("Found %d players"), PlayerArray.Num());

	for (const auto PlayerState : PlayerArray)
	{
		AShooterPS* shooterPS = Cast<AShooterPS>(PlayerState);
		if (shooterPS == nullptr || !shooterPS->bIsPlayerReady)
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT all players are ready"));
			return false;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("All players are ready !!"));

	return true;
}