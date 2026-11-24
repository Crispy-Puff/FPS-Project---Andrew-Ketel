// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterCodeTest.h"
#include "BotCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ABotCharacter : public ACharacterCodeTest
{
	GENERATED_BODY()
	
public:
	ABotCharacter();
	virtual void Tick( float DeltaTime ) override;
};
