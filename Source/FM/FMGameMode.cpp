// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMGameMode.h"
#include "FMCharacter.h"

AFMGameMode::AFMGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/Player/BP_FMCharacter.BP_FMCharacter'"));
	if (PlayerPawnObject.Object != NULL){
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

}


void AFMGameMode::BeginPlay(){
	Super::BeginPlay();

	if (GEngine){
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("GameMode: BeginPlay Called"));
	}

}