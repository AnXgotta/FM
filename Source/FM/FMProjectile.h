// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "FMProjectile.generated.h"


UCLASS()
class AFMProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	// projectile collision component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	TSubobjectPtr<USphereComponent> CollisionComp;

	// projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	TSubobjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	// called on projectile hit
	UFUNCTION()
	void OnHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	void InitVelocity(const FVector& ShootDirection);
	
};
