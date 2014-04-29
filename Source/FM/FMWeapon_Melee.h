// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "FMWeapon.h"
#include "FMWeapon_Melee.generated.h"

USTRUCT()
struct FMeleeWeaponData{

	GENERATED_USTRUCT_BODY()

	// amount of time for combo
	UPROPERTY(EditDefaultsOnly, Category = Usage)
	float comboWindowTime;

	// defaults 
	FMeleeWeaponData(){
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
	//UPROPERTY()
	const FName WEAPON_HILT_SOCKET = TEXT("WeaponHiltSocket");
	//UPROPERTY()
	const FName WEAPON_TIP_SOCKET = TEXT("WeaponTipSocket");

	UPROPERTY()
	FVector WeaponHiltSocketLocation;
	UPROPERTY()
	FVector WeaponTipSocketLocation;

	///////////////////////////////////////////////////////////////
	// OVERRIDEN CLASS FUNCTIONS

	//  Update the weapon
	UFUNCTION()
	virtual void Tick(float DeltaSeconds);

	//virtual void Destroyed() OVERRIDE;
	UFUNCTION()
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
	UFUNCTION()
	virtual void UseWeapon() OVERRIDE;

	UFUNCTION()
	void OnComboWindowStart();

	//////////////////////////////////////////////////////////////////
	// ANIMATIONS

private:
	UFUNCTION()
	float PlayAnimation();

	UFUNCTION()
	UAnimMontage* GetAnimation(int32 swingType, int32 swingNumber);


	//////////////////////////////////////////////////////////////////
	// HIT DETECTIONS

	UPROPERTY()
		bool bDoLineTrace;

	UPROPERTY(EditDefaultsOnly, Category = _Config_Melee)
	int32 tracePoints;
		
	TArray<FVector*> CurrentFrameTracePoints;
	
	TArray<FVector*> PreviousFrameTracePoints;
	
	TArray<FVector*> TemporaryTracePoints;

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	UFUNCTION()
	void GetSocketLocations();

	UFUNCTION()
		void InitializeTracePositions();

	UFUNCTION()
	void GetNewFrameTracePositions();

	UFUNCTION()
	void HandleHitDetection();

	void HandleHit(FHitResult* HitResult);

};
