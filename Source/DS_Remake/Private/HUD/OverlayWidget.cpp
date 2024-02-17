// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverlayWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UOverlayWidget::SetHealthBarPercent(float Percent)
{
	HealthBar->SetPercent(Percent);
}

void UOverlayWidget::SetStaminaBarPercent(float Percent)
{
	StaminaBar->SetPercent(Percent);
}

void UOverlayWidget::SetBossHealthBarPercent(float Percent)
{
	HealthBarBoss->SetPercent(Percent);
}

void UOverlayWidget::HideBossOverlays()
{
	HealthBarBoss->SetRenderOpacity(0.f);
	BossText->SetRenderOpacity(0.f);
}

void UOverlayWidget::ShowBossOverlays()
{
	HealthBarBoss->SetRenderOpacity(1.f);
	BossText->SetRenderOpacity(1.f);
}
