// Fill out your copyright notice in the Description page of Project Settings.


#include "GunActor.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
AGunActor::AGunActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGunActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( bGrip )
	{
		// 선그리기를 하겠다.
		FHitResult hitInfo;
		FVector start = GetActorLocation() + GetActorForwardVector() * 50;
		FVector end = start + GetActorForwardVector() * 100000;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params);

		if ( bHit )
		{
			// 어딘가 닿았다.
			DrawDebugLine(GetWorld(), start, hitInfo.ImpactPoint, FColor::Red, false, 0);
		}
		else {
			// 허공
			DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 0);
		}
	}
}

void AGunActor::OnMyFire()
{
	// 라인트레이스를 발사하고싶다.
	FHitResult hitInfo;
	FVector start = GetActorLocation() + GetActorForwardVector() * 50;
	FVector end = start + GetActorForwardVector() * 100000;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params);
	// 만약 부딪힌것이 있다면
	if ( bHit )
	{
		// 만약 부딪힌것이 물리가 켜져있다면
		auto* hitComp = hitInfo.GetComponent();
		if ( hitComp && hitComp->IsSimulatingPhysics() )
		{
			// 힘을 가하고싶다.
			FVector direction = (end - start).GetSafeNormal();
			FVector force = direction * 1000 * hitComp->GetMass();
			hitComp->AddImpulseAtLocation(force, hitInfo.ImpactPoint);
		}

		// 그곳에 VFX를 표현하고싶다.
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireVFX, hitInfo.ImpactPoint);

	}
}

void AGunActor::SetGrip(bool _bGrip)
{
	bGrip = _bGrip;
}

