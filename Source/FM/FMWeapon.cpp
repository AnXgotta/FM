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
	//Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;

	// USkinnedMeshComponent
	// "If true, DistanceFactor for this SkinnedMeshComponent will be added to the global chart"
	//Mesh3P->bChartDistanceFactor = true;

	// UPrimitiveComponent
	// "Whether the primitive receives decals."
	//Mesh3P->bReceivesDecals = false;

	// UPrimitiveComponent
	// "Whether the primitive should cast a shadow or not."
	//Mesh3P->CastShadow = true;

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
	//Mesh3P->AttachParent = Mesh1P;


	// XXX: START WEAPON CLASS VARIABLES ####################################

	//bWantsToUse = false;
	//bPendingCooldown = false;
	//CurrentState = EWeaponState::Idle;
	//StaminaCost = 25.0f;
	//LastFireTime = 0.0f;

	CurrentState = EWeaponState::Idle;

	bIsEquipped = false;
	bPendingEquip = false;

	// XXX: END WEAPON CLASS VARIABLES #######################################

	// AActor
	// "Primary Actor tick function"."If false, this tick function will never be registered and will never tick."
	//PrimaryActorTick.bCanEverTick = true;

	// AActor
	// "Primary Actor tick function"."Defines the minimum tick group for this tick function; given prerequisites, it can be delayed."
	//(enum ETickingGroup => G_PrePhysics, TG_StartPhysics, TG_DuringPhysics, TG_EndPhysics,TG_PostPhysics, TG_PostUpdateWork, TG_NewlySpawned, TG_MAX)
	//PrimaryActorTick.TickGroup = TG_PrePhysics;

	// AActor
	// No description in documentation -- looks like replication role for networking
	// (enum ENetRole => ROLE_None, ROLE_SimulatedProxy, ROLE_AutonomousProxy, ROLE_Authority, ROLE_MAX)
	//SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	// AActor
	// No description in documentation -- looks like replication boolean to confirm that it should be replicated
	//bReplicates = true;

	// AActor
	// "Replicate instigator to client (used by bNetTemporary projectiles for example)."
	//bReplicateInstigator = true;

	// AActor
	// "If actor has valid Owner, call Owner's IsNetRelevantFor and GetNetPriority."
	// --Notes: I think thie means assume network role of owner... for a weapon class, this is what you want
	//bNetUseOwnerRelevancy = true;

}



void AFMWeapon::PostInitializeComponents(){
	Super::PostInitializeComponents();
	
	//if (WeaponConfig.InitialClips > 0)
	//{
	//	CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
	//	CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;
	//}
	
}

//////////////////////////////////////////////////////////////////////////
// Input
/*
void AFMWeapon::StartUseWeapon(){
	if (Role < ROLE_Authority){
		ServerStartUseWeapon();
	}

	if (!bWantsToUse){
		bWantsToUse = true;
		DetermineWeaponState();
	}
}
/*
void AFMWeapon::StopUseWeapon(){
	if (Role < ROLE_Authority){

		ServerStopUseWeapon();
	}

	if (bWantsToUse){
		bWantsToUse = false;
		DetermineWeaponState();
	}
}
/*
void AFMWeapon::StartCooldown(bool bFromReplication){
	if (!bFromReplication && Role < ROLE_Authority){
		ServerStartCooldown();
	}
	
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
}
*/
/*
void AFMWeapon::StopCooldown(){
	if (CurrentState == EWeaponState::Reloading){
		bPendingReload = false;
		DetermineWeaponState();
		StopWeaponAnimation(ReloadAnim);
	}
}
*/
/*
bool AFMWeapon::ServerStartUseWeapon_Validate(){
	return true;
}
*/
/*
void AFMWeapon::ServerStartUseWeapon_Implementation(){
	StartUseWeapon();
}
*/
/*
bool AFMWeapon::ServerStopUseWeapon_Validate(){
	return true;
}
*/
/*
void AFMWeapon::ServerStopUseWeapon_Implementation(){
	StopUseWeapon();
}
*/
/*
bool AFMWeapon::ServerStartCooldown_Validate(){
	return true;
}
*/
/*
void AFMWeapon::ServerStartCooldown_Implementation(){
	StartCooldown();
}
*/
/*
bool AFMWeapon::ServerStopCooldown_Validate(){
	return true;
}
*/
/*
void AFMWeapon::ServerStopCooldown_Implementation(){
	StopCooldown();
}
*/
/*
void AFMWeapon::ClientStartCooldown_Implementation(){
	StartCooldown();
}
*/
//////////////////////////////////////////////////////////////////////////
// Weapon usage
/*
void AFMWeapon::GiveStamina(int AddAmount){
	
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


void AFMWeapon::UseStamina(){
	
	// check for buffs or whatever in IF
	//MyPawn->UseStamina(StaminaCost);
	
}


/*
void AFMWeapon::HandleUseWeapon(){
	
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
	
}
*/
/*
bool AFMWeapon::ServerHandleUseWeapon_Validate(){
	return true;
}
*/
/*
void AFMWeapon::ServerHandleUseWeapon_Implementation(){
	
	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	HandleFiring();

	if (bShouldUpdateAmmo)
	{
		// update ammo
		UseAmmo();

		// update firing FX on remote clients
		BurstCounter++;
	}
	
}
*/

void AFMWeapon::SetWeaponState(EWeaponState::Type NewState){
	
	const EWeaponState::Type PrevState = CurrentState;

	if (PrevState == EWeaponState::Using && NewState != EWeaponState::Using){
		//OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Using && NewState == EWeaponState::Using){
		//OnBurstStarted();
	}
	
}

void AFMWeapon::DetermineWeaponState(){
	
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped){
		//if (bPendingReload){
			//if (CanReload() == false){
			//	NewState = CurrentState;
			//}else{
			//	NewState = EWeaponState::Reloading;
			//}
		//}else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))		{
		//	NewState = EWeaponState::Firing;
		//}
	}else if (bPendingEquip){
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);	
}

void AFMWeapon::SetOwningPawn(AFMCharacter* NewOwner){
	
	// I HAVE NO IDEA WHY THE CODE BELOW SHOWS ERRORS.
	// "cannot convert arg 1 from AFMCharacter* to AActor* in SetOwner(arg1)"
	// THIS MAKES NO SENSE BECAUSE AFMCharacter inherits from AActor.... it is an AActor
	// also, this is done in the shooter game with no problem
	if (MyPawn != NewOwner){
		//Instigator = NewOwner;
		//MyPawn = NewOwner;
		// net owner for RPC calls
		//SetOwner(NewOwner);
	}	
}

void AFMWeapon::OnBurstStarted(){
/*	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
		LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(this, &AShooterWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
	*/
}

void AFMWeapon::OnBurstFinished(){
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


/////////////////////////////////////////////////////////////////////////
// Replication & effects

void AFMWeapon::OnRep_MyPawn(){
	if (MyPawn)
	{
		//OnEnterInventory(MyPawn);
	}
	else
	{
		//OnLeaveInventory();
	}
}

void AFMWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFMWeapon, MyPawn);

	//DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmo, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmoInClip, COND_OwnerOnly);

	//DOREPLIFETIME_CONDITION(AShooterWeapon, BurstCounter, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(AShooterWeapon, bPendingReload, COND_SkipOwner);

}

/*
void AFMWeapon::OnRep_Cooldown(){
	
	if (bPendingReload)
	{
		StartReload(true);
	}
	else
	{
		StopReload();
	}

}
*/
/*
void AFMWeapon::SimulateWeaponUse(){
	
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
	
}
*/
/*
void AFMWeapon::StopSimulatingWeaponUse(){
	
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
	
}
*/



/*
USkeletalMeshComponent* AFMWeapon::GetWeaponMesh() const
{
	return (MyPawn != NULL && MyPawn->IsFirstPerson()) ? Mesh1P : Mesh3P;
}
*/

class AFMCharacter* AFMWeapon::GetPawnOwner() const {
	return MyPawn;
}


bool AFMWeapon::IsEquipped() const {
	return bIsEquipped;
}



bool AFMWeapon::IsAttachedToPawn() const {
	return bIsEquipped || bPendingEquip;
}


EWeaponState::Type AFMWeapon::GetCurrentState() const{
	return CurrentState;
}

/*
int32 AFMWeapon::GetStaminaCost() const {
	return StaminaCost;
}
*/
/*
float AFMWeapon::GetEquipStartedTime() const {
	return EquipStartedTime;
}
*/
/*
float AFMWeapon::GetEquipDuration() const {
	return EquipDuration;
}
*/


//////////////////////////////////////////////////////////////////////////
// Inventory


void AFMWeapon::OnEquip(){
	
	AttachMeshToPawn();

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
	EquipStartedTime = GetWorld()->GetTimeSeconds();

	// local variable for equip weapon duration
	EquipDuration = Duration;

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
	
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	if (MyPawn) {
		// try to reload empty clip
		//if (MyPawn->IsLocallyControlled() && CurrentAmmoInClip <= 0 && CanReload()) {
		//	StartReload();
		//}
	}

	DetermineWeaponState();
	
}

void AFMWeapon::OnUnEquip(){
	
	DetachMeshFromPawn();
	bIsEquipped = false;
	//StopFire();

	//if (bPendingReload){
	//	StopWeaponAnimation(ReloadAnim);
	//	bPendingReload = false;

	//	GetWorldTimerManager().ClearTimer(this, &AShooterWeapon::StopReload);
	//	GetWorldTimerManager().ClearTimer(this, &AShooterWeapon::ReloadWeapon);
	//}

	if (bPendingEquip){
		//StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(this, &AFMWeapon::OnEquipFinished);
	}

	DetermineWeaponState();
	
}

void AFMWeapon::OnEnterInventory(AFMCharacter* NewOwner){	
	SetOwningPawn(NewOwner);	
}

void AFMWeapon::OnLeaveInventory(){
	
	if (Role == ROLE_Authority){
		SetOwningPawn(NULL);
	}

	if (IsAttachedToPawn()){
		OnUnEquip();
	}

}

void AFMWeapon::AttachMeshToPawn(){
	
	if (MyPawn){
		// Remove and hide both first and third person meshes
		// in current case, only 3rd person mesh
		DetachMeshFromPawn();

		// NO-> For locally controller players we attach weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		// NO-> FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		// WE NEED TO ATTACH WEAPON WHERE IT BELONGS, THIS MIGHT BE ASSUMING ATTACH TO CURRENTLY EQUIPPED WEAPON
		// DEPENDING ON WEAPON, IT WILL BE ATTACHED DIFFERENTLY.  [TWO HAND, ONE HAND, ETC]
		// MUST CUSTOMIZE


		if (MyPawn->IsLocallyControlled() == true){
			// 'MyPawn->GetSpecificPawnMesh(bool WantFirstPerson)' is a custom Character class
			//USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(false);

			// hide visibility?
			//Mesh1P->SetHiddenInGame(false);
			Mesh3P->SetHiddenInGame(false);

			// call Weapon method to attach to pawn where you want
			//Mesh1P->AttachTo(PawnMesh1p, AttachPoint);
			//Mesh3P->AttachTo(PawnMesh3p, AttachPoint);
		}else{
			//USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			//USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			//UseWeaponMesh->AttachTo(UsePawnMesh, AttachPoint);
			//UseWeaponMesh->SetHiddenInGame(false);
		}
	}
	
}

void AFMWeapon::DetachMeshFromPawn(){
	// not worried about 1st person right now
	//Mesh1P->DetachFromParent();
	//Mesh1P->SetHiddenInGame(true);

	// USceneComponenet
	// "Detach this component from whatever it is attached to"
	Mesh3P->DetachFromParent();

	// USceneComponenet
	// "Changes the value of HiddenGame"
	// [Whether to completely hide the primitive in the game; if true, the primitive is not drawn, does not cast a shadow, and does not affect voxel lighting.]
	Mesh3P->SetHiddenInGame(true);
	
}
