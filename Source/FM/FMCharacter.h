// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "FMTypes.h"
#include "GameFramework/Character.h"
#include "FMCharacter.generated.h"


	UCLASS()
class AFMCharacter : public ACharacter
{

	GENERATED_UCLASS_BODY()

	virtual void BeginPlay() OVERRIDE;

	UPROPERTY(VisibleDefaultsOnly, Category = CameraBoom)
	TSubobjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = FollowCamera)
		TSubobjectPtr<UCameraComponent> FollowCamera;

	// projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AFMProjectile> ProjectileClass;

	//  spawn inventory, setup initial variables
	virtual void PostInitializeComponents() OVERRIDE;


	//////////////////////////////////////////////////////////////////////////
	// DAMAGE AND HEALTH AND STAMINA

	// Current health of the Pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
	float currentHealth;

	// Current Max health of the Pawn
	// May want to change replication -- not monitored regularly
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
	float currentMaxHealth;

	// Default Max health of the Pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float defaultMaxHealth;

	// Current stamina of the Pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Stamina)
		float currentStamina;

	// Current Max stamina of the Pawn
	// May want to change replication -- not monitored regularly
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Stamina)
		float currentMaxStamina;

	// Default Max stamina of the Pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stamina)
		float defaultMaxStamina;

	
	//GET health and stamina

	// get default max health
	int32 GetDefaultMaxHealth() const;

	// get current max health
	int32 GetCurrentMaxHealth() const;
	
	// get default max stamina
	int32 GetDefaultMaxStamina() const;

	// get current max stamina
	int32 GetCurrentMaxStamina() const;

protected:

	// when low health effects should start
	float LowHealthPercentage;

public:
	//////////////////////////////////////////////////////////////////////////
	// INPUT HANDLERS handlers

	//  setup pawn specific input handlers 
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

	// sets run flag when key pressed
	UFUNCTION()
		void OnStartRunning();

	// clears run flag when key released
	UFUNCTION()
	void OnStopRunning();

	bool IsRunning() const;

	// [server + local] change sprint state
	void SetRunning(bool bNewSprint, bool bToggle);

	/** get the modifier value for running speed */
	UFUNCTION(BlueprintCallable, Category = Pawn)
		float GetRunningSpeedModifier() const;

	UFUNCTION(reliable, server, WithValidation)
		void ServerSetRunning(bool bNewSprint, bool bToggle);

	UFUNCTION()
		void OnFire0();



protected:
	/////////////////////////////////////////////////////////////////////////
	// INVENTORY

	//  default inventory list
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class AFMWeapon> > DefaultInventoryClasses;

	//  weapons in inventory
	UPROPERTY(Transient, Replicated)
	TArray<class AFMWeapon*> Inventory;

	//  [server] spawns default inventory
	void SpawnDefaultInventory();

	//[server] add weapon to inventory
	void AddWeapon(class AFMWeapon* Weapon);

	// [server] remove weapon from inventory
	void RemoveWeapon(class AFMWeapon* Weapon);

	// Find in inventory
	class AFMWeapon* FindWeapon(TSubclassOf<class AFMWeapon> WeaponClass);

	//[server + local] equips weapon from inventory
	void EquipWeapon(class AFMWeapon* Weapon);

	//  updates current weapon
	void SetCurrentWeapon(class AFMWeapon* NewWeapon, class AFMWeapon* LastWeapon = NULL);

	//  current weapon rep handler
	UFUNCTION()
	void OnRep_CurrentWeapon(class AFMWeapon* LastWeapon);

	//  [server] remove all weapons from inventory and destroy them
	void DestroyInventory();

	//  equip weapon
	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(class AFMWeapon* NewWeapon);


	///////////////////////////////////////////////////////////////////////////
	// ACHARACTER OVERRIDES

	//  Update the character. (Running, health etc).
	virtual void Tick(float DeltaSeconds) OVERRIDE;

	//  cleanup inventory
	virtual void Destroyed() OVERRIDE;

	//  update mesh for first person view
	virtual void PawnClientRestart() OVERRIDE;

	//  [server] perform PlayerState related setup
	virtual void PossessedBy(class AController* C) OVERRIDE;

	//  [client] perform PlayerState related setup
	virtual void OnRep_PlayerState() OVERRIDE;


	//////////////////////////////////////////////////////////////////////////
	// ANIMATIONS

	//  play anim montage
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) OVERRIDE;

	//  stop playing montage
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage) OVERRIDE;

	//  stop playing all montages
	void StopAllAnimMontages();


	//////////////////////////////////////////////////////////////////////////
	// WEAPON USAGE
	
	//  [local] starts weapon Use
	void StartWeaponUse();

	//  [local] stops weapon Use
	void StopWeaponUse();

	//  check if pawn can Use weapon
	bool CanUse() const;

	//  check if pawn can cooldown weapon
	bool CanCooldown() const;

	//  get firing state
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	bool IsUsingWeapon() const;
	
	protected:
	//  pawn mesh: 1st person view
	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//TSubobjectPtr<USkeletalMeshComponent> Mesh1P;

	//  socket or bone name for attaching weapon mesh
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;

	//  currently equipped weapon
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class AFMWeapon* CurrentWeapon;

	//  current weapon use state
	uint8 bWantsToUseWeapon : 1;

	//  Time at which point the last take hit info for the actor times out and won't be replicated; Used to stop join-in-progress effects all over the screen
	float LastTakeHitTimeTimeout;

	//  Replicate where this pawn was last hit and damaged
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	//  Responsible for cleaning up bodies on clients.
	virtual void TornOff();

	//  handle mesh visibility and updates
	void UpdatePawnMeshes();

	//////////////////////////////////////////////////////////////////////////
	// DAMAGE AND DEATH

	public:

	//  Identifies if pawn is in its dying state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	uint32 bIsDying : 1;

	//  Take damage, handle death
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) OVERRIDE;

	//  Pawn suicide
	virtual void Suicide();

	//  Kill this pawn
	virtual void KilledBy(class APawn* EventInstigator);

	//  Returns True if the pawn can die in the current state
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	//  player pressed Use Weapon action
	void OnStartUseWeapon();

	//  player released Use Weapon action
	void OnStopUseWeapon();

	//  player pressed next weapon action
	//void OnNextWeapon();

	//  player pressed prev weapon action
	//void OnPrevWeapon();


	//Kills pawn.  Server/authority only.
	// @param KillingDamage - Damage amount of the killing blow
	// @param DamageEvent - Damage event of the killing blow
	// @param Killer - Who killed this pawn
	// @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	// @returns true if allowed

	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	// Die when we fall out of the world.
	virtual void FellOutOfWorld(const class UDamageType& dmgType) OVERRIDE;

	//  Called on the actor right before replication occurs
	virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) OVERRIDE;

	protected:
	//  notification when killed, for both the server and client.
	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);

	//  play effects on hit
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	//  switch to ragdoll
	void SetRagdollPhysics();

	//  sets up the replication for taking a hit
	void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser, bool bKilled);

	//  play hit or death on client
	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	// check if pawn is still alive
	bool IsAlive() const;

public:

	//////////////////////////////////////////////////////////////////////////
	// Reading data

	//  get mesh component
	USkeletalMeshComponent* GetPawnMesh() const;

	// Get either first or third person mesh.
	USkeletalMeshComponent* GetSpecifcPawnMesh( bool WantFirstPerson ) const;

	/** modifier for max movement speed */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		float RunningSpeedModifier;

	/** current running state */
	UPROPERTY(Transient, Replicated)
	uint8 bWantsToRun : 1;

	/** from gamepad running is toggled */
	uint8 bWantsToRunToggled : 1;

	/** current firing state */
	uint8 bWantsToFire : 1;

	/** handles sounds for running */
	void UpdateRunSounds(bool bNewRunning);

	//  get currently equipped weapon
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class AFMWeapon* GetWeapon() const;

	//  get weapon attach point
	FName GetWeaponAttachPoint() const;

	//  get total number of inventory items
	int32 GetInventoryCount() const;

	// get weapon from inventory at index.Index validity is not checked.
	class AFMWeapon* GetInventoryWeapon(int32 index) const;

	/** material instances for setting team color in mesh (3rd person view) */
//	UPROPERTY(Transient)
//		TArray<UMaterialInstanceDynamic*> MeshMIDs;

	/** animation played on death */
//	UPROPERTY(EditDefaultsOnly, Category = Animation)
//		UAnimMontage* DeathAnim;

	/** sound played on death, local player only */
//	UPROPERTY(EditDefaultsOnly, Category = Pawn)
//		USoundCue* DeathSound;

	/** effect played on respawn */
//	UPROPERTY(EditDefaultsOnly, Category = Pawn)
//		UParticleSystem* RespawnFX;

	/** sound played on respawn */
//	UPROPERTY(EditDefaultsOnly, Category = Pawn)
//		USoundCue* RespawnSound;

	/** sound played when health is low */
//	UPROPERTY(EditDefaultsOnly, Category = Pawn)
//		USoundCue* LowHealthSound;

	/** sound played when running */
//	UPROPERTY(EditDefaultsOnly, Category = Pawn)
//		USoundCue* RunLoopSound;

	/** sound played when stop running */
//	UPROPERTY(EditDefaultsOnly, Category = Pawn)
//		USoundCue* RunStopSound;

	/** used to manipulate with run loop sound */
//	UPROPERTY()
//		UAudioComponent* RunLoopAC;

	/** hook to looped low health sound used to stop/adjust volume */
//	UPROPERTY()
//		UAudioComponent* LowHealthWarningPlayer;

	

};