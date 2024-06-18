// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface>*/
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface>*/

	
protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	void SpawnPatrolPointsAroundEnemy();
	void SpawnSoul();
	/** </AActor> */

	
	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;
	/** <ABaseCharacter> */

	

	UPROPERTY(VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	
private:
	/** AI Behavior */
	void InitializeEnemy();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void SetHealthBarVisibility(bool state);
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void StartAttackTimer();
	void ClearPatrolTimer();
	void ClearAttackTimer();
	void MoveToTarget(AActor* Target);
	bool InTargetRange(AActor* Target, double Radius);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	/*
	Components
	*/

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AttackRadius = 150.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	double AcceptanceRadius = 50.f;
	UPROPERTY()
	class AAIController* EnemyController;
	
	// Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

		
	FTimerHandle PatrolTimer;
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 125.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> SoulClass;

};
