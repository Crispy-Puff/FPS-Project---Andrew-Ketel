#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "RiflePickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class ARiflePickup : public AActor
{
	GENERATED_BODY()

public:
	ARiflePickup();
	
	bool IsAvailable() const { return bIsVisible; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	USphereComponent* SphereCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* PickupMesh;

	UFUNCTION()
	virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
	
	FTimerHandle ReArmTimer;
	int32 ReArmCount = 0;
	bool bIsVisible;
	
	virtual void ReArm();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};