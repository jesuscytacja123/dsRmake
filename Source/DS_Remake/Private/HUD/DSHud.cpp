// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DSHud.h"
#include "HUD/OverlayWidget.h"
#include "Blueprint/UserWidget.h"

void ADSHud::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	UOverlayWidget* Overlay = CreateWidget<UOverlayWidget>(PlayerController, OverlayClass);
	Overlay->AddToViewport();
}
