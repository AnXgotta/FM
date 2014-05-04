

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

	virtual void Tick(float DeltaSeconds) OVERRIDE;


protected:

	// weapon config
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FProjectileWeaponData ProjectileConfig;

	//////////////////////////////////////////////////////////////////////////////
	// WEAPON USAGE

	// [local + server] use weapon implementation
	virtual void UseWeapon() OVERRIDE;

	UFUNCTION()
		virtual void OnUseWeaponStarted() OVERRIDE;

	///////////////////////////////////////////////////////////////////////////////
	// AIM HELPER
	FVector GetAim() const;
};
