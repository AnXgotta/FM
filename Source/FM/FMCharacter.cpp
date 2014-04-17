// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMCharacter.h"
#include "FMProjectile.h"


AFMCharacter::AFMCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Configure character movement
	CharacterMovement->JumpZVelocity = 200.0f;
	CharacterMovement->AirControl = 0.2f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // BaseEyeHeight = FVector(64.0f, 64.0f, 64.0f)
	CameraBoom->TargetArmLength = 100.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUseControllerViewRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUseControllerViewRotation = false; // Camera does not rotate relative to arm
	

}

void AFMCharacter::BeginPlay(){
	Super::BeginPlay();

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Character: BeginPlay Called"));
	}

}

void AFMCharacter::PostInitializeComponents(){
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
//		Health = GetMaxHealth();
		SpawnDefaultInventory();
	}
	/*
	// set initial mesh visibility (3rd person view)
	UpdatePawnMeshes();

	// create material instance for setting team colors (3rd person view)
	for (int32 iMat = 0; iMat < Mesh->GetNumMaterials(); iMat++)
	{
		MeshMIDs.Add(Mesh->CreateAndSetMaterialInstanceDynamic(iMat));
	}

	// play respawn effects
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (RespawnFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, RespawnFX, GetActorLocation(), GetActorRotation());
		}

		if (RespawnSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());
		}
	}
	*/
}

void AFMCharacter::Destroyed(){
	Super::Destroyed();
	DestroyInventory();
}

void AFMCharacter::PawnClientRestart(){
	Super::PawnClientRestart();

	// switch mesh to 1st person view
//	UpdatePawnMeshes();

	// reattach weapon if needed
	SetCurrentWeapon(CurrentWeapon);

	// set team colors for 1st person view
//	UMaterialInstanceDynamic* Mesh1PMID = Mesh1P->CreateAndSetMaterialInstanceDynamic(0);
//	UpdateTeamColors(Mesh1PMID);
}

void AFMCharacter::PossessedBy(class AController* InController){
	Super::PossessedBy(InController);

	// [server] as soon as PlayerState is assigned, set team colors of this pawn for local player
	//UpdateTeamColorsAllMIDs();
}

void AFMCharacter::OnRep_PlayerState(){
	Super::OnRep_PlayerState();

/*	// [client] as soon as PlayerState is assigned, set team colors of this pawn for local player
	if (PlayerState != NULL)
	{
		UpdateTeamColorsAllMIDs();
	}
	*/
}

//////////////////////////////////////////////////////////////////////////
// Animations

float AFMCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName){
	/*
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
	{
		return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}
	*/
	return 0.0f;
}

void AFMCharacter::StopAnimMontage(class UAnimMontage* AnimMontage){
	/*
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance &&
		UseMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage))
	{
		UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOutTime);
	}
	*/
}

void AFMCharacter::StopAllAnimMontages(){
	/*
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (UseMesh && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
	*/
}



//////////////////////////////////////////////////////////////////////////
// Damage & death

void AFMCharacter::FellOutOfWorld(const class UDamageType& dmgType){
	Die(Health, FDamageEvent(dmgType.GetClass()), NULL, NULL);
}

void AFMCharacter::Suicide(){
	KilledBy(this);
}

void AFMCharacter::KilledBy(APawn* EventInstigator){
	if (Role == ROLE_Authority && !bIsDying)
	{
		AController* Killer = NULL;
		if (EventInstigator != NULL)
		{
			Killer = EventInstigator->Controller;
			LastHitBy = NULL;
		}

		Die(Health, FDamageEvent(UDamageType::StaticClass()), Killer, NULL);
	}
}


float AFMCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser){
	//AFMPlayerController* MyPC = Cast<AFMPlayerController>(Controller);

	if (Health <= 0.f)
	{
		return 0.f;
	}

/*	// Modify based on game rules.
	AShooterGameMode* const Game = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}
	
	return ActualDamage;
	*/
	return 0.0f;
}


bool AFMCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const {
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode() == NULL
		|| GetWorld()->GetAuthGameMode()->bLevelChange)	// level transition occurring
	{
		return false;
	}

	return true;
}


bool AFMCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser){
	/*
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<AShooterGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	NetUpdateFrequency = GetDefault<AShooterCharacter>()->NetUpdateFrequency;
	CharacterMovement->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	*/
	return true;
}


void AFMCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser){
	/*
	if (bIsDying)
	{
		return;
	}

	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	if (Role == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

		// play the force feedback effect on the client player controller
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UShooterDamageType *DamageType = Cast<UShooterDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->KilledForceFeedback)
			{
				PC->ClientPlayForceFeedback(DamageType->KilledForceFeedback, false, "Damage");
			}
		}
	}

	// cannot use IsLocallyControlled here, because even local client's controller may be NULL here
	if (GetNetMode() != NM_DedicatedServer && DeathSound && Mesh1P && Mesh1P->IsVisible())
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	// remove all weapons
	DestroyInventory();

	// switch back to 3rd person view
	UpdatePawnMeshes();

	DetachFromControllerPendingDestroy();
	StopAllAnimMontages();

	if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
	{
		LowHealthWarningPlayer->Stop();
	}

	if (RunLoopAC)
	{
		RunLoopAC->Stop();
	}

	// disable collisions on capsule
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (Mesh)
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		Mesh->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	// Death anim
	float DeathAnimDuration = PlayAnimMontage(DeathAnim);

	// Ragdoll
	if (DeathAnimDuration > 0.f)
	{
		GetWorldTimerManager().SetTimer(this, &AShooterCharacter::SetRagdollPhysics, FMath::Min(0.1f, DeathAnimDuration), false);
	}
	else
	{
		SetRagdollPhysics();
	}
	*/
}


void AFMCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser){
	/*
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);

		// play the force feedback effect on the client player controller
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass)
		{
			UShooterDamageType *DamageType = Cast<UShooterDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->HitForceFeedback)
			{
				PC->ClientPlayForceFeedback(DamageType->HitForceFeedback, false, "Damage");
			}
		}
	}

	if (DamageTaken > 0.f)
	{
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}

	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	AShooterHUD* MyHUD = MyPC ? Cast<AShooterHUD>(MyPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		MyHUD->NotifyHit(DamageTaken, DamageEvent, PawnInstigator);
	}

	if (PawnInstigator && PawnInstigator != this && PawnInstigator->IsLocallyControlled())
	{
		AShooterPlayerController* InstigatorPC = Cast<AShooterPlayerController>(PawnInstigator->Controller);
		AShooterHUD* InstigatorHUD = InstigatorPC ? Cast<AShooterHUD>(InstigatorPC->GetHUD()) : NULL;
		if (InstigatorHUD)
		{
			InstigatorHUD->NotifyEnemyHit();
		}
	}
	*/
}


void AFMCharacter::SetRagdollPhysics(){
	/*
	bool bInRagdoll = false;

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}
	else if (!Mesh || !Mesh->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		// initialize physics/etc
		Mesh->SetAllBodiesSimulatePhysics(true);
		Mesh->SetSimulatePhysics(true);
		Mesh->WakeAllRigidBodies();
		Mesh->bBlendPhysics = true;

		bInRagdoll = true;
	}

	CharacterMovement->StopMovementImmediately();
	CharacterMovement->DisableMovement();
	CharacterMovement->SetComponentTickEnabled(false);

	if (!bInRagdoll)
	{
		// hide and set short lifespan
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(10.0f);
	}
	*/
}



void AFMCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled){
	/*
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator.Get()) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass) && (LastTakeHitTimeTimeout == TimeoutTime))
	{
		// same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, just ignore it
			return;
		}

		// otherwise, accumulate damage done this frame
		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<AShooterCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
	*/
}

void AFMCharacter::OnRep_LastTakeHitInfo(){
	/*
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	*/
}

//Pawn::PlayDying sets this lifespan, but when that function is called on client, dead pawn's role is still SimulatedProxy despite bTearOff being true. 
void AFMCharacter::TornOff()
{
	SetLifeSpan(25.f);
}


//////////////////////////////////////////////////////////////////////////
// Inventory

void AFMCharacter::SpawnDefaultInventory(){
	if (Role < ROLE_Authority)
	{
		return;
	}

	// #ttp 321024 - GDC: SHOOTERGAME: Disable secondary weapon (temp)
	// Temporary hack for GDC - add only the first weapon (rifle) thereby disabling the rocket launcher
	int32 NumWeaponClasses = 1;	//DefaultInventoryClasses.Num();	
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;
			AFMWeapon* NewWeapon = GetWorld()->SpawnActor<AFMWeapon>(DefaultInventoryClasses[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

void AFMCharacter::DestroyInventory(){
	if (Role < ROLE_Authority)
	{
		return;
	}
	
	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		AFMWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
	
}

void AFMCharacter::AddWeapon(AFMWeapon* Weapon){
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void AFMCharacter::RemoveWeapon(AFMWeapon* Weapon){
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

AFMWeapon* AFMCharacter::FindWeapon(TSubclassOf<AFMWeapon> WeaponClass){
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] && Inventory[i]->IsA(WeaponClass))
		{
			return Inventory[i];
		}
	}

	return NULL;
}

void AFMCharacter::EquipWeapon(AFMWeapon* Weapon){
	if (Weapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

bool AFMCharacter::ServerEquipWeapon_Validate(AFMWeapon* Weapon){
	return true;
}

void AFMCharacter::ServerEquipWeapon_Implementation(AFMWeapon* Weapon){
	EquipWeapon(Weapon);
}

void AFMCharacter::OnRep_CurrentWeapon(AFMWeapon* LastWeapon){
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AFMCharacter::SetCurrentWeapon(class AFMWeapon* NewWeapon, class AFMWeapon* LastWeapon){
	AFMWeapon* LocalLastWeapon = NULL;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!
		NewWeapon->OnEquip();
	}
}

//////////////////////////////////////////////////////////////////////////
// Replication

void AFMCharacter::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker){
	Super::PreReplication(ChangedPropertyTracker);
	/*
	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(AFMCharacter, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
	*/
}

void AFMCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	/*/
	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(AFMCharacter, Inventory, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated
	DOREPLIFETIME_CONDITION(AFMCharacter, bIsTargeting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AFMCharacter, bWantsToRun, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AFMCharacter, LastTakeHitInfo, COND_Custom);

	// everyone
	DOREPLIFETIME(AFMCharacter, CurrentWeapon);
	DOREPLIFETIME(AFMCharacter, Health);
	*/
}

AFMWeapon* AFMCharacter::GetWeapon() const {
	return CurrentWeapon;
}

int32 AFMCharacter::GetInventoryCount() const {
	return Inventory.Num();
}

AFMWeapon* AFMCharacter::GetInventoryWeapon(int32 index) const {
	return Inventory[index];
}

USkeletalMeshComponent* AFMCharacter::GetPawnMesh() const
{
	return Mesh; // IsFirstPerson() ? Mesh1P : Mesh;
}

bool AFMCharacter::IsUsingWeapon() const
{
	return bWantsToUseWeapon;
};

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AFMCharacter::StartWeaponUse() {
	if (!bWantsToUseWeapon)
	{
		bWantsToUseWeapon = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartUseWeapon();
		}
	}
}

void AFMCharacter::StopWeaponUse(){
	if (bWantsToUseWeapon)
	{
		bWantsToUseWeapon = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopUseWeapon();
		}
	}
}

bool AFMCharacter::CanUse() const {
	//return IsAlive();
	return true;
}

bool AFMCharacter::CanCooldown() const {
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Meshes

void AFMCharacter::UpdatePawnMeshes(){
/*	bool const bFirstPerson = IsFirstPerson();

	Mesh1P->MeshComponentUpdateFlag = !bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh1P->SetOwnerNoSee(!bFirstPerson);

	Mesh->MeshComponentUpdateFlag = bFirstPerson ? EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered : EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh->SetOwnerNoSee(bFirstPerson);
	*/
}


//////////////////////////////////////////////////////////////////////////////////////
// INPUT

void AFMCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent){

	check(InputComponent);
	InputComponent->BindAxis("MoveForward", this, &AFMCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AFMCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &AFMCharacter::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &AFMCharacter::AddControllerPitchInput);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AFMCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &AFMCharacter::OnStopJump);

	InputComponent->BindAction("Fire0", IE_Pressed, this, &AFMCharacter::OnFire0);

}

//////////////////////////////////////////////////////////////////////////////////////
// MOVEMENT

void AFMCharacter::MoveForward(float Value){
	if (Controller && Value != 0.0f){
		// get forward direction
		FRotator Rotation = Controller->GetControlRotation();
		// limit pitch when walking or falling
		if (CharacterMovement->IsMovingOnGround() || CharacterMovement->IsFalling()){
			Rotation.Pitch = 0.0f;
		}
		// add movement in that direction
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFMCharacter::MoveRight(float Value){
	if (Controller && (Value != 0.0f)){
		// get right direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AFMCharacter::OnStartJump(){
	bPressedJump = true;
}

void AFMCharacter::OnStopJump(){
	bPressedJump = false;
}


///////////////////////////////////////////////////////////////////////////////////////
// WEAPON FIRING

void AFMCharacter::OnFire0(){
	// try and fire a projectile
	if (ProjectileClass){
		
		if (RootComponent && CameraBoom){
			// get boom rotation and RootComponent Location in world coords 
			FRotator MuzzleRotation = CameraBoom->GetComponentRotation();
			FVector const MuzzleLocation = RootComponent->GetComponentLocation() + (RootComponent->GetForwardVector() * 50.0f);
			// skew aim up a tiny bit
			MuzzleRotation.Pitch += 10.0f;
			UWorld* const World = GetWorld();
			if (World){
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = Instigator;
				// spawn projectile at the muzzle
				AFMProjectile* const Projectile = World->SpawnActor<AFMProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
				if (Projectile){
					// find launch direction
					FVector const LaunchDir = MuzzleRotation.Vector();
					Projectile->InitVelocity(LaunchDir);
				}
			}


		}
		
	}
}