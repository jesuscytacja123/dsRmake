// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"

#include "Components/ProgressBar.h"
#include "HUD/EnemyHealthBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if(HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UEnemyHealthBar>(GetUserWidgetObject());
	}
	if(HealthBarWidget)
	{
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}
}
