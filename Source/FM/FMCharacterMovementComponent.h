// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "FMCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class UFMCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

	virtual float GetMaxSpeedModifier() const OVERRIDE;
	
};
