// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMGameState.h"
#include "FMGameMode_FreeForAll.h"


AFMGameMode_FreeForAll::AFMGameMode_FreeForAll(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bDelayedStart = true;
}
/*
void AFMGameMode_FreeForAll::DetermineMatchWinner(){
	AFMGameState const* const MyGameState = CastChecked<AFMGameState>(GameState);
	float BestScore = MAX_FLT;
	int32 BestPlayer = -1;
	int32 NumBestPlayers = 0;

	for (int32 i = 0; i < MyGameState->PlayerArray.Num(); i++){
		const float PlayerScore = MyGameState->PlayerArray[i]->Score;
		if (BestScore < PlayerScore)		{
			BestScore = PlayerScore;
			BestPlayer = i;
			NumBestPlayers = 1;
		}else if (BestScore == PlayerScore)	{
			NumBestPlayers++;
		}
	}

	WinnerPlayerState = (NumBestPlayers == 1) ? Cast<AFMPlayerState>(MyGameState->PlayerArray[BestPlayer]) : NULL;
}

bool AFMGameMode_FreeForAll::IsWinner(class AFMPlayerState* PlayerState) const {
	return PlayerState && PlayerState == WinnerPlayerState;
}
*/