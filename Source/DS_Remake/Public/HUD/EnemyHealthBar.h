// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "EnemyHealthBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class DS_REMAKE_API UEnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthBar;
	
};
