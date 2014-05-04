

#pragma once
#include "FMProjectile.h"
#include "FMWeapon.h"
#include "FMWeapon_Projectile.generated.h"


USTRUCT()
struct FProjectileWeaponData{

	GENERATED_USTRUCT_BODY()

	// projectile class
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFMProjectile> ProjectileClass;

	


	FProjectileWeaponData(){
		ProjectileClass = NULL;
	}
};

UCLASS()
class AFMWeapon_Projectile : public AFMWeapon
{
	GENERATED_UCLASS_BODY()


	// apply config on projectile
	void ApplyWeaponConfig(FProjectileWeaponData& Data);

protected:

	// weapon config
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FProjectileWeaponData ProjectileConfig;

	//////////////////////////////////////////////////////////////////////////////
	// WEAPON USAGE

	// [local + server] use weapon implementation
	virtual void UseWeapon() OVERRIDE;
	
	///////////////////////////////////////////////////////////////////////////////
	// AIM HELPER
	FVector GetAim() const;
};
