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

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMeshRight(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right'"));
	// 로드 성공했다면 적용하고싶다.
	if (TempMeshRight.Succeeded())
	{
		MeshRight->SetSkeletalMesh(TempMeshRight.Object);
		MeshRight->SetWorldLocationAndRotation(FVector(-2.98126f, 3.5f, 4.561753f), FRotator(25, 0, 90));
	}

	// 써클을 생성하고 충돌처리가 되지않게 처리하고싶다.
	TeleportCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportCircle"));
	TeleportCircle->SetupAttachment(RootComponent);
	TeleportCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	ResetTeleport();
}

void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 만약 버튼이 눌러졌다면
	if (true == bTeleporting)
	{
		FVector start = MeshRight->GetComponentLocation();
		FVector end = start + MeshRight->GetRightVector() * 100000;

		// 선 그리기
		DrawLine(start, end);

		// LineTrace를 해서 부딪힌 곳이 있다면
		FHitResult hitInfo;
		bool bHit = HitTest(start, end, hitInfo);
		if (bHit) {
			//	그곳에 써클을 보이게하고 배치하고싶다.
			TeleportCircle->SetWorldLocation(hitInfo.Location);
			TeleportCircle->SetVisibility(true);
		}
		// 그렇지 않다면
		else {
			//  써클을 보이지않게 하고싶다.
			TeleportCircle->SetVisibility(false);
		}
	}
}

void AVRPlayer::ONIATeleportStart(const FInputActionValue& value)
{
	// 누르면 써클이 보이고
	bTeleporting = true;
}

void AVRPlayer::ONIATeleportEnd(const FInputActionValue& value)
{
	// 떼면 안보이게 하고싶다.
	ResetTeleport();

}

void AVRPlayer::DrawLine(const FVector& start, const FVector& end)
{
	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, -1, 0, 1);
}

bool AVRPlayer::HitTest(FVector start, FVector end, FHitResult& OutHitInfo)
{
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	return GetWorld()->LineTraceSingleByChannel(OutHitInfo, start, end, ECC_Visibility, params);
}

void AVRPlayer::ResetTeleport()
{
	// 써클을 보이지않게 
	// 텔레포트중이 아님
	TeleportCircle->SetVisibility(false);
	bTeleporting = false;
}

// Called to bind functionality to input
void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (input) {
		input->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AVRPlayer::OnIAMove);
		input->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AVRPlayer::OnIATurn);

		// 텔레포트 입력을 등록하고싶다.
		// 눌렀을때 ONIATeleportStart
		input->BindAction(IA_Teleport, ETriggerEvent::Started, this, &AVRPlayer::ONIATeleportStart);
		// 뗏을때는 ONIATeleportEnd
		input->BindAction(IA_Teleport, ETriggerEvent::Completed, this, &AVRPlayer::ONIATeleportEnd);
	}
}

void AVRPlayer::OnIATurn(const FInputActionValue& value)
{
	float v = value.Get<float>();
	AddControllerYawInput(v);
}



void AVRPlayer::OnIAMove(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), v.Y);
	AddMovementInput(GetActorRightVector(), v.X);
}


