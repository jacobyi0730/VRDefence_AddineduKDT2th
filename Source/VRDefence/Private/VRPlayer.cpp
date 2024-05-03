// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayer.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h>
#include <../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h>
#include <Components/CapsuleComponent.h>
#include <../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <../../../../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraDataInterfaceArrayFunctionLibrary.h>

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
	if ( TempMeshLeft.Succeeded() )
	{
		MeshLeft->SetSkeletalMesh(TempMeshLeft.Object);
		MeshLeft->SetWorldLocationAndRotation(FVector(-2.98126f, -3.5f, 4.561753f), FRotator(-25, -180, 90));
	}

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMeshRight(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/MannequinsXR/Meshes/SKM_MannyXR_right.SKM_MannyXR_right'"));
	// 로드 성공했다면 적용하고싶다.
	if ( TempMeshRight.Succeeded() )
	{
		MeshRight->SetSkeletalMesh(TempMeshRight.Object);
		MeshRight->SetWorldLocationAndRotation(FVector(-2.98126f, 3.5f, 4.561753f), FRotator(25, 0, 90));
	}

	// 써클을 생성하고 충돌처리가 되지않게 처리하고싶다.
	TeleportCircleVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportCircleVFX"));
	TeleportCircleVFX->SetupAttachment(RootComponent);
	TeleportCircleVFX->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	TeleportTraceVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TeleportTraceVFX"));
	TeleportTraceVFX->SetupAttachment(RootComponent);
}

void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어컨트롤러를 가져오고싶다.
	auto* pc = Cast<APlayerController>(Controller);
	// UEnhancedInputLocalPlayerSubsystem를 가져와서
	if ( pc )
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if ( subsystem )
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
	if ( true == bTeleporting )
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TeleportTraceVFX, FName("User.PointArray"), Points);

		// 만약 곡선이면
		if ( bTeleportCurve )
		{
			TickCurve();
		}
		// 그렇지 않으면
		else {
			TickLine();
		}
	}
}

void AVRPlayer::TickLine()
{
	FVector start = MeshRight->GetComponentLocation();
	FVector end = start + MeshRight->GetRightVector() * 100000;

	CheckHitTeleport(start, end);

	// 선 그리기
	DrawLine(start, end);

	Points.Empty(2);
	Points.Add(start);
	Points.Add(end);
}

void AVRPlayer::TickCurve()
{
	// 자유 낙하 공식
	// 1/2 * gravity * t * t

	// 1. 궤적을 계산해서 정점정보를 목록으로 갖고 있고싶다.
	MakeCurvePoints();
	// 2. 점을 이으면서 충돌처리를 하고싶다. 모든 점들을 CheckHitTeleport를 이용해서 처리하고싶다.

	FHitResult hitInfo;
	int maxPoints = Points.Num();
	for ( int i = 0; i < Points.Num() - 1; i++ )
	{
		if ( CheckHitTeleport(Points[i], Points[i + 1]) )
		{
			// 어딘가 부딪혔다.
			maxPoints = i + 1;
			break;
		}
	}

	// 배열의 크기를 maxPoints로 설정하고싶다.
	Points.SetNum(maxPoints);

	// 3. 선을 그리고싶다.
	DrawCurve(maxPoints);
}

bool AVRPlayer::CheckHitTeleport(const FVector& start, FVector& end)
{
	// LineTrace를 해서 부딪힌 곳이 있다면
	FHitResult hitInfo;
	bool bHit = HitTest(start, end, hitInfo);
	if ( bHit && hitInfo.GetActor()->GetName().Contains(TEXT("Floor")) ) {
		//	그곳에 써클을 보이게하고 배치하고싶다.
		end = hitInfo.ImpactPoint;
		TeleportLocation = hitInfo.Location;
		TeleportCircleVFX->SetWorldLocation(hitInfo.Location);
		TeleportCircleVFX->SetVisibility(true);

	}
	// 그렇지 않다면
	else {
		//  써클을 보이지않게 하고싶다.
		TeleportCircleVFX->SetVisibility(false);
	}
	return bHit;
}

void AVRPlayer::MakeCurvePoints()
{
	// 1/2 * g * t * t
	// CurveStep
	Points.Empty(CurveStep);
	FVector gravity(0, 0, -981);
	float simDT = 1.f / 60.f;
	FVector point = MeshRight->GetComponentLocation();
	FVector velocity = MeshRight->GetRightVector() * 1000;
	Points.Add(point);
	for ( int i = 0; i < CurveStep; i++ )
	{
		point += velocity * simDT + 0.5f * gravity * simDT * simDT;
		velocity += gravity * simDT;
		Points.Add(point);
	}

}

void AVRPlayer::DrawCurve(int max)
{
	for ( int i = 0; i < max - 1; i++ )
	{
		DrawLine(Points[i], Points[i + 1]);
	}
}

void AVRPlayer::DoWarp()
{
	if ( false == bWarp )
		return;
	// 시간이 흐르다가 워프 이동시간이 끝나면 워프 종료

	CurrentTime = 0;

	FVector height = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FVector tarLoc = TeleportLocation + height;
	FVector originLoc = GetActorLocation();

	// 충돌체를 끄고싶다.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorld()->GetTimerManager().SetTimer(WarpTimerHandle, [&, originLoc, tarLoc]() {
		
		// 이동처리
		CurrentTime += GetWorld()->GetDeltaSeconds();
		// 현재위치, 목적지
		float alpha = CurrentTime / WarpTime;
		FVector curLoc =  FMath::Lerp(originLoc, tarLoc, alpha);
		SetActorLocation(curLoc);

		// 만약 도착했다면
		if ( alpha >= 1 )
		{
			// 타이머를 멈추고싶다.
			GetWorld()->GetTimerManager().ClearTimer(WarpTimerHandle);
			// 내위치를 tarLoc으로 하고싶다.
			SetActorLocation(tarLoc);
			// 충돌체를 켜고싶다.
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		}, 0.033333f, true);
}

void AVRPlayer::ONIATeleportStart(const FInputActionValue& value)
{
	// 누르면 써클이 보이고
	bTeleporting = true;
	// TraceVFX를 활성화 하고싶다.
	TeleportTraceVFX->SetVisibility(true);
}

void AVRPlayer::ONIATeleportEnd(const FInputActionValue& value)
{
	// 떼면 안보이게 하고싶다.
	// 만약 써클이 활성화 되어있다면 목적지로 이동하고싶다.
	if ( TeleportCircleVFX->GetVisibleFlag() )
	{
		if ( bWarp )
		{
			DoWarp();
		}
		else
		{
			DoTeleport();
		}
	}
	ResetTeleport();

}

void AVRPlayer::DrawLine(const FVector& start, const FVector& end)
{
	//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, -1, 0, 1);
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
	TeleportCircleVFX->SetVisibility(false);
	bTeleporting = false;
	TeleportTraceVFX->SetVisibility(false);
}

void AVRPlayer::DoTeleport()
{
	// 목적지로 이동
	FVector height = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	SetActorLocation(TeleportLocation + height);
}



// Called to bind functionality to input
void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if ( input ) {
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


