// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"


enum class EDeathPose;
class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Attack();
	virtual bool CanAttack();
	bool IsAlive();
	UFUNCTION(BlueprintNativeEvent)
	void Die();
	void DisableCapsuleCollision();
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void PlayDodgeMontage();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;	
private:
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	void DisableMeshCollision();
	UPROPERTY(EditAnywhere, Category = Combat);
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Combat);
	UParticleSystem* HitParticles;

	/**
	* Animation montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = Combat);
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Combat);
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = Combat);
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = Combat);
	UAnimMontage* DodgeMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSections;
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSections;
public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const{ return DeathPose; }

};
