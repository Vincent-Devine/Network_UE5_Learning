#pragma once

#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "../GameFramework/DeathMatchGS.h"
#include "ShooterController.generated.h"

UCLASS()
class SHOOTERMULTI_API AShooterController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	class AShooterCharacter* ShooterCharacter = nullptr;

	virtual void BeginPlayingState() override;
	virtual void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void StartSprint();
	void EndSprint();
	void StartJump();
	void StartAim();
	void EndAim();
	void StartShoot();
	void EndShoot();
	void StartReload();
	void PushButton();
	void Punch();

	// Start Sprint
	UFUNCTION(Server, Reliable, WithValidation) void ServerStartSprint();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastStartSprint();

	// End Sprint
	UFUNCTION(Server, Reliable, WithValidation) void ServerEndSprint();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastEndSprint();


	// Start Shoot
	UFUNCTION(Server, Reliable, WithValidation) void ServerStartShoot();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastStartShoot();

	// End Shoot
	UFUNCTION(Server, Reliable, WithValidation) void ServerEndShoot();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastEndShoot();

	// Start Aim
	UFUNCTION(Server, Reliable, WithValidation) void ServerStartAim();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastStartAim();

	// End Aim
	UFUNCTION(Server, Reliable, WithValidation) void ServerEndAim();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastEndAim();

public:
	void DisableInput(APlayerController* PlayerController) override;

	UFUNCTION(BlueprintCallable, Category = "Shooter|PlayerController")
	void EndJump();
	UFUNCTION(BlueprintCallable, Category = "Shooter|PlayerController")
	void EndReload();
};