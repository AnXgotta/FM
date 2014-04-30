// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "GameFramework/GameMode.h"
#include "FMGameMode.generated.h"

/**
 * 
 */
UCLASS(config=game)
class AFMGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()
	/*
	virtual void BeginPlay() OVERRIDE;


	// Initialize the game. This is called before actors' PreInitializeComponents.
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) OVERRIDE;

	// Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string. 
	virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr<class FUniqueNetId>& UniqueId, FString& ErrorMessage) OVERRIDE;

	// starts match warmup 
	virtual void PostLogin(APlayerController* NewPlayer) OVERRIDE;

	// disable respawns outside current match 
	virtual bool PlayerCanRestart(APlayerController* Player) OVERRIDE;

	
	*/
	

	/*
	// returns default pawn class for given controller 
	virtual UClass* GetDefaultPawnClassForController(AController* InController) OVERRIDE;

	// prevents friendly fire 
	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;
	*/

	// notify about kills 
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	/*
	// can players damage each other? 
	virtual bool CanDealDamage(class AFMPlayerState* DamageInstigator, class AFMPlayerState* DamagedPlayer) const;

	// always create cheat manager 
	virtual bool AllowCheats(APlayerController* P) OVERRIDE;

	


protected:

	

	UPROPERTY(config)
		int32 TimeBetweenMatches;

	// score for kill 
	UPROPERTY(config)
		int32 KillScore;

	// score for death 
	UPROPERTY(config)
		int32 DeathScore;

	// scale for self instigated damage 
	UPROPERTY(config)
		float DamageSelfScale;

	UPROPERTY(config)
		int32 MaxBots;

	// check who won 
	virtual void DetermineMatchWinner();

	// check if PlayerState is a winner 
	virtual bool IsWinner(class AFMPlayerState* PlayerState) const;
	*/

	///////////////////////////////////////////////////////////////////////////////////////
	// MATCH CONTROL

	// delay between first player login and starting match 
	UPROPERTY(config)
	int32 WarmupTime;

	// match duration 
	UPROPERTY(config)
	int32 RoundTime;

	// update remaining time 
	virtual void DefaultTimer() OVERRIDE;

	// starts new match 
	virtual void StartMatch() OVERRIDE;

	// finish current match and lock players 
	UFUNCTION(exec)
	void FinishMatch();

	/////////////////////////////////////////////////////////////////////////////////////
	// PLAYER SPAWNING

	// always pick new random spawn 
	virtual bool ShouldSpawnAtStartSpot(AController* Player) OVERRIDE;

	// select best spawn point for player 
	virtual AActor* ChoosePlayerStart(AController* Player) OVERRIDE;

	// check if player can use spawnpoint 
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	// check if player should use spawnpoint 
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;
	
	////////////////////////////////////////////////////////////////////////////////////
	// GAME SESSION
	
	// Returns game session class to use 
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const OVERRIDE;
	
	/*
public:

	

	// get the name of the bots count option used in server travel URL 
	static FString GetBotsCountOptionName();

	//UPROPERTY()
	//	TArray<class AShooterPickup*> LevelPickups;

	*/
};


