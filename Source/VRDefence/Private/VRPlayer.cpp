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

	// VRī�޶� ������Ʈ�� �����ϰ� ��Ʈ�� ���̰�ʹ�.
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(RootComponent);
	// �����Ʈ�ѷ� �޼�, ������ �����ϰ� ��Ʈ�� ���̰�ʹ�.
	MotionLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionLeft"));
	MotionLeft->SetTrackingMotionSource(TEXT("Left"));
	MotionLeft->SetupAttachment(RootComponent);

	MotionRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionRight"));
	MotionRight->SetTrackingMotionSource(TEXT("Right"));
	MotionRight->SetupAttachment(RootComponent);
	// �޼�, ������ ���̷�Ż�޽�������Ʈ�� ���� �����Ʈ�ѷ��� ���̰�ʹ�.
	MeshLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshLeft"));
	MeshLeft->SetupAttachment(MotionLeft);
	MeshRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshRight"));
	MeshRight->SetupAttachment(MotionRight);
	// �޼�, ������ ���̷�Ż�޽ø� �ε��ؼ� �����ϰ�ʹ�.

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMeshLeft(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_QuinnXR_left.SKM_QuinnXR_left'"));
	// �ε� �����ߴٸ� �����ϰ�ʹ�.
	if (TempMeshLeft.Succeeded())
	{
		MeshLeft->SetSkeletalMesh(TempMeshLeft.Object);
		MeshLeft->SetWorldLocationAndRotation(FVector(-2.98126f, -3.5f, 4.561753f), FRotator(-25, -180, 90));
	}

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMeshRight(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right'"));
	// �ε� �����ߴٸ� �����ϰ�ʹ�.
	if (TempMeshRight.Succeeded())
	{
		MeshRight->SetSkeletalMesh(TempMeshRight.Object);
		MeshRight->SetWorldLocationAndRotation(FVector(-2.98126f, 3.5f, 4.561753f), FRotator(25, 0, 90));
	}

	// ��Ŭ�� �����ϰ� �浹ó���� �����ʰ� ó���ϰ�ʹ�.
	TeleportCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportCircle"));
	TeleportCircle->SetupAttachment(RootComponent);
	TeleportCircle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();

	// �÷��̾���Ʈ�ѷ��� ��������ʹ�.
	auto* pc = Cast<APlayerController>(Controller);
	// UEnhancedInputLocalPlayerSubsystem�� �����ͼ�
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			// AddMappingContext�� ȣ���ϰ�ʹ�.
			subsystem->AddMappingContext(IMC_VRPlayer, 0);
		}
	}

	ResetTeleport();
}

void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ���� ��ư�� �������ٸ�
	if (true == bTeleporting)
	{
		FVector start = MeshRight->GetComponentLocation();
		FVector end = start + MeshRight->GetRightVector() * 100000;

		// �� �׸���
		DrawLine(start, end);

		// LineTrace�� �ؼ� �ε��� ���� �ִٸ�
		FHitResult hitInfo;
		bool bHit = HitTest(start, end, hitInfo);
		if (bHit) {
			//	�װ��� ��Ŭ�� ���̰��ϰ� ��ġ�ϰ�ʹ�.
			TeleportCircle->SetWorldLocation(hitInfo.Location);
			TeleportCircle->SetVisibility(true);
		}
		// �׷��� �ʴٸ�
		else {
			//  ��Ŭ�� �������ʰ� �ϰ�ʹ�.
			TeleportCircle->SetVisibility(false);
		}
	}
}

void AVRPlayer::ONIATeleportStart(const FInputActionValue& value)
{
	// ������ ��Ŭ�� ���̰�
	bTeleporting = true;
}

void AVRPlayer::ONIATeleportEnd(const FInputActionValue& value)
{
	// ���� �Ⱥ��̰� �ϰ�ʹ�.
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
	// ��Ŭ�� �������ʰ� 
	// �ڷ���Ʈ���� �ƴ�
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

		// �ڷ���Ʈ �Է��� ����ϰ�ʹ�.
		// �������� ONIATeleportStart
		input->BindAction(IA_Teleport, ETriggerEvent::Started, this, &AVRPlayer::ONIATeleportStart);
		// �������� ONIATeleportEnd
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


