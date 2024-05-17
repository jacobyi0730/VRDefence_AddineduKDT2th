// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRDEFENCE_API UEnemyHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 계속 호출될 Tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// 체력정보를 받을 기능
	void UpdateInfo(float cur, float max);

	UPROPERTY(EditDefaultsOnly, Category = VR, meta=(BindWidget))
	class UProgressBar* BarBack;
	
	UPROPERTY(EditDefaultsOnly, Category = VR, meta=(BindWidget))
	class UProgressBar* BarFront;


};
