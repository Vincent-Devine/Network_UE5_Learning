#include "ShooterController.h"
#include "../Characters/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void AShooterController::BeginPlayingState()
{
	Super::BeginPlayingState();
	if (GetPawn() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn: %s"), *GetPawn()->GetName());

		ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
		if (ShooterCharacter)
		{
			ShooterCharacter->InitPlayer();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No shoooterCharacter"));
		}
			SetReplicateMovement(true);
	}
}

void AShooterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AShooterController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AShooterController::MoveRight);
	InputComponent->BindAxis("LookUp", this, &AShooterController::LookUp);
	InputComponent->BindAxis("Turn", this, &AShooterController::Turn);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &AShooterController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AShooterController::EndSprint);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AShooterController::StartAim);
	InputComponent->BindAction("Aim", IE_Released, this, &AShooterController::EndAim);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AShooterController::StartReload);
	InputComponent->BindAction("Punch", IE_Pressed, this, &AShooterController::Punch);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AShooterController::StartJump);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShooterController::StartShoot);
	InputComponent->BindAction("Shoot", IE_Released, this, &AShooterController::EndShoot);
	InputComponent->BindAction("PushButton", IE_Pressed, this, &AShooterController::PushButton);
}

void AShooterController::MoveForward(float Value)
{
	
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		if (ShooterCharacter->GetState() == EShooterCharacterState::Sprint && Value <= 0.0f)
			EndSprint();

		FRotator Rotation = GetControlRotation();
		Rotation.Pitch = 0.f;
		Rotation.Roll = 0.f;

		ShooterCharacter->AddMovementInput(Value * Rotation.GetNormalized().Vector());
	}

}
void AShooterController::MoveRight(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead()	&& ShooterCharacter->GetState() != EShooterCharacterState::Sprint
																	&& ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		FRotator Rotation = GetControlRotation();
		Rotation.Pitch = 0.f;
		Rotation.Roll = 0.f;

		ShooterCharacter->AddMovementInput(Value * Rotation.GetNormalized().RotateVector(FVector::RightVector));
	}
}

void AShooterController::LookUp(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		AddPitchInput(Value);
	}
}
void AShooterController::Turn(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		AddYawInput(Value);
	}
}

void AShooterController::StartSprint()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ServerStartSprint();
		//ShooterCharacter->StartSprint();
}

void AShooterController::EndSprint()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ServerEndSprint();
		//ShooterCharacter->EndSprint();
}

void AShooterController::StartJump()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartJump();
}

void AShooterController::EndJump()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndJump();
}


void AShooterController::StartAim()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ServerStartAim();
}

void AShooterController::EndAim()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ServerEndAim();
}

void AShooterController::StartShoot()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ServerStartShoot();
}

void AShooterController::EndShoot()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ServerEndShoot();
}

void AShooterController::StartReload()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartReload();
}

void AShooterController::EndReload()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndReload();
}

void AShooterController::PushButton()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
	{
		ShooterCharacter->ServerPushButton();
		ShooterCharacter->PushButton();
	}
}

void AShooterController::Punch()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
	{
		ShooterCharacter->ServerPunch();
		ShooterCharacter->Punch();
	}
}

void AShooterController::DisableInput(APlayerController* PlayerController)
{
	Super::DisableInput(PlayerController);
	
	EndSprint();
	EndAim();
	EndShoot();
}

// Start Sprint
void AShooterController::ServerStartSprint_Implementation()
{
	MultiCastStartSprint();
}

bool AShooterController::ServerStartSprint_Validate()
{
	return true;
}

void AShooterController::MultiCastStartSprint_Implementation()
{
	ShooterCharacter->StartSprint();
}

bool AShooterController::MultiCastStartSprint_Validate()
{
	return true;
}

// End Sprint
void AShooterController::ServerEndSprint_Implementation()
{
	MultiCastEndSprint();
}

bool AShooterController::ServerEndSprint_Validate()
{
	return true;
}

void AShooterController::MultiCastEndSprint_Implementation()
{
	ShooterCharacter->EndSprint();
}

bool AShooterController::MultiCastEndSprint_Validate()
{
	return true;
}

// Start Shoot
void AShooterController::ServerStartShoot_Implementation()
{
	MultiCastStartShoot();
}

bool AShooterController::ServerStartShoot_Validate()
{
	return true;
}

void AShooterController::MultiCastStartShoot_Implementation()
{
	ShooterCharacter->StartShoot();
}

bool AShooterController::MultiCastStartShoot_Validate()
{
	return true;
}

// End Shoot
void AShooterController::ServerEndShoot_Implementation()
{
	MultiCastEndShoot();
}

bool AShooterController::ServerEndShoot_Validate()
{
	return true;
}

void AShooterController::MultiCastEndShoot_Implementation()
{
	ShooterCharacter->EndShoot();
}

bool AShooterController::MultiCastEndShoot_Validate()
{
	return true;
}

// Start Aim
void AShooterController::ServerStartAim_Implementation()
{
	if(GetLocalRole() != ROLE_SimulatedProxy)
		MultiCastStartAim();
}

bool AShooterController::ServerStartAim_Validate()
{
	return true;
}

void AShooterController::MultiCastStartAim_Implementation()
{
	ShooterCharacter->StartAim();
}

bool AShooterController::MultiCastStartAim_Validate()
{
	return true;
}

// End Aim
void AShooterController::ServerEndAim_Implementation()
{
	if (GetLocalRole() != ROLE_SimulatedProxy)
		MultiCastEndAim();
}

bool AShooterController::ServerEndAim_Validate()
{
	return true;
}

void AShooterController::MultiCastEndAim_Implementation()
{
	ShooterCharacter->EndAim();
}

bool AShooterController::MultiCastEndAim_Validate()
{
	return true;
}