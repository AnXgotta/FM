// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMWeapon.h"


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


	//Mesh3P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));

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

	// From Up the inherited hierarchy, USceneComponent
	// "What we are currently attached to."
	//Mesh3P->AttachParent = Mesh1P;



	// XXX: START WEAPON CLASS VARIABLES ####################################

	//bIsEquipped = false;
	//bWantsToUse = false;
	//bPendingCooldown = false;
	//bPendingEquip = false;
	//CurrentState = EWeaponState::Idle;

	//StaminaCost = 25.0f;
	//LastFireTime = 0.0f;

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

/*

void AFMWeapon::PostInitializeComponents(){
	Super::PostInitializeComponents();
	
	if (WeaponConfig.InitialClips > 0)
	{
		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
		CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;
	}

	DetachMeshFromPawn();
	
}
*/
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
/*
void AFMWeapon::UseStamina(){
	
	if (!HasInfiniteAmmo())
	{
		CurrentAmmoInClip--;
	}

	if (!HasInfiniteAmmo() && !HasInfiniteClip())
	{
		CurrentAmmo--;
	}

	AShooterAIController* BotAI = MyPawn ? Cast<AShooterAIController>(MyPawn->GetController()) : NULL;
	if (BotAI)
	{
		BotAI->CheckAmmo(this);
	}
	
}
*/
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
/*
void AFMWeapon::SetWeaponState(EWeaponState::Type NewState){
	
	const EWeaponState::Type PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
	
}
*/
/*
void AFMWeapon::DetermineWeaponState(){
	
	EWeaponState::Type NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bPendingReload)
		{
			if (CanReload() == false)
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}
		else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
	
}
*/
/*
void AFMWeapon::SetOwningPawn(AFMCharacter* NewOwner){
	
	if (MyPawn != NewOwner){
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}
	
}
*/
/////////////////////////////////////////////////////////////////////////
// Replication & effects
/*
void AFMWeapon::OnRep_MyPawn(){
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
}
*/
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
void AFMWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AShooterWeapon, MyPawn);

	DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AShooterWeapon, CurrentAmmoInClip, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(AShooterWeapon, BurstCounter, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShooterWeapon, bPendingReload, COND_SkipOwner);
	
}
*/
/*
USkeletalMeshComponent* AFMWeapon::GetWeaponMesh() const
{
	return (MyPawn != NULL && MyPawn->IsFirstPerson()) ? Mesh1P : Mesh3P;
}
*/
/*
class AFMCharacter* AFMWeapon::GetPawnOwner() const {
	return MyPawn;
}
*/
/*
bool AFMWeapon::IsEquipped() const {
	return bIsEquipped;
}
*/
/*
bool AFMWeapon::IsAttachedToPawn() const {
	return bIsEquipped || bPendingEquip;
}
*/
/*
EWeaponState::Type AFMWeapon::GetCurrentState() const{
	return CurrentState;
}
*/
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
/*
void AFMWeapon::OnEquip(){
	
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	float Duration = PlayWeaponAnimation(EquipAnim);
	if (Duration <= 0.0f)
	{
		// failsafe
		Duration = 0.5f;
	}
	EquipStartedTime = GetWorld()->GetTimeSeconds();
	EquipDuration = Duration;

	GetWorldTimerManager().SetTimer(this, &AShooterWeapon::OnEquipFinished, Duration, false);

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		PlayWeaponSound(EquipSound);
	}
	
}
*/
/*
void AFMWeapon::OnEquipFinished(){
	
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	if (MyPawn)
	{
		// try to reload empty clip
		if (MyPawn->IsLocallyControlled() &&
			CurrentAmmoInClip <= 0 &&
			CanReload())
		{
			StartReload();
		}
	}

	DetermineWeaponState();
	
}
*/
/*
void AFMWeapon::OnUnEquip(){
	
	DetachMeshFromPawn();
	bIsEquipped = false;
	StopFire();

	if (bPendingReload)
	{
		StopWeaponAnimation(ReloadAnim);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(this, &AShooterWeapon::StopReload);
		GetWorldTimerManager().ClearTimer(this, &AShooterWeapon::ReloadWeapon);
	}

	if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(this, &AShooterWeapon::OnEquipFinished);
	}

	DetermineWeaponState();
	
}
*/
/*
void AFMWeapon::OnEnterInventory(AFMCharacter* NewOwner){
	
	SetOwningPawn(NewOwner);
	
}
*/
/*
void AFMWeapon::OnLeaveInventory(){
	
	if (Role == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}

	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

}
*/
/*
void AFMWeapon::AttachMeshToPawn(){
	
	if (MyPawn)
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		if (MyPawn->IsLocallyControlled() == true)
		{
			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecifcPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecifcPawnMesh(false);
			//Mesh1P->SetHiddenInGame(false);
			Mesh3P->SetHiddenInGame(false);
			//Mesh1P->AttachTo(PawnMesh1p, AttachPoint);
			Mesh3P->AttachTo(PawnMesh3p, AttachPoint);
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachTo(UsePawnMesh, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
	
}
*/
/*
void AFMWeapon::DetachMeshFromPawn(){
	
	//Mesh1P->DetachFromParent();
	//Mesh1P->SetHiddenInGame(true);

	Mesh3P->DetachFromParent();
	Mesh3P->SetHiddenInGame(true);
	
}
*/