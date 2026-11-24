#pragma once

#include "CoreMinimal.h"
#include "AWeapon.h"
#include "Rifle.generated.h"

UCLASS()
class ARifle : public AAWeapon
{
	GENERATED_BODY()

public:
	ARifle();

	virtual void Fire() override;
};