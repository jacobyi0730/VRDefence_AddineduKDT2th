// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunActor.generated.h"

UCLASS()
class VRDEFENCE_API AGunActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 총쏘기 기능
	// 선그리기 활성/비활성 : 손에 잡혔다. 그렇지 않다.
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UParticleSystem* FireVFX;

	void OnMyFire();

	bool bGrip;
	void SetGrip(bool _bGrip);


};
