// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMProjectile.h"


AFMProjectile::AFMProjectile(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP){

	// use a sphere as a simple collision representation
	CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	// set collision profile from DefaultEngine.ini
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	// Hook OnHit function to collisioncomponents 'OnComponentBeginOverlap' delegate
	CollisionComp->OnComponentHit.AddDynamic(this, &AFMProjectile::OnHit);
	RootComponent = CollisionComp;

	// use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;

	// set actor lifespan
	InitialLifeSpan = 3.0f;

}


void AFMProjectile::InitVelocity(const FVector& ShootDirection){
	if (ProjectileMovement){
		// set the projectile's velocity to the desired direction
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}

void AFMProjectile::OnHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	if (OtherActor && (OtherActor != this) && OtherComp){
		OtherComp->AddImpulseAtLocation(ProjectileMovement->Velocity + 100.0f, Hit.ImpactPoint);
	}
}

