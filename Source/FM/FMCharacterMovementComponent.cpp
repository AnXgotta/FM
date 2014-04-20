// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMCharacter.h"
#include "FMCharacterMovementComponent.h"


UFMCharacterMovementComponent::UFMCharacterMovementComponent(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

float UFMCharacterMovementComponent::GetMaxSpeedModifier() const {
	float SpeedMod = Super::GetMaxSpeedModifier();

	const AFMCharacter* FMCharacterOwner = Cast<AFMCharacter>(PawnOwner);
	if (FMCharacterOwner){
		if (FMCharacterOwner->IsRunning()){
			SpeedMod *= FMCharacterOwner->GetRunningSpeedModifier();
		}
	}

	return SpeedMod;
}