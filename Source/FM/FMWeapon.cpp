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


	// XXX: START WEAPON CLASS VARIABLES ####################################

	CurrentState = EWeaponState::Idle;
	currentSwingID = 0.0f;

	bWantsToUse = false;
	bWantsToCombo = false;
	bIsInComboWindow = false;
	bIsEquipped = false;
	bPendingEquip = false;
	bPendingUnEquip = false;
	bWantsToCharge = false;
	bIsCharging = false;
	chargeValue = 0.0f;

	// XXX: END WEAPON CLASS VARIABLES #######################################

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

	// if charging
	if (bIsCharging){
		// if not enough stamina or fully charged, then stop charging 
		if (chargeValue == WeaponConfig.maxChargeValue || (MyPawn && !MyPawn->CheckIfStaminaGreaterThan((WeaponConfig.StaminaCostCharging * DeltaSeconds) + 1.0f))){
			// do nothing
		}else{
			// determine new charge value
			chargeValue = FMath::Min(chargeValue + DeltaSeconds, WeaponConfig.maxChargeValue);
			// use stamina for this tick
			UseStamina(true, DeltaSeconds);
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
	CurrentState = NewState;

	// initial swing from LMB to begin charging
	if (PrevState == EWeaponState::Idle && NewState == EWeaponState::Charging){
		bWantsToCharge = false;
		bIsCharging = true;
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("Weapon | Idle -> Charging"));
		}
	}

	// use weapon after charging LMB
	if (PrevState == EWeaponState::Charging && NewState == EWeaponState::Using){
		bWantsToUse = false;
		bIsCharging = false;
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("Weapon | Charging -> Using"));
		}

		OnUseWeaponStarted();
	}

	// initial swing from not LMB... no charging
	if (PrevState == EWeaponState::Idle && NewState == EWeaponState::Using){
		bWantsToUse = false;
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("Weapon | Idle -> Using"));
		}

		OnUseWeaponStarted();
	}

	// combo handling
	if (PrevState == EWeaponState::Using && NewState == EWeaponState::Using){
		bWantsToUse = false;
		bWantsToCharge = false; 
		bWantsToCombo = false;

		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("Weapon | Using -> Using"));
		}

		if (bIsInComboWindow){
			bIsInComboWindow = false;
			OnUseWeaponStarted();
		}
		bIsInComboWindow = false;
		
	}

	// back to idle from swing
	if (PrevState == EWeaponState::Using && NewState != EWeaponState::Using){
		bWantsToUse = false;
		bWantsToCharge = false;
		bIsCharging = false;
		bWantsToCombo = false;
		bIsInComboWindow = false;
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("Weapon | Using -> Idle"));
		}
	}

	// no stamina
	if (PrevState == EWeaponState::Idle && NewState == EWeaponState::Idle){
		bWantsToUse = false;
		bWantsToCharge = false;
		bIsCharging = false;
		bWantsToCombo = false;
		bIsInComboWindow = false;
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("Weapon | Idle -> Idle"));
		}
	}
}

void AFMWeapon::DetermineWeaponState(){

	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped){
		if (bWantsToUse){
			if (CanUse()){
				if (WeaponConfig.bIsChargable){
					if (bWantsToCombo){
						NewState = EWeaponState::Using;
					}else{
						if (bWantsToCharge){
							if (CanCharge()){
								NewState = EWeaponState::Charging;
							}else{
								NewState = EWeaponState::Using;
							}
						}else{ // if (bIsCharging)... no other way to get here unless already charging
							NewState = EWeaponState::Using;
						}
					}
				}else{
					NewState = EWeaponState::Using;
				}
			}
		}else if (bPendingUnEquip){
			NewState = EWeaponState::UnEquipping;
		}
	}else if (bPendingEquip){
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
		SetSwingID(1);

		// i hate doing this check here
		// used for combo control to skip state check and unneeded charging on combo
		// saves us a lot of control probs later
		if (CurrentState == EWeaponState::Using){
			return;
		}

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
		// i hate doing this check here
		if (CurrentState == EWeaponState::Using){
			bWantsToCombo = true;
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

void AFMWeapon::StartUseWeaponPressedAlternates(int32 swingID){
	if (Role < ROLE_Authority){
		ServerStartUseWeaponPressedAlternates(swingID);
	}

	if (!bWantsToUse){
		bWantsToUse = true;
		SetSwingID(swingID);
		// never charge alternate swings
		if (WeaponConfig.bIsChargable){
			bWantsToCharge = false;
		}

		DetermineWeaponState();
	}
}

bool AFMWeapon::ServerStartUseWeaponPressedAlternates_Validate(int32 swingID){
	return true;
}

void AFMWeapon::ServerStartUseWeaponPressedAlternates_Implementation(int32 swingID){
	StartUseWeaponPressedAlternates(swingID);
}

void AFMWeapon::SetSwingID(int32 newSwingID){
	int currentSwingIDMod = GetCurrentSwingMod();
	switch (newSwingID){
	case 1:
		if (currentSwingIDMod == 0){
			currentSwingID = 1.1f;
		}
		else if (currentSwingIDMod == 1){
			currentSwingID = 1.2f;
		}
		else if (currentSwingIDMod == 2){
			currentSwingID = 1.3f;
		}else {
			currentSwingID = 0.0f;
		}
		break;
	case 2:
		if (currentSwingIDMod == 0){
			currentSwingID = 2.1f;
		}
		else if (currentSwingIDMod == 1){
			currentSwingID = 2.2f;
		}
		else if (currentSwingIDMod == 2){
			currentSwingID = 2.3f;
		}
		else{
			currentSwingID = 0.0f;
		}
		break;
	case 3:
		if (currentSwingIDMod == 0){
			currentSwingID = 3.1f;
		}
		else if (currentSwingIDMod == 1){
			currentSwingID = 3.2f;
		}
		else if (currentSwingIDMod == 2){
			currentSwingID = 3.3f;
		}
		else {
			currentSwingID = 0.0f;
		}
		break;
	case 4:
		currentSwingID = 4.1;
		break;
	default:
		currentSwingID = 0.0f;
	}
}

int32 AFMWeapon::GetCurrentSwingMod(){
	return (int32)(currentSwingID * 10) % 10;
}


//////////////////////////////////////////////////////////////////////////
// WEAPON USAGE

void AFMWeapon::UseStamina(bool bIsCharging, float DeltaSeconds){
	if (MyPawn){
		if (!bIsCharging){
			MyPawn->UseStamina(WeaponConfig.StaminaCost);
		}else{
			MyPawn->UseStamina(WeaponConfig.StaminaCostCharging * DeltaSeconds);
		}
		
	}	
}

void AFMWeapon::BeginStaminaCooldown(){
	if (MyPawn){
		MyPawn->ResetStaminaCooldown();
	}
}

bool AFMWeapon::CanUse() const {
	bool bCanUse = MyPawn && MyPawn->CanUse();
	bool bEnoughStamina = MyPawn && MyPawn->CheckIfStaminaGreaterThan(WeaponConfig.StaminaCost);
	//bool bStateOKToUse = (CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Using);
	return bCanUse && bEnoughStamina;
}

bool AFMWeapon::CanCharge() const {
	bool bCanUse = MyPawn && MyPawn->CanUse();
	bool bEnoughStamina = MyPawn && MyPawn->CheckIfStaminaGreaterThan(WeaponConfig.StaminaCostCharging);
	//bool bStateOKToUse = (CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Using);
	return bCanUse && bEnoughStamina;
	return false;
}

void AFMWeapon::OnUseWeaponStarted(){
	float totalStaminaUse = WeaponConfig.StaminaCost + (WeaponConfig.StaminaCostCharging + chargeValue);

	if (Role == ROLE_Authority){
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, FString::Printf(TEXT("Weapon: SERVER : UseStart cost %f, CurrentSwingID %f"), totalStaminaUse, currentSwingID));
		}
	}
	else{
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, FString::Printf(TEXT("Weapon: CLIENT : UseStart cost %f, CurrentSwingID %f"), totalStaminaUse, currentSwingID));
		}
	}

	// check if greater than max combo count for this swing type
	if (GetCurrentSwingMod() < 4){
		UseStamina(false, 0.0f);
		UseWeapon();
	}else{
		// outside of combo range... just end it here
		bWantsToUse = false;
	}
}

void AFMWeapon::OnUseWeaponEnded(){
	bWantsToUse = false;
	bWantsToCharge = false;
	bIsCharging = false;
	bWantsToCombo = false;
	bIsInComboWindow = false;
	currentSwingID = 0.0f;
	BeginStaminaCooldown();
	DetermineWeaponState();
}

void AFMWeapon::SetOwningPawn(AFMCharacter* NewOwner){
	if (MyPawn != NewOwner){
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}	
}




/////////////////////////////////////////////////////////////////////////
// REPLICATION & EFFECTS

void AFMWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFMWeapon, MyPawn);
}

void AFMWeapon::OnRep_MyPawn(){
	if (MyPawn){
		OnEnterInventory(MyPawn);
	}else{
		//OnLeaveInventory();
	}
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
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Weapon: OnEquip"));
	}

	if (CurrentState != EWeaponState::Idle){
		// play denied sound
		return;
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
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Weapon: OnEquip Finished "));
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

	if (CurrentState != EWeaponState::Idle){
		// play denied sound
		return;
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
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Weapon: OnUnEquip Finished "));
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
	/*
	if (IsAttachedToPawn()){
		OnUnEquip();
	}
	*/
}

void AFMWeapon::EquipForUse(){

	if (MyPawn){		

			switch (WeaponConfig.WeaponType){
			case EWeaponType::Primary:
				Mesh3P->AttachTo(MyPawn->Mesh, RIGHT_HAND_SOCKET, EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Secondary:
				Mesh3P->AttachTo(MyPawn->Mesh, RIGHT_HAND_SOCKET, EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Tertiary:
				Mesh3P->AttachTo(MyPawn->Mesh, RIGHT_HAND_SOCKET, EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Shield:
				Mesh3P->AttachTo(MyPawn->Mesh, LEFT_HAND_SOCKET, EAttachLocation::SnapToTarget);
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
				Mesh3P->AttachTo(MyPawn->Mesh, BACK_SOCKET_PRIMARY, EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Secondary:
				Mesh3P->AttachTo(MyPawn->Mesh, LEFT_HIP_SOCKET, EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Tertiary:
				Mesh3P->AttachTo(MyPawn->Mesh, RIGHT_HIP_SOCKET, EAttachLocation::SnapToTarget);
				break;
			case EWeaponType::Shield:
				Mesh3P->AttachTo(MyPawn->Mesh, BACK_SOCKET_SHIELD, EAttachLocation::SnapToTarget);
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

void AFMWeapon::LogState(EWeaponState::Type NewState){

	switch (NewState){
	case EWeaponState::Idle:
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Idle"));
		}
		break;
	case EWeaponState::Charging:
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Charging"));
		}
		break;
	case EWeaponState::Using:
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Using"));
		}
		break;
	case EWeaponState::Equipping:
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("Equipping"));
		}
		break;
	case EWeaponState::UnEquipping:
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::White, TEXT("UnEquipping"));
		}
		break;
	}
}