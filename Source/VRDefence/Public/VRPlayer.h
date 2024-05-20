﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "VRPlayer.generated.h"

UCLASS()
class VRDEFENCE_API AVRPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// VR카메라 컴포넌트를 생성하고 루트에 붙이고싶다.
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* VRCamera;
	// 모션컨트롤러 왼손, 오른손 생성하고 루트에 붙이고싶다.
	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* MotionLeft;

	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* MotionRight;
	// 왼손, 오른손 스켈레탈메시컴포넌트를 만들어서 모션컨트롤러에 붙이고싶다.
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* MeshLeft;

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* MeshRight;
	// 왼손, 오른손 스켈레탈메시를 로드해서 적용하고싶다.

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputMappingContext* IMC_VRPlayer;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Move;

	void OnIAMove(const FInputActionValue& value);


	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Turn;

	void OnIATurn(const FInputActionValue& value);


	// 텔레포트 처리를 위해서 써클을 표현하고싶다.
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UNiagaraComponent* TeleportCircleVFX;
	//class UStaticMeshComponent* TeleportCircle;

	// 텔레포트중인가? 여부를 기억하고싶다.
	bool bTeleporting;
	// 입력처리와 그에 해당하는 함수를 구현하고싶다.
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Teleport;

	void ONIATeleportStart(const FInputActionValue& value);
	void ONIATeleportEnd(const FInputActionValue& value);

	void DrawLine(const FVector& start, const FVector& end);

	bool HitTest(FVector start, FVector end, FHitResult& OutHitInfo);
	void ResetTeleport();

	// 목적지를 기억하고
	FVector TeleportLocation;
	// 목적지로 이동하는 기능
	void DoTeleport();


	// 곡선 텔레포트
	UPROPERTY(EditDefaultsOnly, Category = VR)
	bool bTeleportCurve = true;

	void TickLine();
	void TickCurve();

	bool CheckHitTeleport(const FVector& start, FVector& end);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	int32 CurveStep = 200;

	TArray<FVector> Points;
	void MakeCurvePoints();

	void DrawCurve(int max);

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UNiagaraComponent* TeleportTraceVFX;


	// 워프 : 빠르게 텔레포트 지점으로 이동하고싶다.
	// 워프 기능을 쓸것인가?, 워프 기능, 워프 이동 타이머
	UPROPERTY(EditDefaultsOnly, Category = VR)
	bool bWarp = true;

	FTimerHandle WarpTimerHandle;
	// 시간이 흐르다가 워프 이동시간이 끝나면 워프 종료
	float CurrentTime;
	float WarpTime = 0.2f;

	void DoWarp();

	// 총쏘기
	// 발사할 위치
	
	// 생성자에서 생성하고 루트에 붙이고싶다. 트래킹할 모션소스는 "RightAim"
	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* RightAim;

	// Fire입력을 등록하고싶다.
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Fire;

	// 키입력받을함수
	void OnIAFire(const FInputActionValue& value);

	// 부딪힌곳에 VFX
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UParticleSystem* FireVFX;

	// Crosshair
	UPROPERTY(EditDefaultsOnly, Category = VR)
	TSubclassOf<AActor> CrosshairFactory;

	UPROPERTY()
	AActor* Crosshair;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	float kAdjustCrosshairScale = 1;

	void DrawCrosshair();


	// 잡기, 놓기
	


	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_ViewReset;

	void OnIAViewReset(const FInputActionValue& value);

	// 트리거 버튼을 이용한 원거리 물체 잡고 놓기
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_RemoteGrip;

	void OnIARemoteGrip(const FInputActionValue& value);
	void OnIARemoteUnGrip(const FInputActionValue& value);

	FTimerHandle RemoteGripTimerHandle;


	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UHapticFeedbackEffect_Curve* HapticFire;


	// 오른손과 왼손으로 각 각 물체를 잡을 수 있다.
	// 반대손으로 물체를 뺏어올 수 있다.

	// 오른손
	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_Grip;

	void OnIAGrip(const FInputActionValue& value);
	void OnIAUnGrip(const FInputActionValue& value);

	// 잡은물체를 기억할 변수
	UPROPERTY()
	class UPrimitiveComponent* GripObject;
	// 던질 방향
	FVector ThrowDirection;
	// 이전위치값
	FVector PrevLocation;
	// 이전회전값
	FQuat PrevRotation;
	FQuat deltaAngle;



	// 왼손
	UPROPERTY()
	class UPrimitiveComponent* GripObjectLeft;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_GripLeft;

	void OnIAGripLeft(const FInputActionValue& value);
	void OnIAUnGripLeft(const FInputActionValue& value);

	// 던질 방향
	FVector ThrowDirectionLeft;
	// 이전위치값
	FVector PrevLocationLeft;
	// 이전회전값
	FQuat PrevRotationLeft;
	FQuat deltaAngleLeft;


	UPROPERTY()
	class AGunActor* GunActor;

	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UInputAction* IA_LeftFire;

	void OnIALeftFire(const FInputActionValue& value);



	// 두손이 함께 사용하는 코드
	struct FOverlapResult DoGrip(class USkeletalMeshComponent* hand);
	void DoUnGrip(class USkeletalMeshComponent* hand, class UPrimitiveComponent* obj, const FQuat _deltaAngle);

	// 던지는 속성을 계산하는 기능
	void TickGripCalc();
	// 던지는 행위의 기능
	void DoThrowObject(class UPrimitiveComponent* obj, const FQuat& _deltaAngle);

	// 잡을 반경
	UPROPERTY(EditDefaultsOnly, Category = VR)
	float GripRadius = 100;

	// <던지기>
	// 던지는힘
	UPROPERTY(EditDefaultsOnly, Category = VR)
	float ThrowPower = 50000;
	// 회전할힘
	UPROPERTY(EditDefaultsOnly, Category = VR)
	float TorquePower = 500;


	UPROPERTY(EditDefaultsOnly, Category = VR)
	class UWidgetInteractionComponent* Interaction;

	void OnIAReleasePointer(const FInputActionValue& value);

};
