// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameState.h"
#include "FMPlayerState.h"
#include "FMGameState.generated.h"


/** ranked PlayerState map, created from the GameState */
typedef TMap<int32, TWeakObjectPtr<AFMPlayerState> > RankedPlayerMap;

UCLASS()
class AFMGameState : public AGameState
{
	GENERATED_UCLASS_BODY()

	// number of teams in current game 
	UPROPERTY(Transient, Replicated)
	int32 NumTeams;

	// accumulated score per team 
	UPROPERTY(Transient, Replicated)
		TArray<int32> TeamScores;

	// time left for warmup / match 
	UPROPERTY(Transient, Replicated)
		int32 RemainingTime;

	// is timer paused? 
	UPROPERTY(Transient, Replicated)
		bool bTimerPaused;

	// gets ranked PlayerState map for specific team 
	void GetRankedMap(int32 TeamIndex, RankedPlayerMap& OutRankedMap) const;
	
};
