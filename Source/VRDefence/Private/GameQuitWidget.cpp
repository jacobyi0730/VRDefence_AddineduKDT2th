// Fill out your copyright notice in the Description page of Project Settings.


#include "GameQuitWidget.h"
#include <../../../../../../../Source/Runtime/UMG/Public/Components/Button.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>

void UGameQuitWidget::NativeConstruct()
{
	Button_Quit->OnClicked.AddDynamic(this, &UGameQuitWidget::OnMyClickGameQuit);
}

void UGameQuitWidget::OnMyClickGameQuit()
{
	UE_LOG(LogTemp, Warning, TEXT("OnMyClickGameQuit"));
	auto pc = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(GetWorld(), pc, EQuitPreference::Quit, false);
}
