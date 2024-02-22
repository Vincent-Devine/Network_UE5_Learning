#include "ShooterCharacter.h"
#include "../Animations/ShooterCharacterAnim.h"
#include "../GameFramework/PlayerGI.h"
#include "../LD/EnemySpawnerButton.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Animation/AnimBlueprint.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Engine/World.h"

AShooterCharacter::AShooterCharacter()
{
	DisapearingDelay = 1.5f;

	// Animation is set in ShooterCharacter_BP to fix build.
	//// Set Animations
	//ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimContainer(TEXT("AnimBlueprint'/Game/Blueprints/Animations/ShooterAnim_BP.ShooterAnim_BP'"));

	//if (AnimContainer.Succeeded())
	//	GetMesh()->SetAnimInstanceClass(AnimContainer.Object->GeneratedClass);

	// Create Weapon
	Weapon = CreateDefaultSubobject<UWeaponComponent>("Rifle");

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshContainer(TEXT("SkeletalMesh'/Game/Weapons/Rifle.Rifle'"));
	if (MeshContainer.Succeeded())
		Weapon->SetSkeletalMesh(MeshContainer.Object);

	Weapon->SetRelativeLocation(FVector(1.0f, 4.0f, -2.0f));
	Weapon->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	Weapon->SetupAttachment(GetMesh(), "hand_r");

	// Create Sprint Arm and Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->ProbeSize = 12.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	Camera->SetupAttachment(SpringArm);
	Camera->SetRelativeLocation(FVector(30.f, 0.f, 100.f));

	SetReplicateMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;

}

EShooterCharacterState AShooterCharacter::GetState() const
{
	return State;
}

void AShooterCharacter::SetState(EShooterCharacterState InState)
{
	PrevState = State;
	State = InState;
}

UWeaponComponent* AShooterCharacter::GetWeaponComponent()
{
	return Weapon;
}

UCameraComponent* AShooterCharacter::GetCameraComponent()
{
	return Camera;
}

void AShooterCharacter::UpdateAimOffsets(float Pitch, float Yaw)
{
	AimPitch = Pitch;
	AimYaw = Yaw;
}

void AShooterCharacter::InitPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("InitPlayer"));
	//const FPlayerInfo& PlayerInfo = static_cast<UPlayerGI*>(GetGameInstance())->GetUserInfo();

	AShooterPS* playerState = static_cast<AShooterPS*>(GetPlayerState());
	UPlayerGI* gameInstance = static_cast<UPlayerGI*>(GetGameInstance());
	if (playerState)
	{
		gameInstance->SetUserInfo(playerState->CurrentTeam, playerState->UserName);
		InitTeamColor(static_cast<ETeam>(playerState->CurrentTeam));
		UE_LOG(LogTemp, Warning, TEXT("InitColor"));
	}



}

void AShooterCharacter::InitTeamColor(ETeam InTeam)
{
	UE_LOG(LogTemp, Warning, TEXT("Team color: %d"), static_cast<int>(InTeam));
	ServerSetTeam(InTeam);
	OnTeamSwitch.Broadcast();
}

void AShooterCharacter::Invincibility(float Duration)
{
	Health = 100000;
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]() { Health = MaxHealth; }, Duration, false);

	InvincibilityFX(Duration);
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, bIsShooting);
	DOREPLIFETIME(AShooterCharacter, State);
	DOREPLIFETIME(AShooterCharacter, PrevState);
	DOREPLIFETIME(AShooterCharacter, AimPitch);
	DOREPLIFETIME(AShooterCharacter, AimYaw);
}

void AShooterCharacter::ServerUpdateAimOffsets_Implementation(float newAimPitch, float newAimYaw)
{
	UpdateAimOffsets(newAimPitch, newAimYaw);
}

void AShooterCharacter::BeginPlay()
{
	OnTeamSwitch.AddLambda([this]() { RefreshTeamHUD(Team); });
	
	Super::BeginPlay();
	
	RunSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// TODO
	if (GetLocalRole() == ROLE_Authority)
		Invincibility(Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->InvincibilityTime);
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead())
		return;

	if (bIsShooting && !Weapon->Shot())
		StartReload();

	// Anim aim offsets
	FRotator LookRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
	float newAimPitch = UKismetMathLibrary::ClampAngle(LookRotation.Pitch, -90.f, 90.f);
	float newAimYaw = UKismetMathLibrary::ClampAngle(LookRotation.Yaw, -90.f, 90.f);

	if(GetLocalRole() == ROLE_AutonomousProxy)
		ServerUpdateAimOffsets(newAimPitch, newAimYaw);

	//Camera->ShakeCamera(uint8(State), GetLastMovementInputVector().Size());
}

void AShooterCharacter::StartSprint()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Reload)
		AbortReload();
	else if (State == EShooterCharacterState::Aim)
		EndAim();

	if (State != EShooterCharacterState::IdleRun && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::Sprint;
	else
		SetState(EShooterCharacterState::Sprint);

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AShooterCharacter::EndSprint()
{
	if (State != EShooterCharacterState::Sprint && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::IdleRun;
	else
		SetState(EShooterCharacterState::IdleRun);

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AShooterCharacter::StartJump()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (CanJump() && (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Sprint))
	{
		SetState(EShooterCharacterState::Jump);
		Jump();
	}
}

void AShooterCharacter::EndJump()
{
	if (State != EShooterCharacterState::Jump && State != EShooterCharacterState::Falling)
		return;

	SetState(EShooterCharacterState::IdleRun);
	StopJumping();
}

void AShooterCharacter::StartAim()
{
	if (State != EShooterCharacterState::IdleRun)
		return;
	
	SetState(EShooterCharacterState::Aim);

	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;

	SpringArm->TargetArmLength = AimArmLength;
	Camera->FieldOfView = AimFOV;
}

void AShooterCharacter::EndAim()
{
	if (State != EShooterCharacterState::Aim)
		return;

	SetState(PrevState);
	
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	
	SpringArm->TargetArmLength = StandardArmLength;
	Camera->FieldOfView = StandardFOV;
}

void AShooterCharacter::StartShoot()
{
	if (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Aim)
		bIsShooting = true;
}

void AShooterCharacter::EndShoot()
{
	bIsShooting = false;
}

void AShooterCharacter::StartReload()
{
	if (Weapon && Weapon->AmmoCount > 0 && Weapon->WeaponMagazineSize > Weapon->LoadedAmmo)
	{
		if (State == EShooterCharacterState::Aim)
			EndAim();
		else if (bIsShooting)
			bIsShooting = false;

		if (State != EShooterCharacterState::IdleRun)
			return;

		SetState(EShooterCharacterState::Reload);
		
		GetCharacterMovement()->MaxWalkSpeed = ReloadWalkSpeed;
	}
}

void AShooterCharacter::EndReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);
	
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	if(Weapon)
		Weapon->Reload();
}
void AShooterCharacter::AbortReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AShooterCharacter::Falling()
{
	Super::Falling();

	if (State == EShooterCharacterState::Jump)
		return;

	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	SetState(EShooterCharacterState::Falling);
}

void AShooterCharacter::PushButton()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::PushButton);
	PlayPushButtonAnim();
}

void AShooterCharacter::InflictPushButton()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, TSubclassOf<AEnemySpawnerButton>());

	if (OverlappingActors.Num() > 0)
	{
		AEnemySpawnerButton* Button = Cast<AEnemySpawnerButton>(OverlappingActors[0]);
		
		if (Button)
			Button->Activate(Team);
	}
}

void AShooterCharacter::PlayPushButtonAnim()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPushButtonMontage();
}

void AShooterCharacter::Punch()
{

	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::Punch);
	PlayPunchAnim();
}

void AShooterCharacter::PlayPunchAnim()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPunchMontage();
}

void AShooterCharacter::StartDisapear()
{
	Super::StartDisapear();

	if (GetLocalRole() != ROLE_Authority)
		return;
	
	FTimerHandle Handle1;
	GetWorld()->GetTimerManager().SetTimer(Handle1, [this]() { Weapon->SetVisibility(false, true); }, 1.f, false);

	if (Controller)
	{
		APlayerController* PlayerControler = Cast<APlayerController>(Controller);
		PlayerControler->DisableInput(PlayerControler);
		
		FTimerHandle Handle2;
		GetWorld()->GetTimerManager().SetTimer(Handle2, [PlayerControler]() { PlayerControler->EnableInput(PlayerControler); }, 5.0f, false);
	}
}

void AShooterCharacter::FinishDisapear()
{
	if (GetLocalRole() == ROLE_Authority && Controller)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		Super::FinishDisapear();
		Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->Respawn(PlayerController);
	}
}

// Punch
void AShooterCharacter::ServerPunch_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
		MultiCastPunch();
}

bool AShooterCharacter::ServerPunch_Validate()
{
	return true;
}

void AShooterCharacter::MultiCastPunch_Implementation()
{
	Punch();
}

bool AShooterCharacter::MultiCastPunch_Validate()
{
	return true;
}

// PushButton
void AShooterCharacter::ServerPushButton_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
		MultiCastPushButton();
}

bool AShooterCharacter::ServerPushButton_Validate()
{
	return true;
}

void AShooterCharacter::MultiCastPushButton_Implementation()
{
	PushButton();
}

bool AShooterCharacter::MultiCastPushButton_Validate()
{
	return true;
}

// SetTeam
void AShooterCharacter::ServerSetTeam_Implementation(ETeam InTeam)
{
	if (GetLocalRole() == ROLE_Authority)
		MultiCastServerSetTeam(InTeam);
}

bool AShooterCharacter::ServerSetTeam_Validate(ETeam InTeam)
{
	return true;
}

void AShooterCharacter::MultiCastServerSetTeam_Implementation(ETeam InTeam)
{
	SetTeam(InTeam);
}

bool AShooterCharacter::MultiCastServerSetTeam_Validate(ETeam InTeam)
{
	return true;
}

// SetInvincibility
void AShooterCharacter::ServerSetInvincibility_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
		MultiCastServerSetInvincibility();
}

bool AShooterCharacter::ServerSetInvincibility_Validate()
{
	return true;
}

void AShooterCharacter::MultiCastServerSetInvincibility_Implementation()
{
	Invincibility(invincibilityTime);
}

bool AShooterCharacter::MultiCastServerSetInvincibility_Validate()
{
	return true;
}