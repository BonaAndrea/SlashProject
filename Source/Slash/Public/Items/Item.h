// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


enum class EItemState : uint8 
{
	EIS_Hovering,
	EIS_Equipped
};

class UNiagaraComponent;
class USphereComponent;
class USoundBase;

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;

	UFUNCTION(BlueprintPure)
	float TransformedCos();
	UFUNCTION(BlueprintPure)
	float TransformedSin();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	template<typename T>
	T Avg(T First, T Second);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	virtual void SpawnPickupSystem();
	virtual void SpawnPickupSound();

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* ItemEffect;


	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;
	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;
	
	float RunningTime = 0.f;
};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First+Second)/2;
}
