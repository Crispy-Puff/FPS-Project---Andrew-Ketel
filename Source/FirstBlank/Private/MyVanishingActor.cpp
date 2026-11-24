// Fill out your copyright notice in the Description page of Project Settings.


#include "MyVanishingActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"

// Sets default values
AMyVanishingActor::AMyVanishingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = SphereCollider;
	SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollider->SetCollisionResponseToAllChannels(ECR_Overlap);

    // Extra testing with cone instead
	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh"));
	ConeMesh->SetupAttachment(RootComponent);
	ConeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ConeMesh->SetCollisionResponseToAllChannels(ECR_Block); 

}

// Called when the game starts or when spawned
void AMyVanishingActor::BeginPlay()
{
	Super::BeginPlay();
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AMyVanishingActor::OnSphereOverlapBegin);
	ConeMesh->OnComponentHit.AddDynamic(this, &AMyVanishingActor::OnConeHit);
}

// Called every frame
void AMyVanishingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AMyVanishingActor::OnSphereOverlapBegin);
	ConeMesh->OnComponentHit.AddDynamic(this, &AMyVanishingActor::OnConeHit);

}

void AMyVanishingActor::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	HandlePlayerCollision(OtherActor);
}

void AMyVanishingActor::OnConeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HandlePlayerCollision(OtherActor);
}

void AMyVanishingActor::HandlePlayerCollision(AActor* OtherActor)
{
	if (!OtherActor) return;

	// Make sure it's the player, not some other actor
	if (OtherActor->IsA<ACharacter>()) 
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,                     // Key: -1 means always add a new message (don't overwrite by key)
				5.0f,                   // Duration in seconds the message stays on screen
				FColor::Green,          // Text color
				TEXT("Pickup collected!")
			);
		}
		
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);

		// Destroy();
	}
}
