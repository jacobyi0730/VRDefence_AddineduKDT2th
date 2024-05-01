// Fill out your copyright notice in the Description page of Project Settings.

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
	class UStaticMeshComponent* TeleportCircle;

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
};
