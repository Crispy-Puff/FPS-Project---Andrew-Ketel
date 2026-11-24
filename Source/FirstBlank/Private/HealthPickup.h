// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiflePickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class AHealthPickup : public ARiflePickup
{
	GENERATED_BODY()
	
	virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult) override;
	
	virtual void ReArm() override;
};
