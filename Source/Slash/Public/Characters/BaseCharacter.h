// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"


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

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);


protected:
	virtual void BeginPlay() override;
	virtual void Attack();
	virtual bool CanAttack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual void Die();

	/**
	*  Play montage Functions
	*/

	virtual void PlayAttackMontage();
	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	/**
	* Animation montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages);
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages);
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages);
	UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, Category = Sounds);
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects);
	UParticleSystem* HitParticles;
};
