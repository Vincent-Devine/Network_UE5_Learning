#include "UndeadCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "../Animations/UndeadCharacterAnim.h"
#include "../GameFramework/DeathMatchGS.h"
#include "../Controllers/UndeadAIController.h"
#include "../Weapons/DamageTypePunch.h"
#include "Components/CapsuleComponent.h"
#include <Net/UnrealNetwork.h>

AUndeadCharacter::AUndeadCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	bAlwaysRelevant = true;
}

EUndeadCharacterState AUndeadCharacter::GetState() const
{
	return State;
}

void AUndeadCharacter::SetState(EUndeadCharacterState InState)
{
	PrevState = State;
	State = InState;

	AUndeadAIController* AIController = Cast<AUndeadAIController>(Controller);
	if (IsValid(AIController))
	{
		if (PrevState == EUndeadCharacterState::Stun)
			AIController->SetIsStun(false);
		else if (State == EUndeadCharacterState::Stun)
			AIController->SetIsStun(true);

	}

	//UE_LOG(LogTemp, Warning, TEXT("State is %s"), *UEnum::GetValueAsString(State));
}

// Called when the game starts or when spawned
void AUndeadCharacter::BeginPlay()
{
	if (Team == ETeam::None && GetLocalRole() == ROLE_Authority)
		MultiCastServerSetTeam(ETeam::AI);

	Super::BeginPlay();
	
	SetState(EUndeadCharacterState::IdleRun);
}

void AUndeadCharacter::StartStun()
{
	if (State == EUndeadCharacterState::Stun)
		return;

	SetState(EUndeadCharacterState::Stun);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AUndeadCharacter::EndStun, StunCooldown, false);

	// Disabled since there is non proper hit animation for this character
	//PlayHitMontage();
}

void AUndeadCharacter::PlayHitMontage()
{
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(GetMesh());

	if (SkeletalMesh != nullptr)
	{
		UUndeadCharacterAnim* AnimInstance = Cast<UUndeadCharacterAnim>(SkeletalMesh->GetAnimInstance());
		if (AnimInstance)
			AnimInstance->PlayHitMontage();
	}
}

void AUndeadCharacter::EndStun()
{
	SetState(EUndeadCharacterState::IdleRun);
}

bool AUndeadCharacter::Punch()
{
	if (State != EUndeadCharacterState::IdleRun)
		return false;

	SetState(EUndeadCharacterState::Punch);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AUndeadCharacter::EndPunch, PunchCooldown, false);

	PlayPunchMontage();

	return true;
}

void AUndeadCharacter::ServerStartShoot_Implementation()
{
	MultiCastStartShoot();
}

bool AUndeadCharacter::ServerStartShoot_Validate()
{
	return true;
}

void AUndeadCharacter::MultiCastStartShoot_Implementation()
{
	Punch();
}

bool AUndeadCharacter::MultiCastStartShoot_Validate()
{
	return true;
}

void AUndeadCharacter::EndPunch()
{
	if (State == EUndeadCharacterState::Punch)
		SetState(EUndeadCharacterState::IdleRun);
}

void AUndeadCharacter::PlayPunchMontage()
{
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(GetMesh());

	if (SkeletalMesh != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Punch montage")));
		UUndeadCharacterAnim* AnimInstance = Cast<UUndeadCharacterAnim>(SkeletalMesh->GetAnimInstance());
		if (AnimInstance)
			AnimInstance->PlayPunchMontage();
	}
}

void AUndeadCharacter::StartDisapear()
{
	Super::StartDisapear();

	ADeathMatchGS* GameState = Cast<ADeathMatchGS>(GetWorld()->GetGameState());
	GameState->RemoveAI();
}

void AUndeadCharacter::Reset()
{
	ActivateRagdoll();

	Super::Reset();
}

float AUndeadCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float Damages = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (IsDead())
		return Damages;

	TSubclassOf<UDamageType> const DamageTypeClass = DamageEvent.DamageTypeClass;

	if (Damages > 0.0f && DamageTypeClass == UDamageTypePunch::StaticClass())
		StartStun();

	return Damages;
}

void AUndeadCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUndeadCharacter, State);
	DOREPLIFETIME(AUndeadCharacter, PrevState);

}

void AUndeadCharacter::ServerSetTeam_Implementation(ETeam team)
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
		MultiCastServerSetTeam(team);
}

bool AUndeadCharacter::ServerSetTeam_Validate(ETeam team)
{
	return true;
}

void AUndeadCharacter::MultiCastServerSetTeam_Implementation(ETeam team)
{
	SetTeam(team);
}

bool AUndeadCharacter::MultiCastServerSetTeam_Validate(ETeam team)
{
	return true;
}
