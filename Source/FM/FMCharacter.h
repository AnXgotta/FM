// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "FMTypes.h"
#include "FMWeapon.h"
#include "GameFramework/Character.h"
#include "FMCharacter.generated.h"


/**
 * 
 */
UCLASS()
class AFMCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() OVERRIDE;

	/** Update the character. (Running, health etc). */
	//virtual void Tick(float DeltaSeconds) OVERRIDE;

	/** cleanup inventory */
	virtual void Destroyed() OVERRIDE;

	/** update mesh for first person view */
	virtual void PawnClientRestart() OVERRIDE;

	/** [server] perform PlayerState related setup */
	virtual void PossessedBy(class AController* C) OVERRIDE;

	/** [client] perform PlayerState related setup */
	virtual void OnRep_PlayerState() OVERRIDE;

	UPROPERTY(VisibleDefaultsOnly, Category = CameraBoom)
	TSubobjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = FollowCamera)
	TSubobjectPtr<UCameraComponent> FollowCamera;



	//////////////////////////////////////////////////////////////////////////
	// Animations

	/** play anim montage */
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) OVERRIDE;

	/** stop playing montage */
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) OVERRIDE;

	/** stop playing all montages */
	void StopAllAnimMontages();

	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/**
	* [server] add weapon to inventory
	*
	* @param Weapon	Weapon to add.
	*/
	void AddWeapon(class AFMWeapon* Weapon);

	/**
	* [server] remove weapon from inventory
	*
	* @param Weapon	Weapon to remove.
	*/
	void RemoveWeapon(class AFMWeapon* Weapon);

	/**
	* Find in inventory
	*
	* @param WeaponClass	Class of weapon to find.
	*/
	class AFMWeapon* FindWeapon(TSubclassOf<class AFMWeapon> WeaponClass);

	/**
	* [server + local] equips weapon from inventory
	*
	* @param Weapon	Weapon to equip
	*/
	void EquipWeapon(class AFMWeapon* Weapon);

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] starts weapon Use */
	void StartWeaponUse();

	/** [local] stops weapon Use */
	void StopWeaponUse();

	/** check if pawn can Use weapon */
	bool CanUse() const;

	/** check if pawn can cooldown weapon */
	bool CanCooldown() const;



protected:
	

	// projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFMProjectile> ProjectileClass;


	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TSubobjectPtr<USkeletalMeshComponent> Mesh1P;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		FName WeaponAttachPoint;

	/** default inventory list */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class AFMWeapon> > DefaultInventoryClasses;

	/** weapons in inventory */
	UPROPERTY(Transient, Replicated)
		TArray<class AFMWeapon*> Inventory;

	/** currently equipped weapon */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class AFMWeapon* CurrentWeapon;

	/** Replicate where this pawn was last hit and damaged */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	/** Time at which point the last take hit info for the actor times out and won't be replicated; Used to stop join-in-progress effects all over the screen */
	float LastTakeHitTimeTimeout;

	/** current weapon use state */
	uint8 bWantsToUseWeapon : 1;

	/** Responsible for cleaning up bodies on clients. */
	virtual void TornOff();

	/** handle mesh visibility and updates */
	void UpdatePawnMeshes();

	virtual void BeginPlay() OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// Damage & death

public:

	/** Identifies if pawn is in its dying state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
		uint32 bIsDying : 1;

	// Current health of the Pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float Health;

	/** Take damage, handle death */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) OVERRIDE;

	/** Pawn suicide */
	virtual void Suicide();

	/** Kill this pawn */
	virtual void KilledBy(class APawn* EventInstigator);

	/** Returns True if the pawn can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;


	//////////////////////////////////////////////////////////////////////////
	// Input handlers

	/** setup pawn specific input handlers */
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) OVERRIDE;
	
	// handle moving forward/backward
	UFUNCTION()
	void MoveForward(float Value);

	// handle strafing left/right
	UFUNCTION()
	void MoveRight(float Value);

	// sets jump flag when key pressed
	UFUNCTION()
	void OnStartJump();

	// clears jump flag when key released
	UFUNCTION()
	void OnStopJump();

	UFUNCTION()
		void OnFire0();

	/** player pressed Use Weapon action */
	void OnStartUseWeapon();

	/** player released Use Weapon action */
	void OnStopUseWeapon();

	/** player pressed next weapon action */
	void OnNextWeapon();

	/** player pressed prev weapon action */
	void OnPrevWeapon();

	/**
	* Kills pawn.  Server/authority only.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	// Die when we fall out of the world.
	virtual void FellOutOfWorld(const class UDamageType& dmgType) OVERRIDE;

	/** Called on the actor right before replication occurs */
	virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) OVERRIDE;
protected:
	/** notification when killed, for both the server and client. */
	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);

	/** play effects on hit */
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	/** switch to ragdoll */
	void SetRagdollPhysics();

	/** sets up the replication for taking a hit */
	void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser, bool bKilled);

	/** play hit or death on client */
	UFUNCTION()
		void OnRep_LastTakeHitInfo();

	//////////////////////////////////////////////////////////////////////////
	// Inventory

	/** updates current weapon */
	void SetCurrentWeapon(class AFMWeapon* NewWeapon, class AFMWeapon* LastWeapon = NULL);

	/** current weapon rep handler */
	UFUNCTION()
		void OnRep_CurrentWeapon(class AFMWeapon* LastWeapon);

	/** [server] spawns default inventory */
	void SpawnDefaultInventory();

	/** [server] remove all weapons from inventory and destroy them */
	void DestroyInventory();

	/** equip weapon */
	UFUNCTION(reliable, server, WithValidation)
		void ServerEquipWeapon(class AFMWeapon* NewWeapon);

	//////////////////////////////////////////////////////////////////////////
	// Reading data

	/** get mesh component */
	USkeletalMeshComponent* GetPawnMesh() const;

	/** get currently equipped weapon */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AFMWeapon* GetWeapon() const;

	/** get weapon attach point */
	FName GetWeaponAttachPoint() const;

	/** get total number of inventory items */
	int32 GetInventoryCount() const;

	/**
	* get weapon from inventory at index. Index validity is not checked.
	*
	* @param Index Inventory index
	*/
	class AFMWeapon* GetInventoryWeapon(int32 index) const;

	/** get firing state */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		bool IsUsingWeapon() const;

};
