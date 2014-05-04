// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "FMProjectile.generated.h"


USTRUCT()
struct FProjectileData{

	GENERATED_USTRUCT_BODY()

	// life time
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float ProjectileLife;

	// impact damage amount
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		float ImpactDamage;

	// AOE damage radius
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		float AOERadius;

	// Damage type
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		TSubclassOf<UDamageType> DamageType;



	FProjectileData(){
		ProjectileLife = 10.0f;
		ImpactDamage = 20.0f;
		AOERadius = 300.0f;
		DamageType = UDamageType::StaticClass();
	}
};

UCLASS(Abstract, Blueprintable, DependsOn = FMWeapon_Projectile)
class AFMProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

	// initial setup
	virtual void PostInitializeComponents() OVERRIDE;

	// setup velocity
	void InitVelocity(float SpeedMultiplier);

	// handle impact
	void OnImpact(const FHitResult& HitResult);

protected:

	// movement component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		TSubobjectPtr<UProjectileMovementComponent> MovementComp;

	// collisions
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		TSubobjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		TSubobjectPtr<UParticleSystemComponent> ParticleComp;

	// hit effect

	// controller that fired this projectile
	TWeakObjectPtr<AController> MyController;

	// Projectile Data
	UPROPERTY(EditDefaultsOnly, Category=Config)
	struct FProjectileData WeaponConfig;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_HandleImpact)
		bool bDidHit;


	UFUNCTION()
		void OnRep_HandleImpact();

	void HandleImpact(const FHitResult& Impact);

	void DisableAndDestroy();

	// update velocity on client
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) OVERRIDE;
	
};
