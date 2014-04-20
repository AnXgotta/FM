// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FMTypes.h"
#include "GameFramework/HUD.h"
#include "FMHUD.generated.h"


struct FHitData{
	
	// last hit time
	float hitTime;

	// strength of last hit
	float hitPercentage;

	// defaults
	FHitData(){
		hitTime = 0.0f;
		hitPercentage = 0.0f;
	}

};

struct FDeathMessage{

	// player name scoring kill
	FString killerName;

	// player name killed
	FString victimName;

	// is killer enemy for local player
	uint8 bKillerIsEnemy : 1;

	// is local player killer
	uint8 bKillerIsOwner : 1;

	// is vitim enemy for local player
	uint8 bVictimIsEnemy : 1;

	// is victim local player (From docs... unclear -> "local player is killer")
	uint8 bVictimIsOwner : 1;

	// timestamp for message removal
	float hideTime;

	// what killed the player
	TWeakObjectPtr<class UFMDamageType> damageType;

	// defaults

	FDeathMessage()
		: bKillerIsEnemy(false)
			, bKillerIsOwner(false)
			, bVictimIsEnemy(false)
			, bVictimIsOwner(false)
			, hideTime(0.0f)
	{
	}
	


};

UCLASS()
class AFMHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	//////////////////////////////////////////////////////////////////
	// CLASS OVERRIDE FUNCTIONS
	
public:

	// main HUD update loop
	virtual void DrawHUD() OVERRIDE;


	// Sent from pawn hit, used to calculate hit notification overlay for drawing.
	// @param	DamageTaken		The amount of damage.
	// @param	DamageEvent		The actual damage event.
	// @param	PawnInstigator	The pawn that did the damage.
	//void NotifyHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator);

	// Notifies we have hit the enemy.
	//void NotifyEnemyHit();

	// State of match.
	EMatchState::Type matchState;

	// Set state of current match.
	// @param	NewState	The new match state.
	void SetMatchState(EMatchState::Type newState);

	// Get state of current match. 
	EMatchState::Type GetMatchState() const;


	// Toggles in game scoreboard.
	// Note:Will not display if the game menu is visible.
	// @param	bEnable	Required scoreboard display state.
	//void ShowScoreboard(bool bEnable);


	//////////////////////////////////////////////////////////////////////////
	// HEALTH/STAMINA BAR

	// health bar background icon
	//UPROPERTY()
	//	FCanvasIcon HealthBarBg;

	// health bar icon
	//UPROPERTY()
	//	FCanvasIcon HealthBar;

	// stamina bar background icon
	//UPROPERTY()
	//	FCanvasIcon HealthBarBg;

	// stamina bar icon
	//UPROPERTY()
	//	FCanvasIcon HealthBar;

	///////////////////////////////////////////////////////////////////////////
	// GENREAL UI SHIT

	// UI scaling factor for other resolutions than Full HD.
	float ScaleUI;

	// General offset for HUD elements. 
	float Offset;

	// Current animation pulse value.
	float PulseValue;

	// Lighter HUD color.
	FColor HUDLight;

	// Darker HUD color.
	FColor HUDDark;

	// FontRenderInfo enabling casting shadow.s
	//FFontRenderInfo ShadowedFont;

	// Large font - used for ammo display etc.
	//UPROPERTY()
	//	UFont* BigFont;

	// Normal font - used for death messages and such.
	UPROPERTY()
		UFont* NormalFont;

	// Converts floating point seconds to MM:SS string.
	// @param TimeSeconds		The time to get a string for.
	FString GetTimeString(float TimeSeconds);



	//////////////////////////////////////////////////////////////////////////
	// UI DRAW METHODS

	// Draw player's health bar.
	//void DrawHealth();

	// Draw player's stamina bar.
	//void DrawStamina();



	//////////////////////////////////////////////////////////////////////////
	// DEBUG INFO STRING

	// Temporary helper for drawing text-in-a-box.
	void DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor);


	void DrawDebugHealthStamina();

};
