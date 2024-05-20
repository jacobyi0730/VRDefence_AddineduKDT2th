// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameQuitWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRDEFENCE_API UGameQuitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 시작할 때 버튼에 함수를 연결하고싶다.
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, Category=VR, meta =(BindWidget))
	class UButton* Button_Quit;

	UFUNCTION()
	void OnMyClickGameQuit();
	
};
