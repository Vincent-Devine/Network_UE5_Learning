
#pragma once

#include "HealthCharacter.h"
#include <Net/UnrealNetwork.h>
#include "UndeadCharacter.generated.h"

UENUM(BlueprintType)
enum class EUndeadCharacterState : uint8
{
	IdleRun,
	Punch,
	Stun,
	Dead
};

UCLASS()
class SHOOTERMULTI_API AUndeadCharacter : public AHealthCharacter
{
	GENERATED_BODY()

protected:

	//bool bHasPunched;
	UPROPERTY(replicated, BlueprintReadOnly, Category = "Character|Undead")
	EUndeadCharacterState State;
	UPROPERTY(replicated)
	EUndeadCharacterState PrevState;

	void PlayPunchMontage();

	void PlayHitMontage();

	void BeginPlay() override;

public:

	UPROPERTY(Category = UndeadCharacter, EditAnywhere, BlueprintReadWrite)
	float PunchCooldown = 1.5f;

	UPROPERTY(Category = UndeadCharacter, EditAnywhere, BlueprintReadWrite)
	float StunCooldown = 1.0f;

	AUndeadCharacter();

	void StartDisapear() override;
	void Reset() override;

	UFUNCTION(BlueprintCallable, Category = "Character|Undead")
	void StartStun();
	UFUNCTION()
	void EndStun();
	UFUNCTION(BlueprintCallable, Category = "Character|Undead")
	bool Punch();
	UFUNCTION()
	void EndPunch();
	UFUNCTION(BlueprintCallable, Category = "Character|Undead")
	EUndeadCharacterState GetState() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Undead")
	void SetState(EUndeadCharacterState InState);

	virtual float	TakeDamage(float	DamageAmount,
										FDamageEvent const& DamageEvent,
										class AController* EventInstigator,
										AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Start Shoot
	UFUNCTION(Server, Reliable, WithValidation) void ServerStartShoot();
	UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastStartShoot();

	// SetTeam
	UFUNCTION(Server, Reliable, WithValidation) void ServerSetTeam(ETeam m_team);
	UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastServerSetTeam(ETeam m_team);
};
