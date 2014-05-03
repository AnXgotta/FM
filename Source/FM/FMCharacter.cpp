// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "FM.h"
#include "FMPlayerController.h"
#include "FMGameMode.h"
#include "FMCharacter.h"
#include "FMWeapon.h"
#include "FMDamageType.h"
#include "FMCharacterMovementComponent.h"


AFMCharacter::AFMCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP.SetDefaultSubobjectClass<UFMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Configure character movement
	CharacterMovement->JumpZVelocity = 400.0f;
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
	

	/*
	Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("PawnMesh1P"));
	Mesh1P->AttachParent = CapsuleComponent;
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1P->bChartDistanceFactor = false;
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	*/
	/*


	Mesh->bOnlyOwnerSee = false;
	Mesh->bOwnerNoSee = true;
	Mesh->bReceivesDecals = false;
	Mesh->SetCollisionObjectType(ECC_Pawn);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//	Mesh->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	Mesh->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	CapsuleComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
//	CapsuleComponent->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	
	TargetingSpeedModifier = 0.5f;
	bIsTargeting = false;
	
	
	bWantsToUse = false;
	*/

	bWantsToUseWeapon = false;

	currentStamina = 100.0f;
	currentMaxStamina = 100.0f;
	defaultMaxStamina = 100.0f;
	staminaRegenerationCooldown = 5.0f;
	staminaRegenerationPerSecond = 10.0f;
	bRegenerateStamina = false;


	currentHealth = 100.0f;
	currentMaxHealth = 100.0f;
	defaultMaxHealth = 100.0f;

	RunningStaminaCostPerSecond = 10.0f;
	RunningSpeedModifier = 1.5f;
	bWantsToRun = false;

	
	LowHealthPercentage = 0.20f;
	

}

void AFMCharacter::BeginPlay(){
	Super::BeginPlay();
	if (Role == ROLE_Authority){
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Character : SERVER : BeginPlay Called %s"), *this->GetName()));
		}
	}else{
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Character : CLIENT : BeginPlay Called %s"), *this->GetName()));
		}
	}
	
}

void AFMCharacter::PostInitializeComponents(){
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)	{
		currentHealth = GetDefaultMaxHealth();
		currentStamina = GetDefaultMaxStamina();
		SpawnDefaultInventory();
	}
	
	// set initial mesh visibility
	UpdatePawnMeshes();

	// create material instance for setting team colors (3rd person view)
	//for (int32 iMat = 0; iMat < Mesh->GetNumMaterials(); iMat++){
	//	MeshMIDs.Add(Mesh->CreateAndSetMaterialInstanceDynamic(iMat));
	//}

	// play respawn effects
	if (GetNetMode() != NM_DedicatedServer){
		//if (RespawnFX){
		//	UGameplayStatics::SpawnEmitterAtLocation(this, RespawnFX, GetActorLocation(), GetActorRotation());
		//}

		//if (RespawnSound){
		//	UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());
		//}
	}
	
}

void AFMCharacter::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);

	// sprinting
	if (bWantsToRun && IsRunning()){
		if (GetCurrentStamina() > 0.5f){
			UseStamina(DeltaSeconds * RunningStaminaCostPerSecond);
		}else{
			SetRunning(false, false);
		}
	}

	// stamina regeneration
	if (bRegenerateStamina){
		if (GetCurrentStamina() < 100.0f){
			AddStamina(DeltaSeconds * staminaRegenerationPerSecond);
		}else{
			StopRegenerateStamina();
		}
	}

	/*
	if (bWantsToRunToggled && !IsRunning()){
		SetRunning(false, false);
	}
	AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	if (MyPC && MyPC->HasHealthRegen()){
		if (this->currentHealth < this->GetDefaultMaxHealth())
		{
			this->currentHealth += 5 * DeltaSeconds;
			if (currentHealth > this->GetDefaultMaxHealth())
			{
				currentHealth = this->GetDefaultMaxHealth();
			}
		}
	}

	if (LowHealthSound && GEngine->UseSound())
	{
		if ((this->currentHealth > 0 && this->currentHealth < this->GetDefaultMaxHealth() * LowHealthPercentage) && (!LowHealthWarningPlayer || !LowHealthWarningPlayer->IsPlaying()))
		{
			LowHealthWarningPlayer = UGameplayStatics::PlaySoundAttached(LowHealthSound, GetRootComponent(),
				NAME_None, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, true);
			LowHealthWarningPlayer->SetVolumeMultiplier(0.0f);
		}
		else if ((this->currentHealth > this->GetDefaultMaxHealth() * LowHealthPercentage || this->currentHealth < 0) && LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
		{
			LowHealthWarningPlayer->Stop();
		}
		if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
		{
			const float MinVolume = 0.3f;
			const float VolumeMultiplier = (1.0f - (this->currentHealth / (this->GetDefaultMaxHealth() * LowHealthPercentage)));
			LowHealthWarningPlayer->SetVolumeMultiplier(MinVolume + (1.0f - MinVolume) * VolumeMultiplier);
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

	// update mesh
	UpdatePawnMeshes();

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

	// [client] as soon as PlayerState is assigned, set team colors of this pawn for local player
	if (PlayerState != NULL){
		//UpdateTeamColorsAllMIDs();
	}
	
}


//////////////////////////////////////////////////////////////////////////
// ANIMATIONS

float AFMCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName){
	
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance){
		//return UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
	}
	
	return 0.0f;
}

void AFMCharacter::StopAnimMontage(class UAnimMontage* AnimMontage){
	
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance && UseMesh->AnimScriptInstance->Montage_IsPlaying(AnimMontage)) {
		//UseMesh->AnimScriptInstance->Montage_Stop(AnimMontage->BlendOutTime);
	}
	
}

void AFMCharacter::StopAllAnimMontages(){
	
	USkeletalMeshComponent* UseMesh = GetPawnMesh();
	if (UseMesh && UseMesh->AnimScriptInstance){
		//UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
	
}


//////////////////////////////////////////////////////////////////////////
// DAMAGE AND DEATH
/*
void AFMCharacter::FellOutOfWorld(const class UDamageType& dmgType){
	Die(currentHealth, FDamageEvent(dmgType.GetClass()), NULL, NULL);
}

void AFMCharacter::Suicide(){
	KilledBy(this);
}

void AFMCharacter::KilledBy(APawn* EventInstigator){
	if (Role == ROLE_Authority && !bIsDying){
		AController* Killer = NULL;
		if (EventInstigator != NULL){
			Killer = EventInstigator->Controller;
			LastHitBy = NULL;
		}

		Die(currentHealth, FDamageEvent(UDamageType::StaticClass()), Killer, NULL);
	}
}
*/
float AFMCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser){
	AFMPlayerController* MyPC = Cast<AFMPlayerController>(Controller);

	if (currentHealth <= 0.0f){
		return 0.f;
	}
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("Character: TakeDamage "));
	}

	// Modify based on game rules.
	//AFMGameMode* const Game = GetWorld()->GetAuthGameMode<AFMGameMode>();
	//Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f)	{
		currentHealth = FMath::Max(0.0f, currentHealth - ActualDamage);
		if (currentHealth <= 0.0f){
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}else{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		//MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}
	
	return ActualDamage;
}

bool AFMCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const {
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode() == NULL
		|| GetWorld()->GetAuthGameMode()->bLevelChange){	// level transition occurring
	
		return false;
	}

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("Character: Can Die True "));
	}

	return true;
}



bool AFMCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser){
	
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser)){
		return false;
	}

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("Character: Die "));
	}

	currentHealth = FMath::Max(0.0f, currentHealth);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<AFMGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	NetUpdateFrequency = GetDefault<AFMCharacter>()->NetUpdateFrequency;
	CharacterMovement->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	
	return true;
}




void AFMCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser){
	
	if (bIsDying){
		return;
	}

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("Character: OnDeath "));
	}

	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	if (Role == ROLE_Authority){
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);

		// play the force feedback effect on the client player controller
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass){
			UFMDamageType *DamageType = Cast<UFMDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->KilledForceFeedback){
				PC->ClientPlayForceFeedback(DamageType->KilledForceFeedback, false, "Damage");
			}
		}
	}

	// cannot use IsLocallyControlled here, because even local client's controller may be NULL here
	//if (GetNetMode() != NM_DedicatedServer && DeathSound && Mesh1P && Mesh1P->IsVisible()){
		//UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	//}

	// remove all weapons
	DestroyInventory();

	// fix up 3rd person mesh
	UpdatePawnMeshes();

	DetachFromControllerPendingDestroy();
	StopAllAnimMontages();

	//if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())	{
	//	LowHealthWarningPlayer->Stop();
	//}

	//if (RunLoopAC){
	//	RunLoopAC->Stop();
	//}

	// disable collisions on capsule
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (Mesh){
		//static FName CollisionProfileName(TEXT("Ragdoll"));
		//Mesh->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	// Death anim
	//float DeathAnimDuration = PlayAnimMontage(DeathAnim);
	
	// Ragdoll
	
	//if (DeathAnimDuration > 0.f){
//		GetWorldTimerManager().SetTimer(this, &AFMCharacter::SetRagdollPhysics, FMath::Min(0.1f, DeathAnimDuration), false);
//	}else{
		SetRagdollPhysics();
	//}
	
}


void AFMCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser){
	
	if (Role == ROLE_Authority){
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);

		// play the force feedback effect on the client player controller
		
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && DamageEvent.DamageTypeClass){
			UFMDamageType *DamageType = Cast<UFMDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
			if (DamageType && DamageType->HitForceFeedback)	{
				if (GEngine){
					GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("Character: ClientPlayForceFeedback"));
				}
				PC->ClientPlayForceFeedback(DamageType->HitForceFeedback, false, "Damage");
			}
		}
		
	}

	if (DamageTaken > 0.0){
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}

	//AFMPlayerController* MyPC = Cast<AFMPlayerController>(Controller);
	//AFMHUD* MyHUD = MyPC ? Cast<AFMHUD>(MyPC->GetHUD()) : NULL;
	//if (MyHUD)	{
	//	MyHUD->NotifyHit(DamageTaken, DamageEvent, PawnInstigator);
	//}

	if (PawnInstigator && PawnInstigator != this && PawnInstigator->IsLocallyControlled()){
		//AFMPlayerController* InstigatorPC = Cast<AFMPlayerController>(PawnInstigator->Controller);
		//AShooterHUD* InstigatorHUD = InstigatorPC ? Cast<AShooterHUD>(InstigatorPC->GetHUD()) : NULL;
		//if (InstigatorHUD){
		//	InstigatorHUD->NotifyEnemyHit();
		//}
	}
	
}

void AFMCharacter::SetRagdollPhysics(){
	
	bool bInRagdoll = false;

	if (IsPendingKill()){
		bInRagdoll = false;
	}else if (!Mesh || !Mesh->GetPhysicsAsset()){
		bInRagdoll = false;
	}else{
		// initialize physics/etc
		Mesh->SetAllBodiesSimulatePhysics(true);
		Mesh->SetSimulatePhysics(true);
		Mesh->WakeAllRigidBodies();
		Mesh->bBlendPhysics = true;

		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		bInRagdoll = true;
	}

	CharacterMovement->StopMovementImmediately();
	CharacterMovement->DisableMovement();
	CharacterMovement->SetComponentTickEnabled(false);

	if (!bInRagdoll){
		// hide and set short lifespan
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}else{
		SetLifeSpan(10.0f);
	}
	
}


void AFMCharacter::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled){
	
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator.Get()) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass) && (LastTakeHitTimeTimeout == TimeoutTime)){
		// same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)	{
			// Redundant death take hit, just ignore it
			return;
		}

		// otherwise, accumulate damage done this frame
		Damage += LastTakeHitInfo.ActualDamage;
		
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<AFMCharacter>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
	
}

void AFMCharacter::OnRep_LastTakeHitInfo(){
	
	if (LastTakeHitInfo.bKilled){
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}else{
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	
}

//Pawn::PlayDying sets this lifespan, but when that function is called on client, dead pawn's role is still SimulatedProxy despite bTearOff being true. 
void AFMCharacter::TornOff(){
	SetLifeSpan(25.f);
}

bool AFMCharacter::IsAlive() const{
	return currentHealth > 0;
}

//////////////////////////////////////////////////////////////////////////
// INVENTORY (WEAPON MANAGEMENT (Adding to default inventory ) HANDLED IN BLUEPRINT!)

void AFMCharacter::SpawnDefaultInventory(){
	if (Role < ROLE_Authority){
		return;
	}

	int32 NumWeaponClasses = DefaultInventoryClasses.Num();	
	for (int32 i = 0; i < NumWeaponClasses; i++){
		if (DefaultInventoryClasses[i]){
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;
			AFMWeapon* NewWeapon = GetWorld()->SpawnActor<AFMWeapon>(DefaultInventoryClasses[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}

	if (Inventory.Num() > 0){
		EquipWeapon(Inventory[0]);
	}
}

void AFMCharacter::DestroyInventory(){
	if (Role < ROLE_Authority)
	{
		return;
	}
	
	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--){
		AFMWeapon* Weapon = Inventory[i];
		if (Weapon){
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
	
}

void AFMCharacter::AddWeapon(AFMWeapon* Weapon){
	if (Weapon && Role == ROLE_Authority){
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void AFMCharacter::RemoveWeapon(AFMWeapon* Weapon){
	if (Weapon && Role == ROLE_Authority){
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

AFMWeapon* AFMCharacter::FindWeapon(TSubclassOf<AFMWeapon> WeaponClass){
	for (int32 i = 0; i < Inventory.Num(); i++){
		if (Inventory[i] && Inventory[i]->IsA(WeaponClass))	{
			return Inventory[i];
		}
	}

	return NULL;
}

void AFMCharacter::EquipWeapon(AFMWeapon* Weapon){
	if (Weapon){
		if (Role == ROLE_Authority){
			SetCurrentWeapon(Weapon);
		}else{
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

void AFMCharacter::SetCurrentWeapon(class AFMWeapon* NewWeapon, class AFMWeapon* LastWeapon){
	AFMWeapon* LocalLastWeapon = NULL;

	if (LastWeapon != NULL){
		LocalLastWeapon = LastWeapon;
	}else if (NewWeapon != CurrentWeapon){
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon){
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;
	if (Role == ROLE_Authority){
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, TEXT("Character: SERVER : SetCurrentWeapon"));
		}
	}
	else{
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, TEXT("Character: CLIENT : SetCurrentWeapon"));
		}
	}
	

	// equip new one
	if (NewWeapon){
		if (Role == ROLE_Authority){
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Character: SERVER : SetCurrentWeapon %s"), *NewWeapon->GetName()));
			}
		}
		else{
			if (GEngine){
				GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, FString::Printf(TEXT("Character: CLIENT : SetCurrentWeapon %s"), *NewWeapon->GetName()));
			}
		}
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!
		NewWeapon->OnEquip();

	}
}


//////////////////////////////////////////////////////////////////////////
// REPLICATION

/*
void AFMCharacter::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker){
	Super::PreReplication(ChangedPropertyTracker);
	
	// Only replicate this property for a short duration after it changes so join in progress players don't get spammed with fx when joining late
	DOREPLIFETIME_ACTIVE_OVERRIDE(AFMCharacter, LastTakeHitInfo, GetWorld() && GetWorld()->GetTimeSeconds() < LastTakeHitTimeTimeout);
	
}
*/

void AFMCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	//enum ELifetimeCondition{COND_None = 0,COND_InitialOnly = 1,COND_OwnerOnly = 2,COND_SkipOwner = 3,COND_SimulatedOnly = 4,
	//COND_AutonomousOnly = 5,COND_SimulatedOrPhysics = 6,COND_InitialOrOwner = 7,COND_Custom = 8,COND_Max = 9,}

	/*	COND_InitialOnly - This property will only attempt to send on the initial bunch
		COND_OwnerOnly - This property will only send to the actor's owner
		COND_SkipOwner - This property send to every connection EXCEPT the owner
		COND_SimulatedOnly - This property will only send to simulated actors
		COND_AutonomousOnly - This property will only send to autonomous actors
		COND_SimulatedOrPhysics - This property will send to simulated OR bRepPhysics actors
		COND_InitialOrOwner - This property will send on the initial packet, or to the actors owner
		COND_Custom - This property has no particular condition, but wants the ability to toggle on / off via SetCustomIsActiveOverride
*/

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(AFMCharacter, Inventory, COND_OwnerOnly);
	//Unsure about rep style for currentMaxHealth
	DOREPLIFETIME_CONDITION(AFMCharacter, currentMaxHealth, COND_OwnerOnly);
	//Unsure about rep style for currentStamina
	DOREPLIFETIME_CONDITION(AFMCharacter, currentStamina, COND_OwnerOnly);
	//Unsure about rep style for currentMaxStamina
	DOREPLIFETIME_CONDITION(AFMCharacter, currentMaxStamina, COND_OwnerOnly);
	//Unsure about rep style for staminaRegenerationPerSecond
	DOREPLIFETIME_CONDITION(AFMCharacter, staminaRegenerationPerSecond, COND_OwnerOnly);
	
	// everyone except local owner: flag change is locally instigated
	//DOREPLIFETIME_CONDITION(AFMCharacter, bIsTargeting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AFMCharacter, bWantsToRun, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AFMCharacter, LastTakeHitInfo, COND_Custom);

	// everyone
	DOREPLIFETIME(AFMCharacter, CurrentWeapon);

	DOREPLIFETIME(AFMCharacter, currentHealth);	
}

void AFMCharacter::OnRep_CurrentWeapon(AFMWeapon* LastWeapon){
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
	if (Role == ROLE_Authority){
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, TEXT("Character: SERVER : OnRep_CurrentWeapon"));
		}
	}
	else{
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Blue, TEXT("Character: CLIENT : OnRep_CurrentWeapon"));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GETS AND SETS

float AFMCharacter::GetDefaultMaxHealth() const {
	return defaultMaxHealth;
}

float AFMCharacter::GetCurrentMaxHealth() const {
	return currentMaxHealth;
}

float AFMCharacter::GetCurrentHealth() const{
	return currentHealth;
}

float AFMCharacter::GetDefaultMaxStamina() const {
	return defaultMaxStamina;
}

float AFMCharacter::GetCurrentMaxStamina() const {
	return currentMaxStamina;
}

float AFMCharacter::GetCurrentStamina() const {
	return currentStamina;
}

USkeletalMeshComponent* AFMCharacter::GetPawnMesh() const {
	return Mesh; // IsFirstPerson() ? Mesh1P : Mesh;
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


/////////////////////////////////////////////////////////////////////////
// HEALTH AND STAMINA MANAGEMENT

float AFMCharacter::PercentStaminaAvailable(float StaminaCost){
	return currentStamina/StaminaCost;
}

// MUST BE CALLED ON BOTH SERVER AND CLIENT
// ALL THINGS THAT USE STAMINA MUST PERFORM ON SERVER AND CLIENT< THEN CALL THIS THERE
void AFMCharacter::UseStamina(float Value){
	StopRegenerateStamina();
	currentStamina = FMath::Max(0.0f, currentStamina - Value);
}

void AFMCharacter::ResetStaminaCooldown(){
	bRegenerateStamina = false;
	GetWorldTimerManager().SetTimer(this, &AFMCharacter::StartRegenerateStamina, staminaRegenerationCooldown, false);
}

void AFMCharacter::AddStamina(float Value){
	currentStamina = FMath::Min(currentMaxStamina, currentStamina + Value);
}

void AFMCharacter::StartRegenerateStamina(){
	bRegenerateStamina = true;
}

void AFMCharacter::StopRegenerateStamina(){
	bRegenerateStamina = false;
	GetWorldTimerManager().ClearTimer(this, &AFMCharacter::StartRegenerateStamina);
}

bool AFMCharacter::CheckIfStaminaGreaterThan(float Value){
	return currentStamina > Value;
}

//////////////////////////////////////////////////////////////////////////
// WEAPON USAGE

void AFMCharacter::StartWeaponUse() {
	if (!bWantsToUseWeapon)	{
		bWantsToUseWeapon = true;
		if (CurrentWeapon){
			//CurrentWeapon->StartUseWeapon();
		}
	}
}

void AFMCharacter::StopWeaponUse(){
	if (bWantsToUseWeapon){
		bWantsToUseWeapon = false;
		if (CurrentWeapon){
			//CurrentWeapon->StopUseWeapon();
		}
	}
}

bool AFMCharacter::CanUse() const {
	return true;// IsAlive();
	//return true;
}

bool AFMCharacter::CanCooldown() const {
	// logic goes here
	return true;
}

bool AFMCharacter::IsUsingWeapon() const {
	return bWantsToUseWeapon;
};

//////////////////////////////////////////////////////////////////////////
// MESHES

USkeletalMeshComponent* AFMCharacter::GetSpecifcPawnMesh(bool WantFirstPerson) const {
	return WantFirstPerson == true ? Mesh : Mesh;
}


void AFMCharacter::UpdatePawnMeshes(){
	Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	Mesh->SetOwnerNoSee(false);
	
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

void AFMCharacter::OnStartRunning(){
	AFMPlayerController* MyPC = Cast<AFMPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed()){
		SetRunning(true, false);
	}
}

void AFMCharacter::OnStopRunning(){
	SetRunning(false, false);
}

bool AFMCharacter::IsRunning() const {
	if (!CharacterMovement) return false;

	return bWantsToRun && !GetVelocity().IsZero();// && (GetVelocity().SafeNormal2D | GetActorRotation().Vector()) > -0.1;
}

void AFMCharacter::SetRunning(bool bNewSprint, bool bToggle){
	
	if (Role < ROLE_Authority){
		ServerSetRunning(bNewSprint, bToggle);
	}
	bWantsToRun = bNewSprint;

	if (!bWantsToRun){
		ResetStaminaCooldown();
	}

	// UpdateRunningSounds(bNewSprint);
}

bool AFMCharacter::ServerSetRunning_Validate(bool bNewSprint, bool bToggle){
	return true;
}

void AFMCharacter::ServerSetRunning_Implementation(bool bNewSprint, bool bToggle){
	
	SetRunning(bNewSprint, bToggle);
}

float AFMCharacter::GetRunningSpeedModifier() const{
	return RunningSpeedModifier;
}


///////////////////////////////////////////////////////////////////////////////////////
// INPUT HANDLERS

void AFMCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent){

	check(InputComponent);
	InputComponent->BindAxis("MoveForward", this, &AFMCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AFMCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &AFMCharacter::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &AFMCharacter::AddControllerPitchInput);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AFMCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &AFMCharacter::OnStopJump);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AFMCharacter::OnStartRunning);
	InputComponent->BindAction("Sprint", IE_Released, this, &AFMCharacter::OnStopRunning);

	InputComponent->BindAction("Fire0", IE_Pressed, this, &AFMCharacter::OnFire0Pressed);
	InputComponent->BindAction("Fire0", IE_Released, this, &AFMCharacter::OnFire0Released);
	InputComponent->BindAction("Fire1", IE_Pressed, this, &AFMCharacter::OnFire1);
	InputComponent->BindAction("Fire2", IE_Pressed, this, &AFMCharacter::OnFire2);
	InputComponent->BindAction("Fire3", IE_Pressed, this, &AFMCharacter::OnFire3);

	InputComponent->BindAction("Equip1", IE_Pressed, this, &AFMCharacter::OnEquip1);
	InputComponent->BindAction("Equip2", IE_Pressed, this, &AFMCharacter::OnEquip2);
	InputComponent->BindAction("Equip3", IE_Pressed, this, &AFMCharacter::OnEquip3);

}

void AFMCharacter::OnFire0Pressed(){	
	if (CurrentWeapon){
		CurrentWeapon->StartUseWeaponPressed();
	}
}

void AFMCharacter::OnFire0Released(){
	if (CurrentWeapon){
		CurrentWeapon->StartUseWeaponReleased();
	}
}


void AFMCharacter::OnFire1(){
	if (CurrentWeapon){
		CurrentWeapon->StartUseWeaponPressedAlternates(2);
	}
}

void AFMCharacter::OnFire2(){
	if (CurrentWeapon){
		CurrentWeapon->StartUseWeaponPressedAlternates(3);
	}
}

void AFMCharacter::OnFire3(){
	if (CurrentWeapon){
		CurrentWeapon->StartUseWeaponPressedAlternates(4);
	}
}

void AFMCharacter::OnEquip1(){
	if (Inventory[0]){
		EquipWeapon(Inventory[0]);
	}
}

void AFMCharacter::OnEquip2(){
	if (Inventory[1]){
		EquipWeapon(Inventory[1]);
	}
}

void AFMCharacter::OnEquip3(){
	if (Inventory[2]){
		EquipWeapon(Inventory[2]);
	}
}