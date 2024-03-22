// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}

void UAttributeComponent::ReceiveDamage(float DamageTaken)
{
	Health = FMath::Clamp(Health - DamageTaken, 0.f, MaxHealth);
	if (Health == 0.f) {
		//death
	}
}

float UAttributeComponent::GetHealthPercent()
{
	return Health/MaxHealth;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

