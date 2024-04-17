// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.Generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;
class USlashOverlay;
class ASoul;
class ATreasure;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void Jump() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input);
	UInputMappingContext* SlashContext;

	UPROPERTY(EditAnywhere, Category = Input);
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input);
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input);
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = Input);
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, Category = Input);
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, Category = Input);
	UInputAction* DodgeAction;
	UPROPERTY(EditAnywhere, Category = Input);
	UInputAction* PauseAction;
	UPROPERTY(EditAnywhere, Category = Input);
	TSubclassOf<UUserWidget> PauseWidget;

	UUserWidget* PauseWidgetInstance;
	
	bool IsPauseOpen = false;

	
	void EquipWeapon(AWeapon* Weapon);
	void PlayEquipMontage(FName SectionName);
	bool CanDisarm();
	bool CanArm();
	virtual bool CanAttack() override;
	virtual void Attack() override;
	void Die_Implementation() override;

	//UFUNCTION(BlueprintCallable) -> è una proprietà che eredita dal padre
	virtual void AttackEnd() override;
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void FinishAction();
	
	/** Callback functions*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Disarm();
	bool IsOccupied();
	bool HasEnoughStamina();
	void Dodge();
	void Pause();
	void Arm();
	void EKeyPressed();


private:
	void InitializeSlashOverlay(APlayerController* PlayerController);
	void SetHUDHealth();
	bool IsUnoccupied();

	/** Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;
	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	/**
	* Animation montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = Montages);
	UAnimMontage* EquipMontage;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"));
	EActionState ActionState = EActionState::EAS_Unoccupied;
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> GameOverWidget;
	UUserWidget* GameOverWidgetInstance;

public:
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; }
	FORCEINLINE ECharacterState GetCharacterState() const{ return CharacterState; }
	FORCEINLINE EActionState GetActionState() const{ return ActionState; }
};