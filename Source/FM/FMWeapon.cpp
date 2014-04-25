// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "FM.h"
#include "FMWeapon.h"
#include "FMCharacter.h"



AFMWeapon::AFMWeapon(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// START FIRST PERSON MESH BS #######################################################

	/*
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->bChartDistanceFactor = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = Mesh1P;
	*/

	// END FIRST PERSON MESH BS #######################################################

	// Create 3d person mesh component to be assigned in Blueprint later for this weapon
	Mesh3P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));

	// USkinnedMeshComponent
	// "This is update frequency flag even when our Owner has not been rendered recently." 
	// (namespace EMeshComponentUpdateFlag( enumType ==> AlwaysTickPoseAndRefreshBones, AlwaysTickPose, OnlyTickPoseWhenRendered)
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;

	// USkinnedMeshComponent
	// "If true, DistanceFactor for this SkinnedMeshComponent will be added to the global chart"
	Mesh3P->bChartDistanceFactor = true;

	// UPrimitiveComponent
	// "Whether the primitive receives decals."
	//Mesh3P->bReceivesDecals = false;

	// UPrimitiveComponent
	// "Whether the primitive should cast a shadow or not."
	Mesh3P->CastShadow = true;

	// UPrimitiveComponent
	// "Changes the collision channel that this object uses when it moves."
	// (enum ECollisionChannel ==> there are a shitload of possible values)
	//Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);

	// UPrimitiveComponent
	// "Controls what kind of collision is enabled for this body."
	// (namespace ECollisionEnabled{ enum Type ==> NoCollision, QueryOnly, QueryAndPhysics)
	//Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// UPrimitiveComponent
	// "Changes all ResponseToChannels container for this PrimitiveComponent."
	// (enum ECollisionResponse => ECR_Ignore, ECR_Overlap, ECR_Block, ECR_MAX)
	//Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);

	// UPrimitiveComponent
	// "Changes a member of the ResponseToChannels container for this PrimitiveComponent."
	// (enum ECollisionResponse => ECR_Ignore, ECR_Overlap, ECR_Block, ECR_MAX)
	//Mesh3P->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);

	// UPrimitiveComponent
	// "Changes a member of the ResponseToChannels container for this PrimitiveComponent."
	// (enum ECollisionResponse => ECR_Ignore, ECR_Overlap, ECR_Block, ECR_MAX)
	//Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// UPrimitiveComponent
	// "Changes a member of the ResponseToChannels container for this PrimitiveComponent."
	// (enum ECollisionResponse => ECR_Ignore, ECR_Overlap, ECR_Block, ECR_MAX)
	//Mesh3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);

	// USceneComponent
	// "What we are currently attached to."
	// I want 3d person mesh as primary... it should be primary.. not attached to 1st person
	//Mesh3P->AttachParent = Mesh;


	// XXX: START WEAPON CLASS VARIABLES ####################################

	
	//bPendingCooldown = false;
	//CurrentState = EWeaponState::Idle;
	//StaminaCost = 25.0f;
	//LastFireTime = 0.0f;

	CurrentState = EWeaponState::Idle;

	bWantsToUse = false;

	bIsEquipped = false;
	bPendingEquip = false;
	bPendingUnEquip = false;
	bWantsToCharge = false;
	chargeValue = 0.0f;

	// XXX: END WEAPON CLASS VARIABLES #######################################

	// AActor
	// "Primary Actor tick function"."If false, this tick function will never be registered and will never tick."
	PrimaryActorTick.bCanEverTick = true;

	// AActor
	// "Primary Actor tick function"."Defines the minimum tick group for this tick function; given prerequisites, it can be delayed."
	//(enum ETickingGroup => G_PrePhysics, TG_StartPhysics, TG_DuringPhysics, TG_EndPhysics,TG_PostPhysics, TG_PostUpdateWork, TG_NewlySpawned, TG_MAX)
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	// AActor
	// No description in documentation -- looks like replication role for networking
	// (enum ENetRole => ROLE_None, ROLE_SimulatedProxy, ROLE_AutonomousProxy, ROLE_Authority, ROLE_MAX)
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	// AActor
	// No description in documentation -- looks like replication boolean to confirm that it should be replicated
	bReplicates = true;

	// AActor
	// "Replicate instigator to client (used by bNetTemporary projectiles for example)."
	bReplicateInstigator = true;

	// AActor
	// "If actor has valid Owner, call Owner's IsNetRelevantFor and GetNetPriority."
	// --Notes: I think thie means assume network role of owner... for a weapon class, this is what you want
	bNetUseOwnerRelevancy = true;

}


///////////////////////////////////////////////////////////////
// OVERRIDEN CLASS FUNCTIONS

void AFMWeapon::PostInitializeComponents(){
	Super::PostInitializeComponents();
	
	//if (WeaponConfig.InitialClips > 0)
	//{
	//	CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
	//	CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;
	//}
	
}

void AFMWeapon::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);

	if (bIsCharging){
		chargeValue = FMath::Min(chargeValue + DeltaSeconds, WeaponConfig.maxChargeValue);
		if (Role == ROLE_Authority){
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Weapon: SERVER : Charging!! %f"), chargeValue));
			}
		}
		else{
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Weapon: CLIENT : Charging!! %f"), chargeValue));
			}
		}
	}

}

void AFMWeapon::Destroyed(){
	Super::Destroyed();

	//StopSimulatingWeaponFire();
}

////////////////////////////////////////////////////////////////////////
// WEAPON STATE

EWeaponState::Type AFMWeapon::GetCurrentState() const{
	return CurrentState;
}

void AFMWeapon::SetWeaponState(EWeaponState::Type NewState){

	const EWeaponState::Type PrevState = CurrentState;

	// initial click and start initial charge
	if (PrevState == EWeaponState::Idle && NewState == EWeaponState::Charging){
		bWantsToCharge = false;
		bIsCharging = true;
		if (Role == ROLE_Authority){
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Weapon: SERVER : Charging %f"), chargeValue));
			}
		}
		else{
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Weapon: CLIENT : Charging %f"), chargeValue));
			}
		}
	}

	// release click to end charge and fire
	if (PrevState == EWeaponState::Charging && NewState == EWeaponState::Using){
		bIsCharging = false;
		// Start using weapon
		OnUseWeaponStarted();

		if (Role == ROLE_Authority){
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Weapon: SERVER : Using %f"), chargeValue));
			}
		}
		else{
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Weapon: CLIENT : Using %f"), chargeValue));
			}
		}

		chargeValue = 0.0f;

	}

	// end of using weapon
	if (PrevState == EWeaponState::Using && NewState != EWeaponState::Using){
		// STOP USE WEAPON
		// OnUseWeaponFinished()
		
	}

	// non chargable weapon initial use
	

	// non charge weapon try to use while currently using
	if (PrevState == EWeaponState::Using && NewState == EWeaponState::Using){
		// HANDLE COMBO
		// OnUseWeaponStarted()
		// for combos we need to know if already in use...
		// we will need an array to hold use types [Swings] and can do a check for currentSwing
		// if currentSwing != firstSwing, manage combo stuff [if in combo window -> do combo | else -> end use] 
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("If Previously using and currently charging."), chargeValue));
		}
		//otherwise just start first swing
	}

	CurrentState = NewState;

}

void AFMWeapon::DetermineWeaponState(){

	EWeaponState::Type NewState = EWeaponState::Idle;
	// if weapon is equipped
	if (bIsEquipped){
		// if is trying to use
		if (bWantsToUse){
			// is able(stamina)
			if (CanUse()){
				// if this is a chargable weapon
				if (WeaponConfig.bIsChargable){
					// if is trying to charge and is able (stamina), not sure if checking charge stamina is necessary
					if (bWantsToCharge && CanCharge()){
						NewState = EWeaponState::Charging;
					}else if (bIsCharging) {
						//run out of stamina while charging to force fire?
						if (GEngine){
							GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Wants to use and is charging"), chargeValue));
						}
						NewState = EWeaponState::Using;
					}else{
						NewState = EWeaponState::Using;
					}
				}else{
					// not chargable
					NewState = EWeaponState::Using;
				}
			}else{
				// HANDLE not enough stamina to use weapon
			}
		}else if(bIsCharging){
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Use during a charge"), chargeValue));
			}
			NewState = EWeaponState::Using;
		}else if (bPendingUnEquip){
			NewState = EWeaponState::UnEquipping;
		}else{
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Idle weapon."), chargeValue));
			}
		}
	}else if (bPendingEquip){
		// weapon is being equipped
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}


//////////////////////////////////////////////////////////////////////////
// INPUT


void AFMWeapon::StartUseWeaponPressed(){
	if (Role < ROLE_Authority){
		ServerStartUseWeaponPressed();
	}

	if (!bWantsToUse){
		bWantsToUse = true;
		if(WeaponConfig.bIsChargable){
			bWantsToCharge = true;
		}
			
		DetermineWeaponState();			
	}
}

void AFMWeapon::StartUseWeaponReleased(){
	if (Role < ROLE_Authority){
		ServerStartUseWeaponReleased();
	}

	if (bWantsToUse){
		bWantsToUse = false;
		if(WeaponConfig.bIsChargable){
			bWantsToCharge = false;	
		}
		DetermineWeaponState();
	}
}

bool AFMWeapon::ServerStartUseWeaponPressed_Validate(){
	return true;
}

void AFMWeapon::ServerStartUseWeaponPressed_Implementation(){
	StartUseWeaponPressed();
}

bool AFMWeapon::ServerStartUseWeaponReleased_Validate(){
	return true;
}

void AFMWeapon::ServerStartUseWeaponReleased_Implementation(){
	StartUseWeaponReleased();
}


//////////////////////////////////////////////////////////////////////////
// WEAPON USAGE

void AFMWeapon::UseStamina(bool bIsCharging){
	if (MyPawn){
		if (!bIsCharging){
			MyPawn->UseStamina(WeaponConfig.StaminaCost);
		}else{
			MyPawn->UseStamina(WeaponConfig.StaminaCostCharging);
		}
		
	}	
}

bool AFMWeapon::CanUse() const {
	bool bCanUse = MyPawn && MyPawn->CanUse();
	bool bEnoughStamina = MyPawn && (MyPawn->GetCurrentStamina() > WeaponConfig.StaminaCost);
	//bool bStateOKToUse = (CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Using);
	return bCanUse && bEnoughStamina && !bPendingCooldown;
}

bool AFMWeapon::CanCharge() const {
	bool bCanUse = MyPawn && MyPawn->CanUse();
	bool bEnoughStamina = MyPawn && (MyPawn->GetCurrentStamina() > WeaponConfig.StaminaCostCharging + WeaponConfig.StaminaCost);
	//bool bStateOKToUse = (CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Using);
	return bCanUse && bEnoughStamina && !bPendingCooldown;
	return false;
}

void AFMWeapon::StartCooldown(bool bFromReplication){
	if (!bFromReplication && Role < ROLE_Authority){
		//ServerStartCooldown();
	}

	/*
	if (bFromReplication || CanReload())
	{
	bPendingReload = true;
	DetermineWeaponState();

	float AnimDuration = PlayWeaponAnimation(ReloadAnim);
	if (AnimDuration <= 0.0f)
	{
	AnimDuration = WeaponConfig.NoAnimReloadDuration;
	}

	GetWorldTimerManager().SetTimer(this, &AShooterWeapon::StopReload, AnimDuration, false);
	if (Role == ROLE_Authority)
	{
	GetWorldTimerManager().SetTimer(this, &AShooterWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
	PlayWeaponSound(ReloadSound);
	}
	}
	*/
}

void AFMWeapon::StopCooldown(){
	/*
	if (CurrentState == EWeaponState::Reloading){
	bPendingReload = false;
	DetermineWeaponState();
	StopWeaponAnimation(ReloadAnim);
	}
	*/
}

bool AFMWeapon::ServerStopUseWeapon_Validate(){
	return true;
}

void AFMWeapon::ServerStopUseWeapon_Implementation(){
	//StopUseWeapon();
}

bool AFMWeapon::ServerStartCooldown_Validate(){
	return true;
}

void AFMWeapon::ServerStartCooldown_Implementation(){
	StartCooldown();
}

bool AFMWeapon::ServerStopCooldown_Validate(){
	return true;
}

void AFMWeapon::ServerStopCooldown_Implementation(){
	StopCooldown();
}

void AFMWeapon::ClientStartCooldown_Implementation(){
	StartCooldown();
}

void AFMWeapon::OnUseWeaponStarted(){


	HandleUseWeapon();

	/*
	// start using, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&	LastFireTime + WeaponConfig.TimeBetweenShots > GameTime){
	GetWorldTimerManager().SetTimer(this, &AShooterWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}else{
	HandleFiring();
	}
	*/
}

void AFMWeapon::OnUseWeaponFinished(){
	/*
	// stop firing FX on remote clients
	BurstCounter = 0;

	// stop firing FX locally, unless it's a dedicated server
	if (GetNetMode() != NM_DedicatedServer)
	{
	StopSimulatingWeaponFire();
	}

	GetWorldTimerManager().ClearTimer(this, &AShooterWeapon::HandleFiring);
	bRefiring = false;
	*/
}

void AFMWeapon::HandleUseWeapon(){
	
	if (CanUse()){
		if (GetNetMode() != NM_DedicatedServer){
			SimulateWeaponUse();
		}
	}

	// virtual function to actually use the weapon, do the things in child weapon classes
	UseWeapon();

	/*
	if ((CurrentAmmoInClip > 0 || HasInfiniteClip() || HasInfiniteAmmo()) && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();

			UseAmmo();

			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (MyPawn && MyPawn->IsLocallyControlled()) // [AActor->IsLocallyControlled ==> true if controlled by a local (not network) Controller
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);
			AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(MyPawn->Controller);
			AShooterHUD* MyHUD = MyPC ? Cast<AShooterHUD>(MyPC->GetHUD()) : NULL;
			if (MyHUD)
			{
				MyHUD->NotifyOutOfAmmo();
			}
		}

		// stop weapon fire FX, but stay in Firing state
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (Role < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// reload after firing last round
		if (CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}

		// setup refire timer
		bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(this, &AShooterWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
	*/
}

bool AFMWeapon::ServerHandleUseWeapon_Validate(){
	return true;
}

void AFMWeapon::ServerHandleUseWeapon_Implementation(){
	/*

	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	HandleFiring();

	if (bShouldUpdateAmmo)
	{
		// update ammo
		UseAmmo();

		// update firing FX on remote clients
		BurstCounter++;
	}
	*/
}

void AFMWeapon::SetOwningPawn(AFMCharacter* NewOwner){
	
	// I HAVE NO IDEA WHY THE CODE BELOW SHOWS ERRORS.
	// "cannot convert arg 1 from AFMCharacter* to AActor* in SetOwner(arg1)"
	// THIS MAKES NO SENSE BECAUSE AFMCharacter inherits from AActor.... it is an AActor
	// also, this is done in the shooter game with no problem
	if (MyPawn != NewOwner){
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}	
}

void AFMWeapon::HandleFiring(){
	/*
	if ((CurrentAmmoInClip > 0 || HasInfiniteClip() || HasInfiniteAmmo()) && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();

			UseAmmo();

			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);
			AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(MyPawn->Controller);
			AShooterHUD* MyHUD = MyPC ? Cast<AShooterHUD>(MyPC->GetHUD()) : NULL;
			if (MyHUD)
			{
				MyHUD->NotifyOutOfAmmo();
			}
		}

		// stop weapon fire FX, but stay in Firing state
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (Role < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// reload after firing last round
		if (CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}

		// setup refire timer
		bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(this, &AShooterWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
	*/
}

bool AFMWeapon::ServerHandleFiring_Validate(){
	return true;
}

void AFMWeapon::ServerHandleFiring_Implementation(){
	/*
	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	HandleFiring();

	if (bShouldUpdateAmmo)
	{
		// update ammo
		UseAmmo();

		// update firing FX on remote clients
		BurstCounter++;
	}
	*/
}

void AFMWeapon::SimulateWeaponUse(){

	if (Role == ROLE_Authority && CurrentState != EWeaponState::Using){
		return;
	}

	// this is where we do the animations for weapon usage


	/*
	if (Role == ROLE_Authority && CurrentState != EWeaponState::Firing)
	{
		return;
	}

	if (MuzzleFX)
	{
	USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
	if (!bLoopedMuzzleFX || MuzzlePSC == NULL)
	{
	// Split screen requires we create 2 effects. One that we see and one that the other player sees.
	if ((MyPawn != NULL) && (MyPawn->IsLocallyControlled() == true))
	{
	AController* PlayerCon = MyPawn->GetController();
	if (PlayerCon != NULL)
	{
	Mesh1P->GetSocketLocation(MuzzleAttachPoint);
	MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh1P, MuzzleAttachPoint);
	MuzzlePSC->bOwnerNoSee = false;
	MuzzlePSC->bOnlyOwnerSee = true;

	Mesh3P->GetSocketLocation(MuzzleAttachPoint);
	MuzzlePSCSecondary = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, Mesh3P, MuzzleAttachPoint);
	MuzzlePSCSecondary->bOwnerNoSee = true;
	MuzzlePSCSecondary->bOnlyOwnerSee = false;
	}
	}
	else
	{
	MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, UseWeaponMesh, MuzzleAttachPoint);
	}
	}
	}

	if (!bLoopedFireAnim || !bPlayingFireAnim)
	{
	PlayWeaponAnimation(FireAnim);
	bPlayingFireAnim = true;
	}

	if (bLoopedFireSound)
	{
	if (FireAC == NULL)
	{
	FireAC = PlayWeaponSound(FireLoopSound);
	}
	}
	else
	{
	PlayWeaponSound(FireSound);
	}

	AShooterPlayerController* PC = (MyPawn != NULL) ? Cast<AShooterPlayerController>(MyPawn->Controller) : NULL;
	if (PC != NULL && PC->IsLocalController())
	{
	if (FireCameraShake != NULL)
	{
	PC->ClientPlayCameraShake(FireCameraShake, 1);
	}
	if (FireForceFeedback != NULL)
	{
	PC->ClientPlayForceFeedback(FireForceFeedback, false, "Weapon");
	}
	}
	*/
}

void AFMWeapon::StopSimulatingWeaponUse(){

	// this is where we stop animations for whatever reason



	/*
	if (bLoopedMuzzleFX)
	{
	if (MuzzlePSC != NULL)
	{
	MuzzlePSC->DeactivateSystem();
	MuzzlePSC = NULL;
	}
	if (MuzzlePSCSecondary != NULL)
	{
	MuzzlePSCSecondary->DeactivateSystem();
	MuzzlePSCSecondary = NULL;
	}
	}

	if (bLoopedFireAnim && bPlayingFireAnim)
	{
	StopWeaponAnimation(FireAnim);
	bPlayingFireAnim = false;
	}

	if (FireAC)
	{
	FireAC->FadeOut(0.1f, 0.0f);
	FireAC = NULL;

	PlayWeaponSound(FireFinishSound);
	}
	*/
}


/////////////////////////////////////////////////////////////////////////
// REPLICATION & EFFECTS

void AFMWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFMWeapon, MyPawn);

	//DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmo, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmoInClip, COND_OwnerOnly);

	//DOREPLIFETIME_CONDITION(AShooterWeapon, BurstCounter, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(AShooterWeapon, bPendingReload, COND_SkipOwner);

}

void AFMWeapon::OnRep_MyPawn(){
	if (MyPawn){
		OnEnterInventory(MyPawn);
	}else{
		//OnLeaveInventory();
	}
}

void AFMWeapon::OnRep_Cooldown(){
	/*
	if (bPendingReload){
		StartReload(true);
	}else{
		StopReload();
	}
	*/
}

class AFMCharacter* AFMWeapon::GetPawnOwner() const {
	return MyPawn;
}

bool AFMWeapon::IsAttachedToPawn() const {
	return bIsEquipped || bPendingEquip;
}


//////////////////////////////////////////////////////////////////////////
// INVENTORY

bool AFMWeapon::IsEquipped() const {
	return bIsEquipped;
}

void AFMWeapon::OnEquip(){

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Weapon: OnEquip "));
	}

	bPendingEquip = true;
	DetermineWeaponState();

	// XXX: START NO ANIMATION YET
	//float Duration = PlayWeaponAnimation(EquipAnim);
	float Duration = 0.25f; // temp value until animations set
	if (Duration <= 0.0f) {
		// failsafe
		Duration = 0.5f;
	}
	// END NO ANIMATION YET

	// Local variable to track time of start equip time
	//EquipStartedTime = GetWorld()->GetTimeSeconds();

	// local variable for equip weapon duration
	//EquipDuration = Duration;

	// "Class to globally manage timers"."Version that takes any generic delegate."
	// SET A TIMER TO CALL A FUNCTION VIA FUNCTION POINTER AFTER SET AMOUNT OF TIME
	GetWorldTimerManager().SetTimer(this, &AFMWeapon::OnEquipFinished, Duration, false);

	// DONT CARE ABOUT SOUND NOW
	// Play equip sound locally
	//if (MyPawn && MyPawn->IsLocallyControlled()){ // [APawn->IsLocallyControlled ==> true if controlled by a local (not network) Controller
	//	PlayWeaponSound(EquipSound);
	//}
	
}

void AFMWeapon::OnEquipFinished(){
	
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Weapon: OnEquipFinished "));
	}

	EquipForUse();
	bIsEquipped = true;
	bPendingEquip = false;
	DetermineWeaponState();
	
}

void AFMWeapon::OnUnEquip(){
	
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Weapon: OnUnEquip "));
	}
	
	bPendingUnEquip = true;

	// Weapon swapping mid-weapon-swap
	if (bPendingEquip){
		//StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;
		GetWorldTimerManager().ClearTimer(this, &AFMWeapon::OnEquipFinished);

	}

	DetermineWeaponState();

	//if (bPendingReload){
	//	StopWeaponAnimation(ReloadAnim);
	//	bPendingReload = false;

	//	GetWorldTimerManager().ClearTimer(this, &AShooterWeapon::StopReload);
	//	GetWorldTimerManager().ClearTimer(this, &AShooterWeapon::ReloadWeapon);
	//}

	// XXX: START NO ANIMATION YET
	//float Duration = PlayWeaponAnimation(EquipAnim);
	float Duration = 0.25f; // temp value until animations set
	if (Duration <= 0.0f) {
		// failsafe
		Duration = 0.5f;
	}
	// END NO ANIMATION YET

	// Local variable to track time of start equip time
	//EquipStartedTime = GetWorld()->GetTimeSeconds();

	// local variable for equip weapon duration
	//UnequipDuration = Duration;

	// "Class to globally manage timers"."Version that takes any generic delegate."
	// SET A TIMER TO CALL A FUNCTION VIA FUNCTION POINTER AFTER SET AMOUNT OF TIME
	GetWorldTimerManager().SetTimer(this, &AFMWeapon::OnUnEquipFinished, Duration, false);

	// DONT CARE ABOUT SOUND NOW
	// Play equip sound locally
	//if (MyPawn && MyPawn->IsLocallyControlled()){ // [APawn->IsLocallyControlled ==> true if controlled by a local (not network) Controller
	//	PlayWeaponSound(EquipSound);
	//}
	
}

void AFMWeapon::OnUnEquipFinished(){

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Weapon: OnUnEquipFinished "));
	}

	UnequipFromUse();
	bIsEquipped = false;
	bPendingUnEquip = false;
	DetermineWeaponState();

}

void AFMWeapon::OnEnterInventory(AFMCharacter* NewOwner){	
	SetOwningPawn(NewOwner);
	UnequipFromUse();
}

void AFMWeapon::OnLeaveInventory(){
	
	if (Role == ROLE_Authority){
		SetOwningPawn(NULL);
	}

	if (IsAttachedToPawn()){
		OnUnEquip();
	}

}

void AFMWeapon::EquipForUse(){

	if (MyPawn){		

			switch (WeaponConfig.WeaponType){
			case EWeaponType::Primary:
				Mesh3P->AttachTo(MyPawn->Mesh, TEXT("RightHandSocket"), EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Secondary:
				Mesh3P->AttachTo(MyPawn->Mesh, TEXT("RightHandSocket"), EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Tertiary:
				Mesh3P->AttachTo(MyPawn->Mesh, TEXT("RightHandSocket"), EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Shield:
				Mesh3P->AttachTo(MyPawn->Mesh, TEXT("LeftHandSocket"), EAttachLocation::SnapToTarget);
				break;
			}

			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(false);

			Mesh3P->SetHiddenInGame(false);

	}
	
}

void AFMWeapon::UnequipFromUse(){

	if (MyPawn){

		switch (WeaponConfig.WeaponType){
			case EWeaponType::Primary:
				Mesh3P->AttachTo(MyPawn->Mesh, TEXT("BackSocket0"), EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Secondary:
				Mesh3P->AttachTo(MyPawn->Mesh, TEXT("LeftHipSocket"), EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Tertiary:
				Mesh3P->AttachTo(MyPawn->Mesh, TEXT("RightHipSocket"), EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Shield:
				Mesh3P->AttachTo(MyPawn->Mesh, TEXT("BackSocket1"), EAttachLocation::SnapToTarget);
				break;
		}
	
		Mesh3P->SetHiddenInGame(false);

	}
}

float AFMWeapon::GetEquipDuration() const {
	return EquipDuration;
}


//////////////////////////////////////////////////////
// NOT IMPLEMENTED

//USkeletalMeshComponent* AFMWeapon::GetWeaponMesh() const{
	/*
return (MyPawn != NULL && MyPawn->IsFirstPerson()) ? Mesh1P : Mesh3P;
}
*/

//void AFMWeapon::GiveStamina(int AddAmount){
	/*
const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
AddAmount = FMath::Min(AddAmount, MissingAmmo);
CurrentAmmo += AddAmount;

AShooterAIController* BotAI = MyPawn ? Cast<AShooterAIController>(MyPawn->GetController()) : NULL;
if (BotAI)
{
BotAI->CheckAmmo(this);
}

// start reload if clip was empty
if (GetCurrentAmmoInClip() <= 0 &&
CanReload() &&
MyPawn->GetWeapon() == this)
{
ClientStartReload();
}

}
*/

//float AFMWeapon::GetEquipStartedTime() const {
	/*
return EquipStartedTime;
}
*/