#include "RiflePickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CharacterCodeTest.h"
#include "TimerManager.h"
#include "Engine/World.h"

ARiflePickup::ARiflePickup()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = SphereCollider;
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
	SphereCollider->InitSphereRadius(75.0f);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(RootComponent);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // visual only, sphere handles overlap
	bIsVisible = true;
}

void ARiflePickup::BeginPlay()
{
	Super::BeginPlay();
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ARiflePickup::OnSphereOverlapBegin);
}

void ARiflePickup::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (ACharacterCodeTest* Character = Cast<ACharacterCodeTest>(OtherActor))
	{
		Character->PickupRifle();

		bIsVisible = false;
		SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
		
		GetWorld()->GetTimerManager().SetTimer(ReArmTimer, this, &ARiflePickup::ReArm, 15.0f, true);
	}
}

void ARiflePickup::ReArm()
{
	
	bIsVisible = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
			
	GetWorld()->GetTimerManager().ClearTimer(ReArmTimer);
}

void ARiflePickup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ReArmTimer);
}