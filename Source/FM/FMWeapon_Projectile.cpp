

#include "FM.h"
#include "FMCharacter.h"
#include "FMPlayerController.h"
#include "FMWeapon_Projectile.h"


AFMWeapon_Projectile::AFMWeapon_Projectile(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
// CONFIG

void AFMWeapon_Projectile::ApplyWeaponConfig(FProjectileWeaponData& Data){
	Data = ProjectileConfig;
}



///////////////////////////////////////////////////////////////////////////////////////////
// WEAPON USAGE

void AFMWeapon_Projectile::UseWeapon(){
	
	if (Role == ROLE_Authority){
		if (MyPawn){
			FVector MyLoc;
			FRotator MyRot;
			FVector Forward = MyPawn->GetActorForwardVector();
			//PC->GetPlayerViewPoint(CamLoc, CamRot);
			MyRot = MyPawn->GetControlRotation();
			MyLoc = MyPawn->GetPawnViewLocation() + MyPawn->GetActorForwardVector() * 50.0f;

			FTransform SpawnTM(MyRot, MyLoc);
			
			UWorld* const World = GetWorld();
			if (World){
				AFMProjectile* projectile = Cast<AFMProjectile>(UGameplayStatics::BeginSpawningActorFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));
				if (projectile){
					projectile->Instigator = Instigator;
					projectile->SetOwner(MyPawn);
					projectile->InitVelocity(Forward);
					UGameplayStatics::FinishSpawningActor(projectile, SpawnTM);
					if (GEngine){
						GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("Projectile Weapon: Use and shot "));
					}
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