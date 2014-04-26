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


void AFMWeapon_Melee::UseWeapon(){
	if (Role == ROLE_Authority){
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, TEXT("MeleeWeapon: SERVER : UseWeapon"));
		}
	}
	else{
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, TEXT("MeleeWeapon: CLIENT : UseWeapon"));
		}
	}

	GetWorldTimerManager().SetTimer(this, &AFMWeapon_Melee::OnUseWeaponEnded, 1.0f, false);
	chargeValue = 0.0f;
}


void AFMWeapon_Melee::OnUseWeaponEnded(){
	if (Role == ROLE_Authority){
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, TEXT("MeleeWeapon: SERVER : DoneUseWeapon"));
		}
	}
	else{
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, TEXT("MeleeWeapon: CLIENT : DoneUseWeapon"));
		}
	}

	Super::OnUseWeaponEnded();
}
