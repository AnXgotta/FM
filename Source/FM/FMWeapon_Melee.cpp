// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMWeapon_Melee.h"


AFMWeapon_Melee::AFMWeapon_Melee(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}


void AFMWeapon_Melee::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

/////////////////////////////////////////////////////////////////////////////////////////
// WEAPON USAGE

void AFMWeapon_Melee::UseWeapon(){
	FString t = TEXT("BLANK");
	switch (GetCurrentSwingMod()){
	case 1:
		t = TEXT("Weapon Swing 1");
		break;
	case 2:
		t = TEXT("Weapon Swing 2");
		break;
	case 3:
		t = TEXT("Weapon Swing 3");
		break;
	default:
		t = TEXT("Weapon Swing Default");
	}
	if (Role == ROLE_Authority){
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, t);
		}
	}
	else{
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, t);
		}
	}

	float animationDuration = PlayAnimation();
	if (animationDuration > 0.0f){
		float timerStartComboWindowOpen = animationDuration - MeleeConfig.comboWindowTime;
		GetWorldTimerManager().SetTimer(this, &AFMWeapon_Melee::OnComboWindowStart, timerStartComboWindowOpen, false);
	}
	
	// DEBUG
	GetWorldTimerManager().SetTimer(this, &AFMWeapon_Melee::OnComboWindowStart, 1.0f /*timerStartComboWindowOpen*/, false);
	// END DEBUG

	GetWorldTimerManager().SetTimer(this, &AFMWeapon_Melee::OnUseWeaponEnded, 3.0f /*animationDuration*/, false);
	chargeValue = 0.0f;
}


void AFMWeapon_Melee::OnUseWeaponEnded(){
	

	if (Role == ROLE_Authority){
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("MeleeWeapon: SERVER : DoneUseWeapon"));
		}
	}
	else{
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("MeleeWeapon: CLIENT : DoneUseWeapon"));
		}
	}

	Super::OnUseWeaponEnded();
}

//////////////////////////////////////////////////////////////////////////////////////////
// ANIMATION HELPERS

float AFMWeapon_Melee::PlayAnimation(){
	int swingType = (int)currentSwingID % 10;
	int swingNumber = GetCurrentSwingMod();

	UAnimMontage* currentAnimation = GetAnimation(swingType, swingNumber);
	if (currentAnimation){
		// Play animation
		if (Role == ROLE_Authority){
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, FString::Printf(TEXT("MeleeWeapon: SERVER : Play Animation SwingType: %d SwingNumber: %d"), swingType, swingNumber));
			}
		}
		else{
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, FString::Printf(TEXT("MeleeWeapon: CLIENT : Play Animation SwingType: %d SwingNumber: %d"), swingType, swingNumber));
			}
		}
		return currentAnimation->SequenceLength;
	}
	else{
		// animation not exist
		if (Role == ROLE_Authority){
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, FString::Printf(TEXT("MeleeWeapon: SERVER : Failed Animation SwingType: %d SwingNumber: %d"), swingType, swingNumber));
			}
		}
		else{
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, FString::Printf(TEXT("MeleeWeapon: CLIENT : Failed Animation SwingType: %d SwingNumber: %d"), swingType, swingNumber));
			}
		}
		return -1.0f;
	}



}

UAnimMontage* AFMWeapon_Melee::GetAnimation(int32 swingType, int32 swingNumber){
	UAnimMontage* currentAnimation = NULL;
	int32 actualSwingNumberIndex = swingNumber - 1;
	switch (swingType){
	case 1:
		if (actualSwingNumberIndex < MeleeAnimations.Swing1Animations.Num()){
			currentAnimation = MeleeAnimations.Swing1Animations[actualSwingNumberIndex];
		}
	break;
	case 2:
		if (actualSwingNumberIndex < MeleeAnimations.Swing2Animations.Num()){
			currentAnimation = MeleeAnimations.Swing2Animations[actualSwingNumberIndex];
		}
		break;
	case 3:
		if (actualSwingNumberIndex < MeleeAnimations.Swing3Animations.Num()){
			currentAnimation = MeleeAnimations.Swing3Animations[actualSwingNumberIndex];
		}
		break;
	case 4:
		if (actualSwingNumberIndex < MeleeAnimations.Swing4Animations.Num()){
			currentAnimation = MeleeAnimations.Swing4Animations[actualSwingNumberIndex];
		}
		break;
	}

	return currentAnimation;
}

void AFMWeapon_Melee::OnComboWindowStart(){
	bIsInComboWindow = true;
}