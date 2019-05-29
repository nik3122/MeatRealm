// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HeroCharacter.h"
#include "GameFramework/Controller.h"
#include "PickupBase.h"


// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	//InitialLifeSpan = 10;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComp->InitSphereRadius(15.f);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnCompBeginOverlap);
	RootComponent = CollisionComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComp->CanCharacterStepUpOn = ECB_No;

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComp->SetUpdatedComponent(CollisionComp);
	ProjectileMovementComp->ProjectileGravityScale = 0.0f;
	ProjectileMovementComp->InitialSpeed = 3000.0f;
	ProjectileMovementComp->MaxSpeed = 3000.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = false;

	// TODO Show a billboard if by default on the placeholder


}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComp->Velocity	= ShootDirection * ProjectileMovementComp->InitialSpeed;
}


void AProjectile::OnCompBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//	UE_LOG(LogTemp, Warning, TEXT("BeginOverlap... "));

	if (!HasAuthority()) return;

	auto TheReceiver = OtherActor;

	const auto IsNotWorthChecking = TheReceiver == nullptr || TheReceiver == this || OtherComp == nullptr;
	if (IsNotWorthChecking) return;

	// Ignore other projectiles
	if (TheReceiver->IsA(AProjectile::StaticClass())) return;

	if (TheReceiver->GetClass()->ImplementsInterface(UAffectableInterface::StaticClass()))
	{
		// Dont shoot myself
		if (TheReceiver == Instigator) return;

		// Get TheGiver Controller
		AHeroController* TheGiver = nullptr;
		const auto InstigatorChar = Cast<AHeroCharacter>(Instigator);
		if (InstigatorChar) TheGiver = InstigatorChar->GetHeroController();
		if (TheGiver == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Instigator of damage is null!"));
		}

		// Apply damage
		auto AffectableReceiver = Cast<IAffectableInterface>(TheReceiver);
		if (AffectableReceiver == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("AffectableReceiver of damage is null!"));
		}
		AffectableReceiver->ApplyDamage(TheGiver, ShotDamage);
	}

	Destroy();
}
