// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "AIController.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <../../../../../../../Source/Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h>
#include "EnemyHPWidget.h"
#include "Components/WidgetComponent.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemyHPComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHPComp"));

	EnemyHPComp->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// AI값을 채우고싶다.
	AI = Cast<AAIController>(Controller);

	EnemyHP = Cast<UEnemyHPWidget>(EnemyHPComp->GetWidget());
	EnemyHP->UpdateInfo(1, 1);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// EnemyHPComp를 빌보드처리하고싶다.
	auto* pc = GetWorld()->GetFirstPlayerController();
	if ( pc )
	{
		FVector dir = pc->PlayerCameraManager->GetCameraLocation() - EnemyHPComp->GetComponentLocation();
		EnemyHPComp->SetWorldRotation(UKismetMathLibrary::MakeRotFromX(dir.GetSafeNormal()));
	}

	switch ( State )
	{
	case EEnemyState::Search:	TickSearch();		break;
	case EEnemyState::Move:		TickMove();			break;
	case EEnemyState::Attack:	TickAttack();		break;
	}

	FString strState = UEnum::GetValueAsString(State);
	DrawDebugString(GetWorld(), GetActorLocation(), strState, nullptr, FColor::White, 0);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::TickSearch()
{
	TArray<AActor*> towers;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), TEXT("Tower"), towers);

	if (towers.Num() > 0)
	{
		Target = towers[0];
		State = EEnemyState::Move;
	}
}

void AEnemy::TickMove()
{
	auto result = AI->MoveToLocation(Target->GetActorLocation(), 200);

	if ( result == EPathFollowingRequestResult::AlreadyAtGoal )
	{
		State = EEnemyState::Attack;
	}

}

void AEnemy::TickAttack()
{

}

void AEnemy::OnMyTakeDamage(int32 damage)
{
	if ( bDie )
		return;

	HP -= damage;
	// 만약 HP가 0이하라면
	if ( HP <= 0 )
	{
		// HP 를 0으로 만들고
		HP = 0;
		bDie = true;
		// 1초 후에 파괴되고싶다.
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [&]() {
			Destroy();
		}, 1, false);
	}

	EnemyHP->UpdateInfo(HP, MaxHP);
}

