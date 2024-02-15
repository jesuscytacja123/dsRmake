// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverlayWidget.h"

#include "Components/ProgressBar.h"


void UOverlayWidget::SetHealthBarPercent(float Percent)
{
	HealthBar->SetPercent(Percent);
}

void UOverlayWidget::SetStaminaBarPercent(float Percent)
{
	StaminaBar->SetPercent(Percent);
}
