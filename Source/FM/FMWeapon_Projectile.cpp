

#include "FM.h"
#include "FMCharacter.h"
#include "FMPlayerController.h"
#include "FMWeapon_Projectile.h"


AFMWeapon_Projectile::AFMWeapon_Projectile(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}


void AFMWeapon_Projectile::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);
}



/////////////////////////////////////////////////////////////////////////////////////////
// CONFIG

void AFMWeapon_Projectile::ApplyWeaponConfig(FProjectileWeaponData& Data){
	Data = ProjectileConfig;
}



///////////////////////////////////////////////////////////////////////////////////////////
// WEAPON USAGE

void AFMWeapon_Projectile::OnUseWeaponStarted(){
	float totalStaminaUse = WeaponConfig.StaminaCost + (WeaponConfig.StaminaCostCharging + chargeValue);

	if (Role == ROLE_Authority){
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, FString::Printf(TEXT("Weapon: SERVER : UseStart cost %f, CurrentSwingID %f"), totalStaminaUse, currentSwingID));
		}
	}
	else{
		if (GEngine){
			GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, FString::Printf(TEXT("Weapon: CLIENT : UseStart cost %f, CurrentSwingID %f"), totalStaminaUse, currentSwingID));
		}
	}

	// check if greater than max combo count for this swing type
	if (GetCurrentSwingMod() < 2){
		UseStamina(false, 0.0f);
		UseWeapon();
	}else{
		// outside of combo range... just end it here
		bWantsToUse = false;
	}
}

void AFMWeapon_Projectile::UseWeapon(){
	
	if (Role == ROLE_Authority){
		if (MyPawn){
			FVector MyLoc;
			FRotator MyRot;
			FVector Forward = MyPawn->FollowCamera->GetForwardVector();
			MyRot = MyPawn->GetControlRotation();
			MyRot.Pitch += 30.0f;
			MyLoc = MyPawn->GetActorLocation() + Forward * 100.0f + FVector(0.0f, 0.0f, 50.0f);
			
			UWorld* const World = GetWorld();
			if (World){
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = Instigator;

				AFMProjectile* projectile = World->SpawnActor<AFMProjectile>(ProjectileConfig.ProjectileClass, MyLoc, MyRot, SpawnParams);
				if (projectile){
					FVector shootDirection = MyPawn->FollowCamera->GetForwardVector() * FMath::Max(1.0f, chargeValue)s;
					projectile->InitVelocity(shootDirection);
				}
			}
			
		}
	}

	

	Super::OnUseWeaponEnded();

}


////////////////////////////////////////////////////////////////////////////////////////////
// AIM HELPER

FVector AFMWeapon_Projectile::GetAim() const {

	AFMPlayerController* const PC = Instigator ? Cast<AFMPlayerController>(Instigator->Controller) : NULL;
	FVector FinalAim = FVector::ZeroVector;

	if (PC){
		FVector MyLoc;
		FRotator MyRot;
		//PC->GetPlayerViewPoint(CamLoc, CamRot);
		MyRot = MyPawn->GetControlRotation();
		MyLoc = MyPawn->GetPawnViewLocation();
		FinalAim = MyRot.Vector();
	}
	return FinalAim;
}