// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "FMGameMode.h"
#include "FMGameMode_FreeForAll.generated.h"

/**
 * 
 */
UCLASS()
class AFMGameMode_FreeForAll : public AFMGameMode
{
	GENERATED_UCLASS_BODY()

protected:

	/** best player */
	UPROPERTY(transient)
	class AFMPlayerState* WinnerPlayerState;

	/** check who won */
	virtual void DetermineMatchWinner() OVERRIDE;

	/** check if PlayerState is a winner */
	virtual bool IsWinner(class AFMPlayerState* PlayerState) const OVERRIDE;
	
};
