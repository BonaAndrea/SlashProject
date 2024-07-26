#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "Engine/TargetPoint.h"
#include "Items/Soul.h"

AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->SetGenerateOverlapEvents(true);

    HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
    HealthBarWidget->SetupAttachment(GetRootComponent());
    
    PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
    PawnSensing->SightRadius = 300.f;
    PawnSensing->SetPeripheralVisionAngle(45.f);

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    // Configurazione dei parametri di raggio
    AcceptanceRadius = 50.0f;      // Raggio di accettazione
    AttackRadius = 200.0f;         // Raggio di attacco
    CombatRadius = 1000.0f;        // Raggio di combattimento
    PatrolRadius = 300.0f;         // Raggio di pattugliamento
}

void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
    if (HealthBarWidget) HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());

    InitializeEnemy();
    Tags.Add(FName("Enemy"));

    SpawnPatrolPointsAroundEnemy();
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsDead()) return;

    if (EnemyState > EEnemyState::EES_Patrolling) {
        CheckCombatTarget();
    }
    else {
        CheckPatrolTarget();
    }
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if(IsDead()) return DamageAmount;
    HandleDamage(DamageAmount);

    CombatTarget = EventInstigator->GetPawn();
    if(IsInsideAttackRadius())
    {
        EnemyState = EEnemyState::EES_Attacking;
    }
    else if(IsOutsideAttackRadius())
    {
        ChaseTarget();
    }
    return DamageAmount;
}

void AEnemy::Destroyed()
{
    if (EquippedWeapon) {
        EquippedWeapon->Destroy();
    }
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    Super::GetHit_Implementation(ImpactPoint, Hitter);
    if(!IsDead()) SetHealthBarVisibility(true);
    ClearPatrolTimer();
    ClearAttackTimer();
    StopAttackMontage();
    EquippedWeapon->DisableWeaponBoxCollision();
    if(IsDead()) return;
    if(IsInsideAttackRadius())
    {
        StartAttackTimer();
    }
}

void AEnemy::SpawnPatrolPointsAroundEnemy()
{
    const int32 NumPoints = 4; // Number of patrol points to spawn
    const float CircleRadius = 500.0f; // Radius of the circle around enemy

    for (int32 i = 0; i < NumPoints; ++i)
    {
        float Angle = 2.0f * PI * i / NumPoints; // Calculate angle for each point
        FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * CircleRadius;
        FVector SpawnLocation = GetActorLocation() + Offset;

        // Perform a line trace to find the floor's Z position
        FHitResult HitResult;
        FVector StartLocation = SpawnLocation + FVector(0.0f, 0.0f, 500.0f); // Start trace above the desired position
        FVector EndLocation = SpawnLocation + FVector(0.0f, 0.0f, -500.0f); // End trace below the desired position

        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);

        if (bHit)
        {
            // Adjust the Z position to the floor's Z position
            SpawnLocation.Z = HitResult.Location.Z;

            // Spawn ATargetPoint at calculated location
            ATargetPoint* PatrolPoint = GetWorld()->SpawnActor<ATargetPoint>(SpawnLocation, FRotator::ZeroRotator);
            if (PatrolPoint)
            {
                PatrolTargets.Add(PatrolPoint);
            }
        }
    }

    // Choose a random patrol target from the generated points
    if (PatrolTargets.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, PatrolTargets.Num() - 1);
        PatrolTarget = PatrolTargets[RandomIndex];
    }
}

void AEnemy::SpawnSoul()
{
    UWorld* World = GetWorld();
    if(World && SoulClass && Attributes)
    {
        const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
        ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
        if(SpawnedSoul)
        {
            SpawnedSoul->SetOwner(this);
            SpawnedSoul->SetSouls(Attributes->GetSouls());
        }
    }
}

void AEnemy::Die_Implementation()
{
    Super::Die_Implementation();

    EnemyState = EEnemyState::EES_Dead;
    ClearAttackTimer();
    ClearPatrolTimer();
    DisableCapsuleCollision();
    SetHealthBarVisibility(false);
    SetLifeSpan(DeathLifeSpan);
    GetCharacterMovement()->bOrientRotationToMovement = false;
    SetWeaponCollision(ECollisionEnabled::NoCollision);
    SpawnSoul();
}

void AEnemy::Attack()
{
    Super::Attack();
    if(CombatTarget == nullptr) return;
    EnemyState = EEnemyState::EES_Engaged;
    PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
    return IsInsideAttackRadius() &&
           !IsAttacking() &&
           !IsEngaged() &&
           !IsDead();
}

void AEnemy::HandleDamage(float DamageAmount)
{
    Super::HandleDamage(DamageAmount);
    if (Attributes && HealthBarWidget) {
        HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
    }
}

void AEnemy::AttackEnd()
{
    EnemyState = EEnemyState::EES_NoState;
    CheckCombatTarget();
}

void AEnemy::StartPatrolTimer()
{
    GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(PatrolWaitMin, PatrolWaitMax));
}

void AEnemy::InitializeEnemy()
{
    EnemyController = Cast<AAIController>(GetController());
    SetHealthBarVisibility(false);
    MoveToTarget(PatrolTarget);
    StartPatrolTimer();
    SpawnDefaultWeapon();
}

void AEnemy::CheckPatrolTarget()
{
    if (InTargetRange(PatrolTarget, PatrolRadius))
    {
        PatrolTarget = ChoosePatrolTarget();
        StartPatrolTimer();
    }
}

void AEnemy::CheckCombatTarget()
{
    if (IsOutsideCombatRadius())
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy is outside combat radius"));
        ClearAttackTimer();
        LoseInterest();
        if (IsEngaged())
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy is engaged, starting patrolling"));
            StartPatrolling();
        }
    }
    else if (IsOutsideAttackRadius() && !IsChasing())
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy is outside attack radius and not chasing"));
        ClearAttackTimer();
        if (!IsEngaged())
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy is not engaged, chasing target"));
            ChaseTarget();
        }
    }
    else if (CanAttack())
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy can attack"));
        StartAttackTimer();
    }
}

void AEnemy::PatrolTimerFinished()
{
    MoveToTarget(PatrolTarget);
}

void AEnemy::SetHealthBarVisibility(bool state)
{
    if (HealthBarWidget) {
        HealthBarWidget->SetVisibility(state);
    }
}

void AEnemy::LoseInterest()
{
    UE_LOG(LogTemp, Warning, TEXT("Losing interest in combat target"));
    CombatTarget = nullptr;
    SetHealthBarVisibility(false);
    EnemyState = EEnemyState::EES_Engaged;
}

void AEnemy::StartPatrolling()
{
    UE_LOG(LogTemp, Warning, TEXT("Starting patrolling"));
    EnemyState = EEnemyState::EES_Patrolling;
    GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
    PatrolTarget = ChoosePatrolTarget();
    StartPatrolTimer();
}

void AEnemy::ChaseTarget()
{
    UE_LOG(LogTemp, Warning, TEXT("Chasing target"));

    EnemyState = EEnemyState::EES_Chasing;
    GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
    MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
    return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
    return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
    return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
    return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
    return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
    return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
    return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::StartAttackTimer()
{
    EnemyState = EEnemyState::EES_Attacking;
    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, FMath::RandRange(AttackMin, AttackMax));
}

void AEnemy::ClearPatrolTimer()
{
    GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::ClearAttackTimer()
{
    GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::MoveToTarget(AActor* Target)
{
    if (EnemyController == nullptr || Target == nullptr) return;

    UE_LOG(LogTemp, Warning, TEXT("Moving to target: %s"), *Target->GetName());

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    EnemyController->MoveTo(MoveRequest);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
    if (Target == nullptr) return false;
    const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
    return DistanceToTarget <= Radius;
}

AActor* AEnemy::ChoosePatrolTarget()
{
    TArray<AActor*> ValidTargets;
    for (AActor* Target : PatrolTargets) {
        if (Target != PatrolTarget) {
            ValidTargets.AddUnique(Target);
        }
    }

    const int32 NumPatrolTargets = ValidTargets.Num();
    if (NumPatrolTargets > 0) {
        const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
        return ValidTargets[TargetSelection];
    }

    return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
    UWorld* World = GetWorld();
    if (World && WeaponClass) {
        AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
        if (DefaultWeapon) {
            DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
            EquippedWeapon = DefaultWeapon;
        }
    }
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
    const bool bShouldChaseTarget =
        EnemyState != EEnemyState::EES_Dead &&
        EnemyState != EEnemyState::EES_Chasing &&
        EnemyState < EEnemyState::EES_Attacking &&
        SeenPawn->ActorHasTag(FName("Player"));

    UE_LOG(LogTemp, Warning, TEXT("bShouldChaseTarget = %s"), bShouldChaseTarget ? TEXT("true") : TEXT("false"));
    
    if (bShouldChaseTarget) {
        CombatTarget = SeenPawn;
        ClearPatrolTimer();
        ChaseTarget();
    }
}
