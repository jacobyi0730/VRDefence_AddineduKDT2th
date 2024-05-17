﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8 {
	Search	UMETA(DisplayerName = SEARCH),
	Move	UMETA(DisplayerName = MOVE),
	Attack	UMETA(DisplayerName = ATTACK),
};


UCLASS()
class VRDEFENCE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	EEnemyState State = EEnemyState::Search;

	UPROPERTY()
	class AAIController* AI;

	UPROPERTY()
	class AActor* Target;

	void TickSearch();
	void TickMove();
	void TickAttack();

	int32 MaxHP = 2;
	int32 HP = MaxHP;

	bool bDie;

	void OnMyTakeDamage(int32 damage);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UWidgetComponent* EnemyHPComp;	// 생성자에서

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UEnemyHPWidget* EnemyHP;		// beginPlay에서

};