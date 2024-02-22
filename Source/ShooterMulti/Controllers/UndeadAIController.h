#pragma once

#include "AIController.h"
#include "UndeadAIController.generated.h"

UCLASS(ClassGroup = AI, BlueprintType, Blueprintable)
class SHOOTERMULTI_API AUndeadAIController : public AAIController
{
	GENERATED_BODY()
	
protected:

	UBlackboardComponent* BlackboardComponent;

	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shooter|AIController")
	UBlackboardData* BlackboardData;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Shooter|AIController")
	UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintCallable, Category = "Shooter|AIController")
	bool Punch();

	UFUNCTION()
	void SetIsStun(bool IsStun);

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//// Start Shoot
	//UFUNCTION(Server, Reliable, WithValidation) void ServerStartShoot();
	//UFUNCTION(NetMulticast, Reliable, WithValidation) void MultiCastStartShoot();
};
