// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMCharacter.h"
#include "FMProjectile.h"


AFMCharacter::AFMCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Configure character movement
	CharacterMovement->JumpZVelocity = 600.f;
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