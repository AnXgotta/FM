// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMGameMode.h"
#include "FMGameSession.h"
#include "FMGameState.h"
#include "FMPlayerController.h"
#include "FMCharacter.h"


AFMGameMode::AFMGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/_Blueprints/Player/BP_FMCharacter.BP_FMCharacter'"));
	if (PlayerPawnObject.Object != NULL){
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	PlayerControllerClass = AFMPlayerController::StaticClass();
	PlayerStateClass = AFMPlayerState::StaticClass();
	GameStateClass = AFMGameState::StaticClass();

}


void AFMGameMode::BeginPlay(){
	Super::BeginPlay();

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("GameMode: BeginPlay "));
	}


}

void AFMGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage){
	Super::InitGame(MapName, Options, ErrorMessage);
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("GameMode: InitGame"));
	}
}


/** Returns game session class to use */
TSubclassOf<AGameSession> AFMGameMode::GetGameSessionClass() const {
	return AFMGameSession::StaticClass();
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("GameMode: GetGameSessionClass "));
	}
}

void AFMGameMode::DefaultTimer(){
	Super::DefaultTimer();

	// don't update timers for Play In Editor mode, it's not real match
	if (GetWorld()->IsPlayInEditor()){
		return;
	}

	AFMGameState* const MyGameState = Cast<AFMGameState>(GameState);
	if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused){
		MyGameState->RemainingTime--;

		if (MyGameState->RemainingTime <= 0){
			if (MyGameState->bMatchIsOver){
				RestartGame();
			}else if (MyGameState->bMatchHasBegun){
				FinishMatch();
			}else{
				StartMatch();
			}
		}
	}
}

void AFMGameMode::StartMatch(){
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("GameMode: StartMatch"));
	}

	AFMGameState* const MyGameState = Cast<AFMGameState>(GameState);
	const bool bWasStarted = MyGameState && MyGameState->bMatchHasBegun;

	Super::StartMatch();

	const bool bIsStarted = MyGameState && MyGameState->bMatchHasBegun;
	if (!bWasStarted && bIsStarted)	{
		MyGameState->RemainingTime = RoundTime;
		bDelayedStart = false;

		// notify players
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)		{
			AFMPlayerController* PC = Cast<AFMPlayerController>(*It);
			if (PC)			{
				PC->ClientGameStarted();
			}
		}
	}
}

void AFMGameMode::FinishMatch(){
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("GameMode: FinishMatch"));
	}

	AFMGameState* const MyGameState = Cast<AFMGameState>(GameState);
	if (!MyGameState->bMatchIsOver)	{
		PerformEndGameHandling();
		DetermineMatchWinner();

		// notify players
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)		{
			AFMPlayerState* PlayerState = Cast<AFMPlayerState>((*It)->PlayerState);
			const bool bIsWinner = IsWinner(PlayerState);

			(*It)->GameHasEnded(NULL, bIsWinner);
		}

		// lock all pawns
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)		{
			(*It)->TurnOff();
		}

		// set up to restart the match
		MyGameState->RemainingTime = TimeBetweenMatches;
	}
}

void AFMGameMode::DetermineMatchWinner(){
	// nothing to do here
}

bool AFMGameMode::IsWinner(class AFMPlayerState* PlayerState) const {
	return false;
}

void AFMGameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr<FUniqueNetId>& UniqueId, FString& ErrorMessage){
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("GameMode: PreLogin "));
	}

	AFMGameState* const MyGameState = Cast<AFMGameState>(GameState);
	const bool bMatchIsOver = MyGameState && MyGameState->bMatchIsOver;
	const FString EndGameError = TEXT("Match is over!");

	ErrorMessage = bMatchIsOver ? *EndGameError : GameSession->ApproveLogin(Options);
}


void AFMGameMode::PostLogin(APlayerController* NewPlayer){
	Super::PostLogin(NewPlayer);

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("GameMode: PostLogin "));
	}

	// update spectator location for client
	AFMPlayerController* NewPC = Cast<AFMPlayerController>(NewPlayer);
	if (NewPC && NewPC->GetPawn() == NULL){
		NewPC->ClientSetSpectatorCamera(NewPC->GetSpawnLocation(), NewPC->GetControlRotation());
	}

	// start warmup if needed
	AFMGameState* const MyGameState = Cast<AFMGameState>(GameState);
	if (MyGameState && MyGameState->RemainingTime == 0)	{
		const bool bWantsMatchWarmup = bDelayedStart && !GetWorld()->IsPlayInEditor();
		if (bWantsMatchWarmup && WarmupTime > 0){
			MyGameState->RemainingTime = WarmupTime;
		}else{
			StartMatch();
		}
	}

	// notify new player if match is already in progress
	if (NewPC && MyGameState && MyGameState->bMatchHasBegun){
		NewPC->ClientGameStarted();
		NewPC->ClientStartOnlineGame();
	}
}

bool AFMGameMode::PlayerCanRestart(APlayerController* Player){
	AFMGameState const* const MyGameState = Cast<AFMGameState>(GameState);
	return MyGameState && MyGameState->bMatchHasBegun && !MyGameState->bMatchIsOver;
}

void AFMGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType){
	AFMPlayerState* KillerPlayerState = Killer ? Cast<AFMPlayerState>(Killer->PlayerState) : NULL;
	AFMPlayerState* VictimPlayerState = KilledPlayer ? Cast<AFMPlayerState>(KilledPlayer->PlayerState) : NULL;

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState){
		KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
		KillerPlayerState->InformAboutKill();
	}

	if (VictimPlayerState){
		VictimPlayerState->ScoreDeath(KillerPlayerState, DeathScore);
		VictimPlayerState->BroadcastDeath(KillerPlayerState, DamageType);
	}
}

float AFMGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const {
	float ActualDamage = Damage;

	AFMCharacter* DamagedPawn = Cast<AFMCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)	{
		AFMPlayerState* DamagedPlayerState = Cast<AFMPlayerState>(DamagedPawn->PlayerState);
		AFMPlayerState* InstigatorPlayerState = Cast<AFMPlayerState>(EventInstigator->PlayerState);

		// disable friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))		{
			ActualDamage = 0.0f;
		}

		// scale self instigated damage
		if (InstigatorPlayerState == DamagedPlayerState)		{
			ActualDamage *= DamageSelfScale;
		}
	}

	return ActualDamage;
}

bool AFMGameMode::CanDealDamage(class AFMPlayerState* DamageInstigator, class AFMPlayerState* DamagedPlayer) const {
	return true;
}

bool AFMGameMode::AllowCheats(APlayerController* P){
	return true;
}

bool AFMGameMode::ShouldSpawnAtStartSpot(AController* Player){
	return false;
}

UClass* AFMGameMode::GetDefaultPawnClassForController(AController* InController){
	return Super::GetDefaultPawnClassForController(InController);
}

AActor* AFMGameMode::ChoosePlayerStart(AController* Player){
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, DEBUG_MSG_TIME, FColor::Red, TEXT("GameMode: ChoosePlayerStart "));
	}

	for (int32 i = 0; i < PlayerStarts.Num(); i++)	{
		APlayerStart* TestSpawn = PlayerStarts[i];
		if (IsSpawnpointAllowed(TestSpawn, Player))		{
			if (IsSpawnpointPreferred(TestSpawn, Player))			{
				PreferredSpawns.Add(TestSpawn);
			}else{
				FallbackSpawns.Add(TestSpawn);
			}
		}
	}

	APlayerStart* BestStart = NULL;
	if (PreferredSpawns.Num() > 0){
		BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
	} else if (FallbackSpawns.Num() > 0) {
		BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart(Player);
}

bool AFMGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const {
	/*
	AShooterTeamStart* ShooterSpawnPoint = Cast<AShooterTeamStart>(SpawnPoint);
	if (ShooterSpawnPoint){
		AShooterAIController* AIController = Cast<AShooterAIController>(Player);
		if (ShooterSpawnPoint->bNotForBots && AIController)
		{
			return false;
		}

		if (ShooterSpawnPoint->bNotForPlayers && AIController == NULL)
		{
			return false;
		}
	}
	*/
	return true;
}

bool AFMGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
{
	ACharacter* MyPawn = Player ? Cast<ACharacter>(Player->GetPawn()) : NULL;
	if (MyPawn)	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn && OtherPawn != MyPawn)			{
				const float CombinedHeight = (MyPawn->CapsuleComponent->GetScaledCapsuleHalfHeight() + OtherPawn->CapsuleComponent->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = MyPawn->CapsuleComponent->GetScaledCapsuleRadius() + OtherPawn->CapsuleComponent->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// check if player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius){
					return false;
				}
			}
		}
	}

	return true;
}
