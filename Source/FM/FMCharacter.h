// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "FMCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AFMCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = CameraBoom)
	TSubobjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = FollowCamera)
	TSubobjectPtr<UCameraComponent> FollowCamera;

	// Weapon 'muzzle' position relative to camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	// projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFMProjectile> ProjectileClass;


	virtual void BeginPlay() OVERRIDE;


protected:
	// setup pawn specific input handlers
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


	// handle fire0
	UFUNCTION()
	void OnFire0();

};
