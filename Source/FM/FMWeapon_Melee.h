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
	TArray<UAnimMontage*> Swing1Animations;
	// animation played on pawn
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		TArray<UAnimMontage*> Swing2Animations;
	// animation played on pawn
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		TArray<UAnimMontage*> Swing3Animations;
	// animation played on pawn
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		TArray<UAnimMontage*> Swing4Animations;

	FMeleeWeaponAnim(){}
};

UCLASS()
class AFMWeapon_Melee : public AFMWeapon
{
	GENERATED_UCLASS_BODY()

	///////////////////////////////////////////////////////////////
	// SOCKET HANDLING [REFERENCES + NAMES]

	const FName WEAPON_HILT_SOCKET = TEXT("WeaponHiltSocket");
	const FName WEAPON_TIP_SOCKET = TEXT("WeaponTipSocket");

	FTransform WeaponHiltSocketTransform;
	FTransform WeaponTipSocketTransform;

	///////////////////////////////////////////////////////////////
	// OVERRIDEN CLASS FUNCTIONS

	//  Update the weapon
	virtual void Tick(float DeltaSeconds);

	//virtual void Destroyed() OVERRIDE;

	virtual void OnUseWeaponEnded() OVERRIDE;

	////////////////////////////////////////////////////////////////////
	// MELEE WEAPON CONFIGURATION
	// weapon data 
	UPROPERTY(EditDefaultsOnly, Category = _Config_Melee)
	FMeleeWeaponData MeleeConfig;

	// animation set
	UPROPERTY(EditDefaultsOnly, Category = _Animations_Melee)
		FMeleeWeaponAnim MeleeAnimations;

	//////////////////////////////////////////////////////////////////
	// MELEE CONTROL

	virtual void UseWeapon() OVERRIDE;
	
	void OnComboWindowStart();

	//////////////////////////////////////////////////////////////////
	// ANIMATIONS

private:

	float PlayAnimation();

	UAnimMontage* GetAnimation(int32 swingType, int32 swingNumber);

};
