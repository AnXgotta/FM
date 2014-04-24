// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "FMWeapon.h"
#include "FMWeapon_Melee.generated.h"

/**
 * 
 */
UCLASS()
class AFMWeapon_Melee : public AFMWeapon
{
	GENERATED_UCLASS_BODY()

	///////////////////////////////////////////////////////////////
	// OVERRIDEN CLASS FUNCTIONS

	// perform initial setup 
	// called after all components of object have been established
	//virtual void PostInitializeComponents() OVERRIDE;

	//  Update the weapon
	virtual void Tick(float DeltaSeconds) OVERRIDE;

	//virtual void Destroyed() OVERRIDE;


	//////////////////////////////////////////////////////////////////
	// MELEE CONTROL

	virtual void UseWeapon() OVERRIDE;
	

};
