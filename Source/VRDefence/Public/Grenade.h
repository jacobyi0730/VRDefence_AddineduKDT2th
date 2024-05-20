﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

UCLASS()
class VRDEFENCE_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UParticleSystem* VFXFactory;

	void Play();


};
