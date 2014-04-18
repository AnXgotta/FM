// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMPlayerState.h"
#include "FMGameMode.h"
#include "FMGameState.h"
#include "FMPlayerController.h"
#include "FMCharacter.h"


AFMPlayerState::AFMPlayerState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
}


void AFMPlayerState::Reset(){
	Super::Reset();

	SetTeamNum(0);
	NumKills = 0;
	NumDeaths = 0;
}

void AFMPlayerState::ClientInitialize(class AController* InController){
	Super::ClientInitialize(InController);

	Client_OnSetTeam();
}

void AFMPlayerState::SetTeamNum(int32 NewTeamNumber){
	TeamNumber = NewTeamNumber;

	Client_OnSetTeam();
}

void AFMPlayerState::Client_OnSetTeam_Implementation(){
	AFMPlayerController* OwnerController = Cast<AFMPlayerController>(GetOwner());
	if (OwnerController != NULL){
		// Colors are relative to the local player's team, so if the local player changes team everyone's color needs to be updated
		if(OwnerController == GEngine->GetFirstLocalPlayerController(GetWorld())){
			for (TActorIterator<AFMCharacter> CharacterIt(GetWorld()); CharacterIt; ++CharacterIt){
				AFMCharacter* Character = *CharacterIt;
				//Character->UpdateTeamColorsAllMIDs();
			}
		}else		{
			// Update only my color
			AFMCharacter* ShooterCharacter = Cast<AFMCharacter>(OwnerController->GetCharacter());
			if (ShooterCharacter != NULL)			{
				//ShooterCharacter->UpdateTeamColorsAllMIDs();
			}
		}
	}
}

int32 AFMPlayerState::GetTeamNum() const{
	return TeamNumber;
}

int32 AFMPlayerState::GetKills() const{
	return NumKills;
}

int32 AFMPlayerState::GetDeaths() const{
	return NumDeaths;
}

void AFMPlayerState::ScoreKill(AFMPlayerState* Victim, int32 Points){
	NumKills++;
	ScorePoints(Points);
}

void AFMPlayerState::ScoreDeath(AFMPlayerState* KilledBy, int32 Points){
	NumDeaths++;
	ScorePoints(Points);
}

void AFMPlayerState::ScorePoints(int32 Points){
	AFMGameState* const MyGameState = Cast<AFMGameState>(GetWorld()->GameState);
	if (MyGameState && TeamNumber >= 0){
		if (TeamNumber >= MyGameState->TeamScores.Num())		{
			MyGameState->TeamScores.AddZeroed(TeamNumber - MyGameState->TeamScores.Num() + 1);
		}

		MyGameState->TeamScores[TeamNumber] += Points;
	}

	Score += Points;
}

void AFMPlayerState::InformAboutKill_Implementation(){
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)	{
		AFMPlayerController* TestPC = Cast<AFMPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())		{
			//TestPC->OnKill();
		}
	}
}

void AFMPlayerState::BroadcastDeath_Implementation(class AFMPlayerState* KillerPlayerState, const UDamageType* KillerDamageType){
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)	{
		AFMPlayerController* TestPC = Cast<AFMPlayerController>(*It);
		if (TestPC && TestPC->IsLocalController())		{
			//TestPC->OnDeathMessage(KillerPlayerState, this, KillerDamageType);
		}
	}
}

void AFMPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFMPlayerState, TeamNumber);
	DOREPLIFETIME(AFMPlayerState, NumKills);
	DOREPLIFETIME(AFMPlayerState, NumDeaths);
}

FString AFMPlayerState::GetShortPlayerName() const{
	return PlayerName.Left(MAX_PLAYER_NAME_LENGTH);
}
