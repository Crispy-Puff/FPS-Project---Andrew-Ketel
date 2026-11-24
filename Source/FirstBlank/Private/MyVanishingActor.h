// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyVanishingActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UConeComponent; 

UCLASS()
class AMyVanishingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyVanishingActor();

protected:
	virtual void BeginPlay() override;
	
	// Sphere collider
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	USphereComponent* SphereCollider;
	
	// Cone
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* ConeMesh; 

	UFUNCTION()
	// Sphere overlap
	void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	// Cone hit
	void OnConeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// Called either way (cone or sphere)
	void HandlePlayerCollision(AActor* OtherActor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
