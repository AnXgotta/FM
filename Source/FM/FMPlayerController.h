// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Online.h"
#include "GameFramework/PlayerController.h"
#include "FMPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AFMPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:
	/** Delegate for starting a session */
//	FOnEndSessionCompleteDelegate OnStartSessionCompleteEndItDelegate;

	/** Delegate for ending a session */
	//FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;

	/** Delegate for destroying a session */
	//FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Delegate called when we have read/failed to read the achievements from the server */
//	FOnAchievementsReadDelegate OnAchievementsReadDelegate;

	/** sets spectator location and rotation */
	UFUNCTION(reliable, client)
		void ClientSetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation);

	/** notify player about started match */
	UFUNCTION(reliable, client)
		void ClientGameStarted();

	/** Starts the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
		void ClientStartOnlineGame();

	/** Ends the online game using the session name in the PlayerState */
	UFUNCTION(reliable, client)
		void ClientEndOnlineGame();

	/** notify player about finished match */
	virtual void ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner);

	/** used for input simulation from blueprint (for automatic perf tests) */
	UFUNCTION(BlueprintCallable, Category = "Input")
		void SimulateInputKey(FKey Key, bool bPressed = true);

	/** sends cheat message */
//	UFUNCTION(reliable, server, WithValidation)
//		void ServerCheat(const FString& Msg);

	/* Overriden Message implementation. */
	virtual void ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime) OVERRIDE;

	/* Tell the HUD to toggle the chat window. */
	void ToggleChatWindow();

	/** Local function say a string */
	UFUNCTION(exec)
		virtual void Say(const FString& Msg);

	/** RPC for clients to talk to server */
	UFUNCTION(unreliable, server, WithValidation)
		void ServerSay(const FString& Msg);

	/** Local function run an emote */
	// 	UFUNCTION(exec)
	// 	virtual void Emote(const FString& Msg);

	/** notify local client about deaths */
	void OnDeathMessage(class AFMPlayerState* KillerPlayerState, class AFMPlayerState* KilledPlayerState, const UDamageType* KillerDamageType);

	/** toggle InGameMenu handler */
	void OnToggleInGameMenu();

	/** shows scoreboard */
	void OnShowScoreboard();

	/** hides scoreboard */
	void OnHideScoreboard();

	/** set health regen cheat */
	void SetHealthRegen(bool bEnable);

	/** check if gameplay related actions (movement, weapon usage, etc) are allowed right now */
	bool IsGameInputAllowed() const;

	/** is game menu currently active? */
	bool IsGameMenuVisible() const;

	/** Ends and/or destroys game session */
	void CleanupSessionOnReturnToMenu();

	/**
	* Delegate fired when starting an online session has completed (intends to end it, but has to wait for the start to complete first)
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	//virtual void OnStartSessionCompleteEndIt(FName SessionName, bool bWasSuccessful);

	/**
	* Delegate fired when ending an online session has completed
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
//	virtual void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	* Delegate fired when destroying an online session has completed
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	//virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	* Called when the read achievements request from the server is complete
	*
	* @param PlayerId The player id who is responsible for this delegate being fired
	* @param bWasSuccessful true if the server responded successfully to the request
	*/
//	void OnAchievementsRead(const FUniqueNetId& PlayerId, bool bWasSuccessful);

	// Begin APlayerController interface

	/** handle weapon visibility */
	virtual void SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning) OVERRIDE;

	/** initialize the input system from the player settings */
	//virtual void InitInputSystem() OVERRIDE;

	//virtual bool SetPause(bool bPause, FCanUnpause CanUnpauseDelegate = FCanUnpause()) OVERRIDE;

	// End APlayerController interface

	// begin AShooterPlayerController-specific

	/**
	* Reads achievements to precache them before first use
	*/
	void ReadAchievements();

	/**
	* Writes a single achievement (unless another write is in progress).
	*
	* @param Id achievement id (string)
	* @param Percent number 1 to 100
	*/
	void UpdateAchievementProgress(const FString & Id, float Percent);

	/** Returns a pointer to the shooter game hud. May return NULL. */
	//AShooterHUD* GetShooterHUD() const;

	/** Returns the persistent user record associated with this player, or null if there is't one. */
	//class UFMPersistentUser* GetPersistentUser() const;

	/** Informs that player fragged someone */
	void OnKill();

	// end AShooterPlayerController-specific

protected:

	/** if set, gameplay related actions (movement, weapn usage, etc) are allowed */
	uint8 bAllowGameActions : 1;

	/** shooter in-game menu */
	//TSharedPtr<class FShooterIngameMenu> ShooterIngameMenu;

	/** Achievements write object */
	FOnlineAchievementsWritePtr WriteObject;

	/** try to find spot for death cam */
	bool FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation);

	//Begin AActor interface

	/** after all game elements are created */
	virtual void PostInitializeComponents() OVERRIDE;

	//End AActor interface

	//Begin AController interface

	/** transition to dead state, retries spawning later */
	virtual void FailedToSpawnPawn() OVERRIDE;

	/** update camera when pawn dies */
	virtual void PawnPendingDestroy(APawn* P) OVERRIDE;

	//End AController interface

	// Begin APlayerController interface

	/** respawn after dying */
	virtual void UnFreeze() OVERRIDE;

	/** sets up input */
	virtual void SetupInputComponent() OVERRIDE;

	/**
	* Called from game info upon end of the game, used to transition to proper state.
	*
	* @param EndGameFocus Actor to set as the view target on end game
	* @param bIsWinner true if this controller is on winning team
	*/
	virtual void GameHasEnded(class AActor* EndGameFocus = NULL, bool bIsWinner = false) OVERRIDE;

	/** Return the client to the main menu gracefully */
	void ClientReturnToMainMenu_Implementation(const FString& ReturnReason) OVERRIDE;

	/** Causes the player to commit suicide */
	UFUNCTION(exec)
		virtual void Suicide();

	/** Notifies the server that the client has suicided */
	UFUNCTION(reliable, server, WithValidation)
		void ServerSuicide();

	// End APlayerController interface

	FName	ServerSayString;
	
};
