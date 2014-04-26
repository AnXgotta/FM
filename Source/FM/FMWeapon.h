// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "GameFramework/Actor.h"
#include "FMWeapon.generated.h"

UENUM()
namespace EWeaponState{
	enum Type{
		Idle,
		Using,
		Cooldown,
		Equipping,
		UnEquipping,
		Charging,
	};
}

UENUM()
namespace EWeaponType{
	enum Type{
		Primary,
		Secondary,
		Tertiary,
		Shield,
	};
}

USTRUCT()
struct FMWeaponData{

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Type)
	TEnumAsByte<EWeaponType::Type> WeaponType;

	//  total uses (Combos/ammo) 
	UPROPERTY(EditDefaultsOnly, Category = Usage)
	float StaminaCost;

	//  total uses (Combos/ammo) 
	UPROPERTY(EditDefaultsOnly, Category = Usage)
		float StaminaCostCharging;

	//  time between uses 
	UPROPERTY(EditDefaultsOnly, Category = Usage)
	float timeBetweenUses;

	// determines if this weapon has the ability to charge
	UPROPERTY(EditDefaultsOnly, Category = Charging)
		bool bIsChargable;

	// determines if this weapon has the ability to charge
	UPROPERTY(EditDefaultsOnly, Category = Charging)
		float maxChargeValue;

	// defaults 
	FMWeaponData(){
		WeaponType = EWeaponType::Primary;
		StaminaCost = 25.0f;
		StaminaCostCharging = 5.0f;
		timeBetweenUses = 1.0f;
		bIsChargable = false;
		maxChargeValue = 3.0f;
	}

};


 
UCLASS()
class AFMWeapon : public AActor
{
	GENERATED_UCLASS_BODY()

	/////////////////////////////////////////////////////////////////
	// SOCKET NAMES

	const FName RIGHT_HAND_SOCKET = TEXT("RightHandSocket");
	const FName LEFT_HAND_SOCKET = TEXT("LeftHandSocket");
	const FName RIGHT_HIP_SOCKET = TEXT("RightHipSocket");
	const FName LEFT_HIP_SOCKET = TEXT("LeftHipSocket");
	const FName BACK_SOCKET_PRIMARY = TEXT("BackSocket0");
	const FName BACK_SOCKET_SHIELD = TEXT("BackSocket1");

	
	///////////////////////////////////////////////////////////////
	// OVERRIDEN CLASS FUNCTIONS

	// perform initial setup 
	// called after all components of object have been established
	virtual void PostInitializeComponents() OVERRIDE;

	//  Update the weapon (charging)
	virtual void Tick(float DeltaSeconds) OVERRIDE;

	virtual void Destroyed() OVERRIDE;


	//////////////////////////////////////////////////////////////////
	// WEAPON CONFIG/OWNER PAWN

	// weapon data 
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FMWeaponData WeaponConfig;

	// pawn owner of this object
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class AFMCharacter* MyPawn;

	// set the weapon's owning pawn 
	void SetOwningPawn(AFMCharacter* NewOwner);

	// get pawn owner 
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AFMCharacter* GetPawnOwner() const;


	//////////////////////////////////////////////////////
	// STATE

	// current weapon state 
	EWeaponState::Type CurrentState;

	// get current weapon state 
	EWeaponState::Type GetCurrentState() const;

	// update weapon state
	void SetWeaponState(EWeaponState::Type NewState);

	// determine current weapon state 
	void DetermineWeaponState();

	//////////////////////////////////////////////////////
	// MESH

	// weapon mesh: 3rd person view 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	TSubobjectPtr<USkeletalMeshComponent> Mesh3P;

	// get weapon mesh (needs pawn owner to determine variant) 
	//USkeletalMeshComponent* GetWeaponMesh() const;


	//////////////////////////////////////////////////////
	// INVENTORY

	// how much time weapon needs to be equipped 
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	float EquipDuration;

	// is reload animation playing? 
	//UPROPERTY(Transient, ReplicatedUsing = OnRep_Cooldown)
		uint32 bPendingCooldown : 1;

	// is weapon currently equipped? 
	uint32 bIsEquipped : 1;

	// is equip animation playing? 
	uint32 bPendingEquip : 1;

	// is unequip animation playing? 
	uint32 bPendingUnEquip : 1;

	// weapon is being equipped by owner pawn 
	virtual void OnEquip();

	// weapon is now equipped by owner pawn 
	virtual void OnEquipFinished();

	// weapon is holstered by owner pawn 
	virtual void OnUnEquip();

	// weapon is now unequipped by owner pawn 
	virtual void OnUnEquipFinished();

	// [server] weapon was added to pawn's inventory 
	virtual void OnEnterInventory(AFMCharacter* NewOwner);

	// [server] weapon was removed from pawn's inventory 
	virtual void OnLeaveInventory();

	// check if it's currently equipped 
	bool IsEquipped() const;

	// check if mesh is already attached 
	bool IsAttachedToPawn() const;

	// attaches weapon mesh to pawn's mesh 
	void EquipForUse();

	// detaches weapon mesh from pawn 
	void UnequipFromUse();

	// gets the duration of equipping weapon
	float GetEquipDuration() const;


	//////////////////////////////////////////////////////////////////////////
	// INPUT

	// [local + server] start weapon use 
	virtual void StartUseWeaponPressed();

	// [local + server] start weapon use
	virtual void StartUseWeaponReleased();


	//////////////////////////////////////////////////////////////////////////
	// INPUT - SERVER

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartUseWeaponPressed();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartUseWeaponReleased();



	//////////////////////////////////////////////////////////////////////////
	// USAGE CONTROL

	// check if weapon can use 
	bool CanUse() const;

	// check if weapon can charge
	bool CanCharge() const;

	// check if weapon can be cooled 
	bool CanCooldown() const;


	/////////////////////////////////////////////////////////////////////////
	// HANDLE USAGE

	// [local + server] weapon usage started
	virtual void OnUseWeaponStarted();

	// [local + server] weapon usage ended
	virtual void OnUseWeaponEnded();

	// is weapon charge active?
	bool bWantsToCharge;

	// do the charging
	bool bIsCharging;

	// get charging value
	float chargeValue;

	// is weapon fire active? 
	uint32 bWantsToUse : 1;

	// [local] weapon specific fire implementation 
	virtual void UseWeapon() PURE_VIRTUAL(AFMCharacter::UseWeapon, );

	//////////////////////////////////////////////////////////////////////////
	// REPLICATION AND EFFECTS

	UFUNCTION()
		void OnRep_MyPawn();

	

	//////////////////////////////////////////////////////////////////////////
	// STAMINA

	// Use stamina for usage or charging 
	void UseStamina(bool bIsCharging, float DeltaSeconds);

	// reset character cooldown for stamina regeneration
	void BeginStaminaCooldown();

	//////////////////////////////////////////////////////////////////////////
	// NOT IMPLEMENTED

	// HUD THINGS
	/*
	// icon displayed on the HUD when weapon is equipped as primary 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon PrimaryIcon;

	// icon displayed on the HUD when weapon is secondary 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon SecondaryIcon;

	// icon used to draw current uses (left side) 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon PrimaryClipIcon;


	// how many icons to draw per clip 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		float UsageIconsCount;

	// defines spacing between primary usage icons (left side) 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		int32 PrimaryusageIconOffset;

	// defines spacing between secondary usage icons (left side) 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		int32 SecondaryusageIconOffset;

	// crosshair parts icons (left, top, right, bottom and center) 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon Crosshair[5];

	// crosshair parts icons when targeting (left, top, right, bottom and center) 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon AimingCrosshair[5];

	// only use red colored center part of aiming crosshair 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		bool UseLaserDot;

	// false = default crosshair 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		bool UseCustomCrosshair;

	// false = use custom one if set, otherwise default crosshair 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		bool UseCustomAimingCrosshair;

	// true - crosshair will not be shown unless aiming with the weapon 
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		bool bHideCrosshairWhileNotAiming;

*/

	// ANIMATIONS AND EFFECTS
	/*

	// weapon mesh: 1st person view 
	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		//TSubobjectPtr<USkeletalMeshComponent> Mesh1P;

	// firing audio (bLoopedFireSound set) 
//	UPROPERTY(Transient)
//		UAudioComponent* FireAC;

	// name of bone/socket for muzzle in weapon mesh 
//	UPROPERTY(EditDefaultsOnly, Category = Effects)
//		FName MuzzleAttachPoint;

	// FX for muzzle flash 
//	UPROPERTY(EditDefaultsOnly, Category = Effects)
//		UParticleSystem* MuzzleFX;

	// spawned component for muzzle FX 
//	UPROPERTY(Transient)
//		UParticleSystemComponent* MuzzlePSC;

	// spawned component for second muzzle FX (Needed for split screen) 
//	UPROPERTY(Transient)
//		UParticleSystemComponent* MuzzlePSCSecondary;

	// camera shake on used 
//	UPROPERTY(EditDefaultsOnly, Category = Effects)
//		TSubclassOf<UCameraShake> UsedCameraShake;

	// force feedback effect to play when the weapon is used 
//	UPROPERTY(EditDefaultsOnly, Category = Effects)
//		UForceFeedbackEffect *UsedForceFeedback;

	// single Used sound (bLoopedFireSound not set) 
//	UPROPERTY(EditDefaultsOnly, Category = Sound)
//		USoundCue* FireSound;

	// out of stamina sound 
//	UPROPERTY(EditDefaultsOnly, Category = Sound)
//		USoundCue* OutOfAmmoSound;

	// reload animations 
//	UPROPERTY(EditDefaultsOnly, Category = Animation)
//		FWeaponAnim ReloadAnim;

	// equip sound 
//	UPROPERTY(EditDefaultsOnly, Category = Sound)
//		USoundCue* EquipSound;

	// equip animations 
//	UPROPERTY(EditDefaultsOnly, Category = Animation)
//		FWeaponAnim EquipAnim;

	// fire animations 
//	UPROPERTY(EditDefaultsOnly, Category = Animation)
//		FWeaponAnim FireAnim;

		// is fire sound looped? 
//	UPROPERTY(EditDefaultsOnly, Category = Sound)
//		uint32 bLoopedUseSound : 1;

	// is fire animation playing? 
//	uint32 bPlayingUseAnim : 1;

	*/

	// WEAPON USAGE HELPERS [AIM, ANIMATIONS?]
	/*
	// play weapon sounds 
	//UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	// play weapon animations 
	//float PlayWeaponAnimation(const FWeaponAnim& Animation);

	// stop playing weapon animations 
	//void StopWeaponAnimation(const FWeaponAnim& Animation);

	// Get the aim of the weapon, allowing for adjustments to be made by the weapon 
	//virtual FVector GetAdjustedAim() const;

	// Get the aim of the camera 
	//FVector GetCameraAim() const;

	// get the originating location for camera damage 
	//FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	// get the muzzle location of the weapon 
	//FVector GetMuzzleLocation() const;

	// get direction of weapon's muzzle 
	//FVector GetMuzzleDirection() const;

	// find hit 
	//FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;
	*/

};
