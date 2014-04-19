// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
//#include "EnigneUserInterfaceClasses.h"
#include "FMCharacter.h"
#include "FMWeapon.h"
#include "FMPlayerState.h"
#include "FMPlayerController.h"

AFMPlayerController::AFMPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//PlayerCameraManagerClass = AShooterPlayerCameraManager::StaticClass();

	bAllowGameActions = true;

	if (!HasAnyFlags(RF_ClassDefaultObject)){
		OnStartSessionCompleteEndItDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &AFMPlayerController::OnStartSessionCompleteEndIt);
		OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &AFMPlayerController::OnEndSessionComplete);
		OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &AFMPlayerController::OnDestroySessionComplete);
		OnAchievementsReadDelegate = FOnAchievementsReadDelegate::CreateUObject(this, &AFMPlayerController::OnAchievementsRead);
	}
	ServerSayString = TEXT("Say");
}


void AFMPlayerController::SetupInputComponent(){
	Super::SetupInputComponent();

	// UI input
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AFMPlayerController::OnToggleInGameMenu);
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &AFMPlayerController::OnShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &AFMPlayerController::OnHideScoreboard);

	// voice chat
	InputComponent->BindAction("PushToTalk", IE_Pressed, this, &APlayerController::StartTalking);
	InputComponent->BindAction("PushToTalk", IE_Released, this, &APlayerController::StopTalking);

	InputComponent->BindAction("ToggleChat", IE_Pressed, this, &AFMPlayerController::ToggleChatWindow);
}


void AFMPlayerController::PostInitializeComponents(){
	Super::PostInitializeComponents();

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("PlayerController: PostInit "));
	}
	//Build menu only after game is initialized
	//FShooterStyle::Initialize();
	//ShooterIngameMenu = MakeShareable(new FShooterIngameMenu());
	//ShooterIngameMenu->Construct(this);
}

void AFMPlayerController::ReadAchievements(){
	// precache achievements
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer){
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub){
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())	{
				TSharedPtr<FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->ControllerId);

				if (UserId.IsValid()){
					IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

					if (Achievements.IsValid())	{
						Achievements->AddOnAchievementsReadDelegate(OnAchievementsReadDelegate);
						Achievements->ReadAchievements(*UserId.Get());
					}
				}else{
					UE_LOG(LogOnline, Warning, TEXT("No valid user id for this controller."));
				}
			}else{
				UE_LOG(LogOnline, Warning, TEXT("No valid identity interface."));
			}
		}else{
			UE_LOG(LogOnline, Warning, TEXT("No default online subsystem."));
		}
	}else{
		UE_LOG(LogOnline, Warning, TEXT("No local player, cannot read achievements."));
	}
}

void AFMPlayerController::OnAchievementsRead(const FUniqueNetId& PlayerId, bool bWasSuccessful){
	UE_LOG(LogOnline, Display, TEXT("AShooterPlayerController::OnAchievementsRead(bWasSuccessful = %s)"), bWasSuccessful ? TEXT("TRUE") : TEXT("FALSE"));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)	{
		IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();
		if (Achievements.IsValid())		{
			// Clear our delegate and if successful read achievements
			Achievements->ClearOnAchievementsReadDelegate(OnAchievementsReadDelegate);
		}
	}
}

void AFMPlayerController::UnFreeze(){
	ServerRestartPlayer();
}

void AFMPlayerController::FailedToSpawnPawn(){
	if (StateName == NAME_Inactive){
		BeginInactiveState();
	}
	Super::FailedToSpawnPawn();
}

void AFMPlayerController::PawnPendingDestroy(APawn* P){
	FVector CameraLocation = P->GetActorLocation() + FVector(0, 0, 300.0f);
	FRotator CameraRotation(-90.0f, 0.0f, 0.0f);
	FindDeathCameraSpot(CameraLocation, CameraRotation);

	Super::PawnPendingDestroy(P);

	ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

void AFMPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner){
	// write stats
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer){
		AFMPlayerState* FMPlayerState = Cast<AFMPlayerState>(PlayerState);
		if (FMPlayerState){
/*			// update local saved profile
			UShooterPersistentUser* const PersistentUser = GetPersistentUser();
			if (PersistentUser)			{
				PersistentUser->AddMatchResult(FMPlayerState->GetKills(), FMPlayerState->GetDeaths(), bIsWinner);
				PersistentUser->SaveIfDirty();
			}
*/
			// update leaderboards
			IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
			if (OnlineSub){
				IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
				if (Identity.IsValid()){
					TSharedPtr<FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->ControllerId);
					if (UserId.IsValid()){
						IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
						if (Leaderboards.IsValid()){
							/*
							FShooterAllTimeMatchResultsWrite WriteObject;

							WriteObject.SetIntStat(LEADERBOARD_STAT_SCORE, FMPlayerState->GetKills());
							WriteObject.SetIntStat(LEADERBOARD_STAT_KILLS, FMPlayerState->GetKills());
							WriteObject.SetIntStat(LEADERBOARD_STAT_DEATHS, FMPlayerState->GetDeaths());
							WriteObject.SetIntStat(LEADERBOARD_STAT_MATCHESPLAYED, 1);

							// the call will copy the user id and write object to its own memory
							Leaderboards->WriteLeaderboards(FMPlayerState->SessionName, *UserId, WriteObject);
							*/
						}

						//UpdateAchievementProgress(ACH_FINISH_MATCH, 100.0f);
					}
				}
			}
		}
	}

	Super::GameHasEnded(EndGameFocus, bIsWinner);
}

void AFMPlayerController::ClientSetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation){
	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
	SetViewTarget(this);
}

bool AFMPlayerController::FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation){
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	FRotator ViewDir = GetControlRotation();
	ViewDir.Pitch = -45.0f;

	const float YawOffsets[] = { 0.0f, -180.0f, 90.0f, -90.0f, 45.0f, -45.0f, 135.0f, -135.0f };
	const float CameraOffset = 600.0f;
	FCollisionQueryParams TraceParams(TEXT("DeathCamera"), true, GetPawn());

	for (int32 i = 0; i < ARRAY_COUNT(YawOffsets); i++)	{
		FRotator CameraDir = ViewDir;
		CameraDir.Yaw += YawOffsets[i];
		CameraDir.Normalize();

		const FVector TestLocation = PawnLocation - CameraDir.Vector() * CameraOffset;
		const bool bBlocked = GetWorld()->LineTraceTest(PawnLocation, TestLocation, ECC_Camera, TraceParams);

		if (!bBlocked)	{
			CameraLocation = TestLocation;
			CameraRotation = CameraDir;
			return true;
		}
	}

	return false;
}


void AFMPlayerController::SimulateInputKey(FKey Key, bool bPressed){
	InputKey(Key, bPressed ? IE_Pressed : IE_Released, 1, false);
}

void AFMPlayerController::OnKill(){
	//UpdateAchievementProgress(ACH_FRAG_SOMEONE, 100.0f);
}

void AFMPlayerController::OnDeathMessage(class AFMPlayerState* KillerPlayerState, class AFMPlayerState* KilledPlayerState, const UDamageType* KillerDamageType){
	/*
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->ShowDeathMessage(KillerPlayerState, KilledPlayerState, KillerDamageType);

		UpdateAchievementProgress(ACH_DIE_ONCE, 100.0f);
	}
	*/
}

void AFMPlayerController::UpdateAchievementProgress(const FString& Id, float Percent)
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub){
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())	{
				TSharedPtr<FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->ControllerId);

				if (UserId.IsValid()){

					IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();
					if (Achievements.IsValid() && (!WriteObject.IsValid() || WriteObject->WriteState != EOnlineAsyncTaskState::InProgress))	{
						WriteObject = MakeShareable(new FOnlineAchievementsWrite());
						FOnlineAchievementsWriteRef WriteObjectRef = WriteObject.ToSharedRef();

						WriteObject->SetFloatStat(*Id, Percent);

						if (!Achievements->WriteAchievements(*UserId.Get(), WriteObjectRef)){
							UE_LOG(LogOnline, Warning, TEXT("WriteAchievements() failed"));
						}
					}else{
						UE_LOG(LogOnline, Warning, TEXT("No valid achievement interface or another write is in progress."));
					}
				}else{
					UE_LOG(LogOnline, Warning, TEXT("No valid user id for this controller."));
				}
			}else{
				UE_LOG(LogOnline, Warning, TEXT("No valid identity interface."));
			}
		}else{
			UE_LOG(LogOnline, Warning, TEXT("No default online subsystem."));
		}
	}else{
		UE_LOG(LogOnline, Warning, TEXT("No local player, cannot update achievements."));
	}
}

void AFMPlayerController::OnToggleInGameMenu(){
	/*
	if (ShooterIngameMenu.IsValid())
	{
		ShooterIngameMenu->ToggleGameMenu();
	}
	*/
}

void AFMPlayerController::OnShowScoreboard(){
	/*
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->ShowScoreboard(true);
	}
	*/
}

void AFMPlayerController::OnHideScoreboard(){
	/*
	AShooterHUD* ShooterHUD = GetShooterHUD();
	// If have a valid match and the match is over - hide the scoreboard
	if ((ShooterHUD != NULL) && (ShooterHUD->IsMatchOver() == false))
	{
		ShooterHUD->ShowScoreboard(false);
	}
	*/
}

bool AFMPlayerController::IsGameMenuVisible() const {
	bool Result = false;
	/*
	if (ShooterIngameMenu.IsValid())
	{
		Result = ShooterIngameMenu->GetIsGameMenuUp();
	}
	*/

	return Result;
}

void AFMPlayerController::ClientGameStarted_Implementation(){
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Yellow, TEXT("PlayerController: GameStarted"));
	}
	/*
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->SetMatchState(EMatchState::Playing);
	}

	ReadAchievements();
	*/
}

/** Starts the online game using the session name in the PlayerState */
void AFMPlayerController::ClientStartOnlineGame_Implementation(){
	if (!IsPrimaryPlayer())	return;

	AFMPlayerState* FMPlayerState = Cast<AFMPlayerState>(PlayerState);
	if (FMPlayerState){
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub){
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid()){
				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *FMPlayerState->SessionName.ToString());
				Sessions->StartSession(FMPlayerState->SessionName);
			}
		}
	}else{
		// Keep retrying until player state is replicated
		GetWorld()->GetTimerManager().SetTimer(FTimerDelegate::CreateUObject(this, &AFMPlayerController::ClientStartOnlineGame_Implementation), 0.2f, false);
	}
}

/** Ends the online game using the session name in the PlayerState */
void AFMPlayerController::ClientEndOnlineGame_Implementation(){
	if (!IsPrimaryPlayer())	return;

	AFMPlayerState* FMPlayerState = Cast<AFMPlayerState>(PlayerState);
	if (FMPlayerState){
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub){
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid()){
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *FMPlayerState->SessionName.ToString());
				Sessions->EndSession(FMPlayerState->SessionName);
			}
		}
	}
}

void AFMPlayerController::ClientReturnToMainMenu_Implementation(const FString& ReturnReason){
	OnHideScoreboard();
	CleanupSessionOnReturnToMenu();
}

/** Ends and/or destroys game session */
void AFMPlayerController::CleanupSessionOnReturnToMenu(){
	bool bPendingOnlineOp = false;

	// end online game and then destroy it
	AFMPlayerState* FMPlayerState = Cast<AFMPlayerState>(PlayerState);
	if (FMPlayerState){
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub){
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())	{
				EOnlineSessionState::Type SessionState = Sessions->GetSessionState(FMPlayerState->SessionName);
				UE_LOG(LogOnline, Log, TEXT("Session %s is '%s'"), *FMPlayerState->SessionName.ToString(), EOnlineSessionState::ToString(SessionState));

				if (EOnlineSessionState::InProgress == SessionState){
					UE_LOG(LogOnline, Log, TEXT("Ending session %s on return to main menu"), *FMPlayerState->SessionName.ToString());
					Sessions->AddOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
					bPendingOnlineOp = Sessions->EndSession(FMPlayerState->SessionName);
					if (!bPendingOnlineOp)	{
						Sessions->ClearOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
					}
				}else if (EOnlineSessionState::Ending == SessionState){
					UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to end on return to main menu"), *FMPlayerState->SessionName.ToString());
					Sessions->AddOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
					bPendingOnlineOp = true;
				}else if (EOnlineSessionState::Ended == SessionState ||	EOnlineSessionState::Pending == SessionState){
					UE_LOG(LogOnline, Log, TEXT("Destroying session %s on return to main menu"), *FMPlayerState->SessionName.ToString());
					Sessions->AddOnDestroySessionCompleteDelegate(OnDestroySessionCompleteDelegate);
					bPendingOnlineOp = Sessions->DestroySession(FMPlayerState->SessionName);
					if (!bPendingOnlineOp){
						Sessions->ClearOnDestroySessionCompleteDelegate(OnDestroySessionCompleteDelegate);
					}
				}else if (EOnlineSessionState::Starting == SessionState){
					UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to start, and then we will end it to return to main menu"), *FMPlayerState->SessionName.ToString());
					Sessions->AddOnStartSessionCompleteDelegate(OnStartSessionCompleteEndItDelegate);
					bPendingOnlineOp = true;
				}
			}
		}
	}

	if (!bPendingOnlineOp){
		GEngine->HandleDisconnect(GetWorld(), GetWorld()->GetNetDriver());
	}
}

void AFMPlayerController::OnStartSessionCompleteEndIt(FName SessionName, bool bWasSuccessful){
	UE_LOG(LogOnline, Log, TEXT("OnStartSessionCompleteEndIt: Session=%s bWasSuccessful=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub){
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())	{
			Sessions->ClearOnStartSessionCompleteDelegate(OnStartSessionCompleteEndItDelegate);
		}
	}

	// continue
	CleanupSessionOnReturnToMenu();
}

/**
* Delegate fired when ending an online session has completed
*
* @param SessionName the name of the session this callback is for
* @param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void AFMPlayerController::OnEndSessionComplete(FName SessionName, bool bWasSuccessful){
	UE_LOG(LogOnline, Log, TEXT("OnEndSessionComplete: Session=%s bWasSuccessful=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub){
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())	{
			Sessions->ClearOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
		}
	}

	// continue
	CleanupSessionOnReturnToMenu();
}

/**
* Delegate fired when destroying an online session has completed
*
* @param SessionName the name of the session this callback is for
* @param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void AFMPlayerController::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful){
	UE_LOG(LogOnline, Log, TEXT("OnDestroySessionComplete: Session=%s bWasSuccessful=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub){
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())	{
			Sessions->ClearOnDestroySessionCompleteDelegate(OnDestroySessionCompleteDelegate);
		}
	}

	// continue
	CleanupSessionOnReturnToMenu();
}

void AFMPlayerController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner){
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	// Allow only looking around
	SetIgnoreMoveInput(true);
	bAllowGameActions = false;

	// Make sure that we still have valid view target
	SetViewTarget(GetPawn());

/*	// Show scoreboard
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)	{
		ShooterHUD->SetMatchState(bIsWinner ? EMatchState::Won : EMatchState::Lost);
		if (IsPrimaryPlayer())
		{
			ShooterHUD->ShowScoreboard(true);
		}
	}
	*/
}

void AFMPlayerController::SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning){
	Super::SetCinematicMode(bInCinematicMode, bHidePlayer, bAffectsHUD, bAffectsMovement, bAffectsTurning);
	/*
	// If we have a pawn we need to determine if we should show/hide the weapon
	AFMCharacter* MyPawn = Cast<AFMCharacter>(GetPawn());
	AFMWeapon* MyWeapon = MyPawn ? MyPawn->GetWeapon() : NULL;
	if (MyWeapon){
		if (bInCinematicMode && bHidePlayer){
			MyWeapon->SetActorHiddenInGame(true);
		}else if (!bCinematicMode){
			MyWeapon->SetActorHiddenInGame(false);
		}
	}
	*/
}

void AFMPlayerController::InitInputSystem(){
	Super::InitInputSystem();

	if (PlayerInput != NULL){
		//UShooterGameUserSettings *UserSettings = CastChecked<UShooterGameUserSettings>(GEngine->GetGameUserSettings());
		//UserSettings->ApplySettings();
	}
}

/*
void AFMPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
*/

void AFMPlayerController::Suicide(){
	if (IsInState(NAME_Playing)){
		ServerSuicide();
	}
}

bool AFMPlayerController::ServerSuicide_Validate(){
	return true;
}

void AFMPlayerController::ServerSuicide_Implementation(){
	if ((GetPawn() != NULL) && ((GetWorld()->TimeSeconds - GetPawn()->CreationTime > 1) || (GetNetMode() == NM_Standalone))){
		AFMCharacter* MyPawn = Cast<AFMCharacter>(GetPawn());
		if (MyPawn)	{
			//MyPawn->Suicide();
		}
	}
}

bool AFMPlayerController::IsGameInputAllowed() const {
	return bAllowGameActions && !bCinematicMode;
}

void AFMPlayerController::ToggleChatWindow(){
	/*
	AShooterHUD* ShooterHUD = Cast<AShooterHUD>(GetHUD());
	if (ShooterHUD)
	{
		ShooterHUD->ToggleChat();
	}
	*/
}

void AFMPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime){
	/*
	AShooterHUD* ShooterHUD = Cast<AShooterHUD>(GetHUD());
	if (ShooterHUD)
	{
		if (Type == ServerSayString)
		{
			if (SenderPlayerState != PlayerState)
			{
				ShooterHUD->AddChatLine(S);
			}
		}
	}
	*/
}

void AFMPlayerController::Say(const FString& Msg){
	ServerSay(Msg.Left(128));
}

bool AFMPlayerController::ServerSay_Validate(const FString& Msg){
	return true;
}

void AFMPlayerController::ServerSay_Implementation(const FString& Msg){
	GetWorld()->GetAuthGameMode()->Broadcast(this, Msg, ServerSayString);
}

/*
AShooterHUD* AShooterPlayerController::GetShooterHUD() const
{
	return Cast<AShooterHUD>(GetHUD());
}
*/

/*
UFMPersistentUser* AFMPlayerController::GetPersistentUser() const
{
	UShooterLocalPlayer* const ShooterLP = Cast<UShooterLocalPlayer>(Player);
	if (ShooterLP)
	{
		return ShooterLP->PersistentUser;
	}

	return NULL;
}
*/

bool AFMPlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate /*= FCanUnpause()*/){
	const bool Result = APlayerController::SetPause(bPause, CanUnpauseDelegate);

	// Update rich presence.
	const auto PresenceInterface = Online::GetPresenceInterface();
	if (PresenceInterface.IsValid() && PlayerState->UniqueId.IsValid()){
		FPresenceProperties Props;
		if (Result && bPause){
			Props.Add(DefaultPresenceKey, FString("Paused"));
		}else{
			Props.Add(DefaultPresenceKey, FString("InGame"));
		}
		PresenceInterface->SetPresence(*PlayerState->UniqueId, Props);
	}

	return Result;
}