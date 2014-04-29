// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMCharacter.h"
#include "FMWeapon_Melee.h"



AFMWeapon_Melee::AFMWeapon_Melee(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bDoLineTrace = false;
	tracePoints = 20;

	for (int i = 0; i < tracePoints; i++){
		FVector* t1 = new FVector(0.0f, 0.0f, 0.0f);
		FVector* t2 = new FVector(0.0f, 0.0f, 0.0f);
		FVector* t3 = new FVector(0.0f, 0.0f, 0.0f);
		PreviousFrameTracePoints.Add(t1);
		CurrentFrameTracePoints.Add(t2);
		TemporaryTracePoints.Add(t3);
	}
}


void AFMWeapon_Melee::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	if (bDoLineTrace){
		HandleHitDetection();
	}

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
	InitializeTracePositions();
	bDoLineTrace = true;
	GetWorldTimerManager().SetTimer(this, &AFMWeapon_Melee::OnComboWindowStart, 1.0f /*timerStartComboWindowOpen*/, false);
	// END DEBUG

	GetWorldTimerManager().SetTimer(this, &AFMWeapon_Melee::OnUseWeaponEnded, 3.0f /*animationDuration*/, false);
	chargeValue = 0.0f;
}

void AFMWeapon_Melee::OnUseWeaponEnded(){
	bDoLineTrace = false;

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


////////////////////////////////////////////////////////////////////////////////////////
// HIT DETECTION

void AFMWeapon_Melee::GetSocketLocations(){
	if (Mesh3P){
		WeaponHiltSocketLocation = Mesh3P->GetSocketLocation(WEAPON_HILT_SOCKET);
		WeaponTipSocketLocation = Mesh3P->GetSocketLocation(WEAPON_TIP_SOCKET);
	}
}

void AFMWeapon_Melee::InitializeTracePositions(){
	GetSocketLocations();
	FVector differenceVector = WeaponTipSocketLocation - WeaponHiltSocketLocation;
	for (int i = 0; i < tracePoints; i++){
		*TemporaryTracePoints[i] = WeaponHiltSocketLocation + ((differenceVector / (float)tracePoints) * (float)i);
		*PreviousFrameTracePoints[i] = *TemporaryTracePoints[i];
		*CurrentFrameTracePoints[i] = *TemporaryTracePoints[i];
	}
}

void AFMWeapon_Melee::GetNewFrameTracePositions(){
	FVector differenceVector = WeaponTipSocketLocation - WeaponHiltSocketLocation;
	for (int i = 0; i < tracePoints; i++){
		*TemporaryTracePoints[i] = WeaponHiltSocketLocation + ((differenceVector/(float)tracePoints) * (float)i);
		*PreviousFrameTracePoints[i] = *CurrentFrameTracePoints[i];
		*CurrentFrameTracePoints[i] = *TemporaryTracePoints[i];
	}
}

void AFMWeapon_Melee::HandleHitDetection(){
	FHitResult HitOut;
	// need to set to our special channel we make
	ECollisionChannel traceCollisionChannel = ECC_Pawn;
	
	FCollisionQueryParams traceParams(FName(TEXT("SomeName Trace")), true, NULL);

	traceParams.bTraceComplex = true;
	traceParams.bReturnPhysicalMaterial = true;
	
	GetSocketLocations();
	GetNewFrameTracePositions();

	for (int i = 0; i < tracePoints; i++){
		HitOut = FHitResult(ForceInit);
		traceParams.AddIgnoredActors(ActorsToIgnore);
		if (i < tracePoints / 3){
			DrawDebugLine(GetWorld(), *PreviousFrameTracePoints[i], *CurrentFrameTracePoints[i], FColor::Blue, true, 5.0f, 0, 1.0f);
		}else if(i < tracePoints * 2/3){
			DrawDebugLine(GetWorld(), *PreviousFrameTracePoints[i], *CurrentFrameTracePoints[i], FColor::Green, true, 5.0f, 0, 1.0f);
		}else{
			DrawDebugLine(GetWorld(), *PreviousFrameTracePoints[i], *CurrentFrameTracePoints[i], FColor::Red, true, 5.0f, 0, 1.0f);
		}
		
		if (GetWorld()->LineTraceSingle(HitOut, *PreviousFrameTracePoints[i], *CurrentFrameTracePoints[i], traceCollisionChannel, traceParams)){
			HandleHit(&HitOut);
		}

	}

}

void AFMWeapon_Melee::HandleHit(FHitResult* HitResult){
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("Weapon: HandleHit "));
	}
	ActorsToIgnore.Add(HitResult->GetActor());
	FPointDamageEvent DamageEvent;
	DamageEvent.DamageTypeClass = UDamageType::StaticClass();
	DamageEvent.HitInfo = *HitResult;
	DamageEvent.Damage = 15.0f;
	HitResult->GetActor()->TakeDamage(DamageEvent.Damage, DamageEvent, MyPawn->Controller,this);

}