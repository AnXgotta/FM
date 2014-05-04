// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FM.h"
#include "FMProjectile.h"


AFMProjectile::AFMProjectile(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP){

	// use a sphere as a simple collision representation
	CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RootComponent = CollisionComp;

	ParticleComp = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("ParticleComp"));
	ParticleComp->bAutoActivate = false;
	ParticleComp->bAutoDestroy = false;
	ParticleComp->AttachParent = RootComponent;

	MovementComp = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->MaxSpeed = 2000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateInstigator = true;
	bReplicateMovement = true;
}


void AFMProjectile::PostInitializeComponents(){
	Super::PostInitializeComponents();
	MovementComp->OnProjectileStop.AddDynamic(this, &AFMProjectile::OnImpact);
	CollisionComp->MoveIgnoreActors.Add(Instigator);

	SetLifeSpan(WeaponConfig.ProjectileLife);
	MyController = GetInstigatorController();
}

void AFMProjectile::InitVelocity(FVector& ShootDirection){
	if (MovementComp){
		MovementComp->Velocity = ShootDirection * MovementComp->InitialSpeed;
	}
}

void AFMProjectile::OnImpact(const FHitResult& HitResult){
	if (Role == ROLE_Authority && !bDidHit)	{
		HandleImpact(HitResult);
		DisableAndDestroy();
	}
}

void AFMProjectile::HandleImpact(const FHitResult& Impact){
	if (ParticleComp)	{
		ParticleComp->Deactivate();
	}

	// effects and damage origin shouldn't be placed inside mesh at impact point
	const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;

	if (WeaponConfig.ImpactDamage > 0 && WeaponConfig.AOERadius > 0 && WeaponConfig.DamageType)	{
		UGameplayStatics::ApplyRadialDamage(this, WeaponConfig.ImpactDamage, NudgedImpactLocation, WeaponConfig.AOERadius, WeaponConfig.DamageType, TArray<AActor*>(), this, MyController.Get());
	}

	bDidHit = true;
}

void AFMProjectile::DisableAndDestroy(){
	UAudioComponent* ProjAudioComp = FindComponentByClass<UAudioComponent>();
	if (ProjAudioComp && ProjAudioComp->IsPlaying()){
		ProjAudioComp->FadeOut(0.1f, 0.f);
	}

	MovementComp->StopMovementImmediately();

	// give clients some time to show explosion
	SetLifeSpan(2.0f);
}

void AFMProjectile::OnRep_HandleImpact(){
	FVector ProjDirection = GetActorRotation().Vector();

	const FVector StartTrace = GetActorLocation() - ProjDirection * 200;
	const FVector EndTrace = GetActorLocation() + ProjDirection * 150;
	FHitResult Impact;

	if (!GetWorld()->LineTraceSingle(Impact, StartTrace, EndTrace, COLLISION_PROJECTILE, FCollisionQueryParams(TEXT("ProjClient"), true, Instigator))){
		// failsafe
		Impact.ImpactPoint = GetActorLocation();
		Impact.ImpactNormal = -ProjDirection;
	}

	HandleImpact(Impact);
}

void AFMProjectile::PostNetReceiveVelocity(const FVector& NewVelocity){
	if (MovementComp){
		MovementComp->Velocity = NewVelocity;
	}
}


void AFMProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFMProjectile, bDidHit);
}