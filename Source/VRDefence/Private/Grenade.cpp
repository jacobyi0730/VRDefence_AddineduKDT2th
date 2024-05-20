// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"
#include "Enemy.h"
#include "Components/BoxComponent.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrenade::Play()
{
	// 일정시간 후에 터지고싶다.
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, [&](){
		
		// 현재위치에 폭발 VFX를 표현하고싶다.
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFXFactory, GetActorTransform());

		// 터질때 반경 10M안의 적에게 힘을 가해서 밀어내고싶다.
		
		TArray<FOverlapResult> hits;
		FVector origin = GetActorLocation();
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		
		bool bHit = GetWorld()->OverlapMultiByObjectType(hits, origin, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(1000), params);

		for ( int i = 0; i < hits.Num(); i++ )
		{
			auto* enemy =  Cast<AEnemy>(hits[i].GetActor());
			if ( enemy )
			{
				// 적은 일정시간 후에 데미지를 2점 받게 하고싶다.
				enemy->OnMyTakeDamageWithFlying(origin, 1000, hits[i].GetComponent());
			}
		}

		this->Destroy();

	}, 3, false);
}

