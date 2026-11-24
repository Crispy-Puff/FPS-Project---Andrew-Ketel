// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CharacterCodeTest.h"
#include "Engine/World.h"

void AHealthPickup::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                         const FHitResult& SweepResult)
{
	if (ACharacterCodeTest* Character = Cast<ACharacterCodeTest>(OtherActor))
	{
		Character->Heal();
		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Heal acquired!"));
		}
		*/
		bIsVisible = false;
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);

		GetWorld()->GetTimerManager().SetTimer(ReArmTimer, this, &AHealthPickup::ReArm, 15.0f, true);
	}
}


void AHealthPickup::ReArm()
{
	Super::ReArm();
}
