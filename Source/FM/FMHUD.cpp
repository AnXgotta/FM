// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FM.h"
#include "FMCharacter.h"
#include "FMHUD.h"


AFMHUD::AFMHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Offset = 20.0f;
	ScaleUI = 1.0f;
	HUDLight = FColor(175, 202, 213, 255);
	HUDDark = FColor(110, 124, 131, 255);

	static ConstructorHelpers::FObjectFinder<UFont> NormalFontObject(TEXT("/Game/_Materials/Roboto18"));
	NormalFont = NormalFontObject.Object;

}


void AFMHUD::DrawHUD(){
	Super::DrawHUD();

	DrawDebugHealthStamina();

	


}



void AFMHUD::SetMatchState(EMatchState::Type newState){
	matchState = newState;
}

EMatchState::Type AFMHUD::GetMatchState() const {
	return matchState;
}

FString AFMHUD::GetTimeString(float TimeSeconds){
	// only minutes and seconds are relevant
	const int32 TotalSeconds = FMath::Max(0, FMath::Trunc(TimeSeconds) % 3600);
	const int32 NumMinutes = TotalSeconds / 60;
	const int32 NumSeconds = TotalSeconds % 60;

	const FString TimeDesc = FString::Printf(TEXT("%02d:%02d"), NumMinutes, NumSeconds);
	return TimeDesc;
}

void AFMHUD::DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor){
	float SizeX, SizeY;
	Canvas->StrLen(NormalFont, Text, SizeX, SizeY);

	const float UsePosX = bAlignLeft ? PosX : PosX - SizeX;
	const float UsePosY = bAlignTop ? PosY : PosY - SizeY;

	const float BoxPadding = 5.0f;

	FColor DrawColor(HUDDark.R, HUDDark.G, HUDDark.B, HUDDark.A * 0.2f);
	const float X = UsePosX - BoxPadding * ScaleUI;
	const float Y = UsePosY - BoxPadding * ScaleUI;
	FCanvasTileItem TileItem(FVector2D(X, Y), FVector2D((SizeX + BoxPadding * 2.0f) * ScaleUI, (SizeY + BoxPadding * 2.0f) * ScaleUI), DrawColor);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	FCanvasTextItem TextItem(FVector2D(UsePosX, UsePosY), FText::FromString(Text), NormalFont, TextColor);
	TextItem.EnableShadow(FLinearColor::Black);
	//TextItem.FontRenderInfo = ShadowedFont;
	TextItem.Scale = FVector2D(ScaleUI, ScaleUI);
	Canvas->DrawItem(TextItem);
}


void AFMHUD::DrawDebugHealthStamina(){
	AFMCharacter* MyPawn = Cast<AFMCharacter>(GetOwningPawn());
	const int healthAmount = MyPawn->GetCurrentHealth();
	const int staminaAmount = MyPawn->GetCurrentStamina();

	const FString displayText = FString::Printf(TEXT("HEALTH:  %d    STAMINA:  %d"), healthAmount, staminaAmount);

	DrawDebugInfoString(displayText, 600.0f, 50.0f, false, false, HUDLight);

}
