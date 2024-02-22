#pragma once

#include "GameFramework/PlayerState.h"
#include "DeathMatchGM.h"
#include "PlayerGI.h"
#include "ShooterPS.generated.h"

UCLASS()
class SHOOTERMULTI_API AShooterPS : public APlayerState
{
	GENERATED_BODY()

protected:

	void BeginPlay() override;

public:

	UPROPERTY(BlueprintReadOnly)
	int NbKill;
	UPROPERTY(BlueprintReadOnly)
	int NbDeath;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FString UserName;

	// Used to copy properties from the current PlayerState to the passed one
	virtual void CopyProperties(class APlayerState* PlayerState);
	// Used to override the current PlayerState with the properties of the passed one
	virtual void OverrideWith(class APlayerState* PlayerState);

	UFUNCTION()
	void Reset();

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bIsPlayerReady = false;

	UPROPERTY(Replicated, BlueprintReadOnly)
		ETeam CurrentTeam = ETeam::None;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
		void Server_PlayerIsReady(const FPlayerInfo& PlayerInfo);

	UFUNCTION(BlueprintNativeEvent)
		void OnPlayerReady();

	UFUNCTION(Client, Reliable)
		void Client_OnPlayerReady();

	UFUNCTION(Client, Reliable)
		void Client_Test();
};
