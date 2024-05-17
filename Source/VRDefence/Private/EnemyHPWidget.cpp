// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHPWidget.h"
#include "Components/ProgressBar.h"

void UEnemyHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// A = BarFront
	// B = BarBack
	// 
	// A의 퍼센티지값을 가져와서
	float target = BarFront->GetPercent();
	float current = BarBack->GetPercent();
	// B의 퍼센티지값이 A에 수렴하도록 처리하고싶다.
	current = FMath::Lerp(current, target, InDeltaTime * 5);
	BarBack->SetPercent(current);
}

void UEnemyHPWidget::UpdateInfo(float cur, float max)
{
	BarFront->SetPercent(cur / max);
}
