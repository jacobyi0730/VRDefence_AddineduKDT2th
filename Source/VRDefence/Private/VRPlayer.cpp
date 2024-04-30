// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayer.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h>
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h>

// Sets default values
AVRPlayer::AVRPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	// VR카메라 컴포넌트를 생성하고 루트에 붙이고싶다.
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(RootComponent);
	// 모션컨트롤러 왼손, 오른손 생성하고 루트에 붙이고싶다.
	MotionLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionLeft"));
	MotionLeft->SetTrackingMotionSource(TEXT("Left"));
	MotionLeft->SetupAttachment(RootComponent);

	MotionRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionRight"));
	MotionRight->SetTrackingMotionSource(TEXT("Right"));
	MotionRight->SetupAttachment(RootComponent);
	// 왼손, 오른손 스켈레탈메시컴포넌트를 만들어서 모션컨트롤러에 붙이고싶다.
	MeshLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshLeft"));
	MeshLeft->SetupAttachment(MotionLeft);
	MeshRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshRight"));
	MeshRight->SetupAttachment(MotionRight);
	// 왼손, 오른손 스켈레탈메시를 로드해서 적용하고싶다.

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMeshLeft(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_QuinnXR_left.SKM_QuinnXR_left'"));
	// 로드 성공했다면 적용하고싶다.
	if (TempMeshLeft.Succeeded())
	{
		MeshLeft->SetSkeletalMesh(TempMeshLeft.Object);
		MeshLeft->SetWorldLocationAndRotation(FVector(-2.98126f, -3.5f, 4.561753f), FRotator(-25, -180, 90));
	}

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMeshRight(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_QuinnXR_left.SKM_QuinnXR_left'"));
	// 로드 성공했다면 적용하고싶다.
	if (TempMeshRight.Succeeded())
	{
		MeshRight->SetSkeletalMesh(TempMeshRight.Object);
		MeshRight->SetWorldLocationAndRotation(FVector(-2.98126f, 3.5f, 4.561753f), FRotator(25, 0, 90));
	}

}

void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어컨트롤러를 가져오고싶다.
	auto* pc = Cast<APlayerController>(Controller);
	// UEnhancedInputLocalPlayerSubsystem를 가져와서
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			// AddMappingContext를 호출하고싶다.
			subsystem->AddMappingContext(IMC_VRPlayer, 0);
		}
	}
}

void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (input) {
		input->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AVRPlayer::OnIAMove);
	}
}

void AVRPlayer::OnIAMove(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), v.X);
	AddMovementInput(GetActorRightVector(), v.Y);
}

