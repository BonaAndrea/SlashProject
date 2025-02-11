// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "InteractionPopupWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"
#include "Characters/SlashCharacter.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/InteractableInterface.h"

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	DisableWeaponBoxCollision();
	if (InteractionWidgetComponent)
	{
		InteractionWidgetComponent->SetVisibility(false);
	}
}

void AWeapon::ExecuteGetHit(FHitResult BoxHit)
{
	//COME VERIFICARE SE L'ATTORE COLPITO IMPLEMENTA L'INTERFACCIA
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface) HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ActorIsSameType(OtherActor)) return;
	FHitResult BoxHit;
	BoxTrace(BoxHit);
	
	if (BoxHit.GetActor()) {
		if(ActorIsSameType(BoxHit.GetActor())) return;
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
		
	}
}

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComponent"));
	InteractionWidgetComponent->SetupAttachment(RootComponent); // Assicurati che sia attaccato alla radice dell'arma
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // Può anche essere EWidgetSpace::World per posizionamento 3D
	InteractionWidgetComponent->SetDrawSize(FVector2D(200.f, 50.f)); // Dimensioni del widget
	
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound) {
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere) {
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DisableWeaponBoxCollision()
{
	if (GetWeaponBox()) {
		GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DeactivateEmbers()
{
	if (VisualEffects) {
		VisualEffects->Deactivate();
	}
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();
	DisableWeaponBoxCollision();
	DeactivateEmbers();
	PlayEquipSound();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	ItemMesh->AttachToComponent(InParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors) {
		ActorsToIgnore.Add(Actor);
	}
	
	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxTraceExtent, BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, BoxHit, true);
	IgnoreActors.AddUnique(BoxHit.GetActor());

}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if (OtherActor && OtherActor != this)
    {
        IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(OtherActor);
        if (InteractableActor && InteractionWidgetComponent)
        {
            // Mostra il widget di interazione
            InteractionWidgetComponent->SetVisibility(true);

            // Imposta il testo dell'interazione
            ASlashCharacter* Character = Cast<ASlashCharacter>(OtherActor);
            if (Character)
            {
                APlayerController* PC = Cast<APlayerController>(Character->GetController());
                if (PC)
                {
                    bool bIsUsingGamepad = PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Bottom);
                    FText InteractionText = bIsUsingGamepad ? FText::FromString("Press X to Interact") : FText::FromString("Press E to Interact");

                    UInteractionPopupWidget* InteractionWidget = Cast<UInteractionPopupWidget>(InteractionWidgetComponent->GetUserWidgetObject());
                    if (InteractionWidget)
                    {
                        InteractionWidget->SetInteractionText(InteractionText);
                    }
                }
            }
        }
    }
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

    if (OtherActor && OtherActor != this)
    {
        IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(OtherActor);
        if (InteractableActor && InteractionWidgetComponent)
        {
            // Nascondi il widget di interazione
            InteractionWidgetComponent->SetVisibility(false);
        }
    }
}
	

