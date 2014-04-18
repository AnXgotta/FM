// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMGameState.h"


AFMGameState::AFMGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	NumTeams = 0;
	RemainingTime = 0;
	bTimerPaused = false;
}


void AFMGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFMGameState, NumTeams);
	DOREPLIFETIME(AFMGameState, RemainingTime);
	DOREPLIFETIME(AFMGameState, bTimerPaused);
	DOREPLIFETIME(AFMGameState, TeamScores);
}

void AFMGameState::GetRankedMap(int32 TeamIndex, RankedPlayerMap& OutRankedMap) const {
	OutRankedMap.Empty();

	//first, we need to go over all the PlayerStates, grab their score, and rank them
	TMultiMap<int32, AFMPlayerState*> SortedMap;
	for (int32 i = 0; i < PlayerArray.Num(); ++i)	{
		int32 Score = 0;
		AFMPlayerState* CurPlayerState = Cast<AFMPlayerState>(PlayerArray[i]);
		if (CurPlayerState && (CurPlayerState->GetTeamNum() == TeamIndex))		{
			SortedMap.Add(FMath::Trunc(CurPlayerState->Score), CurPlayerState);
		}
	}

	//sort by the keys
	SortedMap.KeySort(TGreater<int32>());

	//now, add them back to the ranked map
	OutRankedMap.Empty();

	int32 Rank = 0;
	for (TMultiMap<int32, AFMPlayerState*>::TIterator It(SortedMap); It; ++It)	{
		OutRankedMap.Add(Rank++, It.Value());
	}

}