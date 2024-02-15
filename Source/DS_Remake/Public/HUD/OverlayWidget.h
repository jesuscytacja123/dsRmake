// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverlayWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class DS_REMAKE_API UOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetHealthBarPercent(float Percent);

	UFUNCTION(BlueprintCallable)
	void SetStaminaBarPercent(float Percent);
	
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthBar;
	
	UPROPERTY(meta=(BindWidget))
	UProgressBar* StaminaBar;
};

