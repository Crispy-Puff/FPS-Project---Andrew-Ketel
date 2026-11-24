// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWeapon.h"
#include "Pistol.generated.h"

/**
 * 
 */
UCLASS()
class APistol : public AAWeapon
{
	GENERATED_BODY()

public:
	APistol();

	virtual void Fire() override;
	
};
