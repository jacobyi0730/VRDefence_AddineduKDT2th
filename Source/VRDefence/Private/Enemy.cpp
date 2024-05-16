// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "AIController.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <../../../../../../../Source/Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h>

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// AI값을 채우고싶다.
	AI = Cast<AAIController>(Controller);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

