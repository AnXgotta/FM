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
	int32 StaminaCost;

	//  total uses (Combos/ammo) 
	UPROPERTY(EditDefaultsOnly, Category = Usage)
		int32 StaminaCostCharging;

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
		StaminaCost = 25;
		StaminaCostCharging = 5;
		timeBetweenUses = 1.0f;
		bIsChargable = false;
		maxChargeValue = 3.0f;
	}

};

USTRUCT()
struct FWeaponAnim{
	GENERATED_USTRUCT_BODY()

	// animation played on pawn (1st person view) 
	//UPROPERTY(EditDefaultsOnly, Category = Animation)
	//UAnimMontage* Pawn1P;

	// animation played on pawn (3rd person view) 
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn3P;
};


 
UCLASS()
class AFMWeapon : public AActor
{
	GENERATED_UCLASS_BODY()

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
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Cooldown)
		uint32 bPendingCooldown : 1;

	// is weapon currently equipped? 
	uint32 bIsEquipped : 1;

	// is equip animation playing? 
	uint32 bPendingEquip : 1;

	// weapon is being equipped by owner pawn 
	virtual void OnEquip();

	// weapon is now equipped by owner pawn 
	virtual void OnEquipFinished();

	// weapon is holstered by owner pawn 
	virtual void OnUnEquip();

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

	// [local + server] stop weapon use 
	//virtual void StopUseWeapon();

	// [all] start weapon cooldown 
	virtual void StartCooldown(bool bFromReplication = false);

	// [local + server] interrupt weapon cooldown 
	virtual void StopCooldown();

	// [server] performs actual cooldown 
	//virtual void CooldownWeapon();

	// trigger cooldown from server 
	UFUNCTION(reliable, client)
		void ClientStartCooldown();


	//////////////////////////////////////////////////////////////////////////
	// INPUT - SERVER

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartUseWeaponPressed();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartUseWeaponReleased();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStopUseWeapon();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartCooldown();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStopCooldown();


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

	/** [local + server] weapon usage started */
	virtual void OnUseWeaponStarted();

	/** [local + server] weapon usage finished */
	virtual void OnUseWeaponFinished();

	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
		void ServerHandleFiring();

	/** [local + server] handle weapon fire */
	void HandleFiring();


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

	// [server] fire & update ammo 
	UFUNCTION(reliable, server, WithValidation)
		void ServerHandleUseWeapon();

	// [local + server] handle weapon fire 
	void HandleUseWeapon();

	// Called in network play to do the cosmetic fx for firing 
	virtual void SimulateWeaponUse();

	// Called in network play to stop cosmetic fx (e.g. for a looping shot). 
	virtual void StopSimulatingWeaponUse();


	//////////////////////////////////////////////////////////////////////////
	// REPLICATION AND EFFECTS

	UFUNCTION()
		void OnRep_MyPawn();

	UFUNCTION()
		void OnRep_Cooldown();


	//////////////////////////////////////////////////////////////////////////
	// STAMINA

	// Use stamina for usage or charging 
	void UseStamina(bool bIsCharging);


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
