// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.sssssssssss
#pragma once

#include "GameFramework/PlayerState.h"
#include "FMPlayerState.generated.h"


UCLASS()
class AFMPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()

	// Begin APlayerState interface
	// clear scores
	virtual void Reset() OVERRIDE;

	// Set the team
	// @param	InController	The controller to initialize state with
	virtual void ClientInitialize(class AController* InController) OVERRIDE;

	// End APlayerState interface

	// Set new team and update pawn. Also updates player character team colors.
	// @param	NewTeamNumber	Team we want to be on.
	void SetTeamNum(int32 NewTeamNumber);

	UFUNCTION(Reliable, Client)
		void Client_OnSetTeam();

	// player killed someone 
	void ScoreKill(AFMPlayerState* Victim, int32 Points);

	// player died 
	void ScoreDeath(AFMPlayerState* KilledBy, int32 Points);

	// get current team 
	int32 GetTeamNum() const;

	// get number of kills 
	int32 GetKills() const;

	// get number of deaths 
	int32 GetDeaths() const;

	// gets truncated player name to fit in death log and scoreboards 
	FString GetShortPlayerName() const;

	// Sends kill (excluding self) to clients 
	UFUNCTION(Reliable, Client)
		void InformAboutKill();

	// broadcast death to local clients 
	UFUNCTION(Reliable, NetMulticast)
		void BroadcastDeath(class AFMPlayerState* KillerPlayerState, const UDamageType* KillerDamageType);

protected:

	// team 
	UPROPERTY(Transient, Replicated)
		int32 TeamNumber;

	// number of kills 
	UPROPERTY(Transient, Replicated)
		int32 NumKills;

	// number of deaths 
	UPROPERTY(Transient, Replicated)
		int32 NumDeaths;

	// helper for scoring points 
	void ScorePoints(int32 Points);
	
};
