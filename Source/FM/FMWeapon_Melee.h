// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "FMWeapon.h"
#include "FMWeapon_Melee.generated.h"

USTRUCT()
struct FMeleeWeaponData{

	GENERATED_USTRUCT_BODY()

	// can do combos?
	UPROPERTY(EditDefaultsOnly, Category = Usage)
	bool bCanCombo;

	// is in combo range
	UPROPERTY(EditDefaultsOnly, Category = Usage)
	bool bIsInComboRange;

	// amount of time for combo
	UPROPERTY(EditDefaultsOnly, Category = Usage)
	float comboWindowTime;

	// defaults 
	FMeleeWeaponData(){
		bCanCombo = true;
		bIsInComboRange = false;
		comboWindowTime = 0.25f;

	}

};

USTRUCT()
struct FMeleeWeaponAnim{

	GENERATED_USTRUCT_BODY()

	// animation played on pawn
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Swing_LeftToRight;

	// animation played on pawn
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Swing_RightToLeft;

	// animation played on pawn
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Swing_Overhead;

	// animation played on pawn
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Swing_Stab;

	// animation played on pawn
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Swing_Something;

};

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

	virtual void OnUseWeaponEnded() OVERRIDE;

	////////////////////////////////////////////////////////////////////
	// MELEE WEAPON CONFIGURATION
	// weapon data 
	UPROPERTY(EditDefaultsOnly, Category = _Config_Melee)
	FMeleeWeaponData MeleeConfig;

	//////////////////////////////////////////////////////////////////
	// MELEE CONTROL



	virtual void UseWeapon() OVERRIDE;
	

};
