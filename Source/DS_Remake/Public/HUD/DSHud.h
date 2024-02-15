// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DSHud.generated.h"

class UOverlayWidget;
/**
 * 
 */
UCLASS()
class DS_REMAKE_API ADSHud : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOverlayWidget> OverlayClass;
};
