#include "MyPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode_Game.h"

TMap<int, AMyPawn*> AMyPawn::Instances = TMap<int, AMyPawn*>();

AMyPawn::AMyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;

	bCollideWhenPlacing = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	RootComponent = CollisionComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetVisibility(false);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(RootComponent);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 30.0f; // The camera follows at this distance behind the character
	CameraBoom->SetRelativeRotation(FRotator(-30, 0, 0));
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bInheritPitch = false;
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	SetupBodyInstance();

	StartLocation = GetActorLocation();
	StartRotation = GetActorRotation();
}

void AMyPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetActorLocation().SizeSquared() > 90000)
	{
		if (auto myGameMode = Cast<AGameMode_Game>(UGameplayStatics::GetGameMode(this)))
		{
			if (Instances[0] == this)
			{
				myGameMode->Player1Score--;
			}
			else
			{
				myGameMode->Player2Score--;
			}

			SetActorLocation(StartLocation);
			StopMovement();
			UGameplayStatics::PlaySound2D(this, FallSfx);

			auto controller = Cast<APlayerController>(GetController());
			float calcValue = (StartRotation.Yaw - GetActorRotation().Yaw) / controller->InputYawScale;
			AddControllerYawInput(calcValue);
			ServerSetYaw(StartRotation.Yaw);
		}
	}
}

void AMyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPawn, Yaw);
	DOREPLIFETIME(AMyPawn, State);
}










void AMyPawn::SetupBodyInstance()
{
	CollisionComponent->SetMassOverrideInKg("", Mass, true);
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true);

	CollisionComponent->BodyInstance.bUseCCD = true;
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto controller = Cast<APlayerController>(GetController());
	auto controllerId = UGameplayStatics::GetPlayerControllerID(controller);
	Instances.Add(controllerId, this);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyPawn::CommonMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPawn::CommonMoveRight);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AMyPawn::CommonStartFire);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AMyPawn::CommonStopFire);
}

void AMyPawn::CommonMoveForward(float Value)
{
	if (auto myGameMode = Cast<AGameMode_Game>(UGameplayStatics::GetGameMode(this)))
	{
		if (Instances.Contains(myGameMode->ActiveControllerId))
		{
			auto pawn = Instances[myGameMode->ActiveControllerId];
			pawn->MoveForward(Value);
		}
	}
}

void AMyPawn::MoveForward(float Value)
{
	if (Value != 0.0f && State == MyPawnState::ACTIVE)
	{
		auto tolerance = 1e-6f;
		if (CollisionComponent->GetPhysicsAngularVelocityInDegrees().SizeSquared() > tolerance)
		{
			StopMovement();
		}

		ControlInputVector += GetControlRotation().Vector() * Value;
	}
}

void AMyPawn::CommonMoveRight(float Value)
{
	if (auto myGameMode = Cast<AGameMode_Game>(UGameplayStatics::GetGameMode(this)))
	{
		if (Instances.Contains(myGameMode->ActiveControllerId))
		{
			auto pawn = Instances[myGameMode->ActiveControllerId];
			pawn->MoveRight(Value);
		}
	}
}

void AMyPawn::MoveRight(float Value)
{
	if (Value != 0.0f && State == MyPawnState::ACTIVE)
	{
		auto tolerance = 1e-6f;
		if (CollisionComponent->GetPhysicsAngularVelocityInDegrees().SizeSquared() > tolerance)
		{
			StopMovement();
		}

		AddControllerYawInput(Value);
		ServerSetYaw(GetControlRotation().Yaw);
	}
}

void AMyPawn::CommonStartFire_Implementation()
{
	if (auto myGameMode = Cast<AGameMode_Game>(UGameplayStatics::GetGameMode(this)))
	{
		if (Instances.Contains(myGameMode->ActiveControllerId))
		{
			auto pawn = Instances[myGameMode->ActiveControllerId];
			pawn->StartFire();
		}
	}
}

bool AMyPawn::CommonStartFire_Validate()
{
	return true;
}

void AMyPawn::StartFire()
{
	if (State == MyPawnState::ACTIVE)
	{
		State = MyPawnState::LAUNCHED;
		CollisionComponent->AddForce(ForceAmount * GetActorForwardVector());
	}
}

void AMyPawn::CommonStopFire_Implementation()
{
	if (auto myGameMode = Cast<AGameMode_Game>(UGameplayStatics::GetGameMode(this)))
	{
		if (Instances.Contains(myGameMode->ActiveControllerId))
		{
			auto pawn = Instances[myGameMode->ActiveControllerId];
			pawn->StopFire();
		}
	}
}

bool AMyPawn::CommonStopFire_Validate()
{
	return true;
}

void AMyPawn::StopFire()
{
	if (State == MyPawnState::LAUNCHED)
	{

		auto myGameMode = Cast<AGameMode_Game>(UGameplayStatics::GetGameMode(this));

		State = MyPawnState::ACTIVE;
		StopMovement();
		UE_LOG(LogTemp, Warning, TEXT("%d"), myGameMode->ActiveControllerId);

		auto next = (myGameMode->ActiveControllerId + 1) % 2;

		Instances[next]->StopMovement();
		UE_LOG(LogTemp, Warning, TEXT("%d"), next);

		myGameMode->ActiveControllerId = next;
	}
}

void AMyPawn::StopMovement()
{
	CollisionComponent->DestroyPhysicsState();
	SetYaw();
	CollisionComponent->BodyInstance = FBodyInstance();
	SetupBodyInstance();
	CollisionComponent->CreatePhysicsState();
}

void AMyPawn::ServerSetYaw_Implementation(float value)
{
	Yaw = value;
	SetYaw();
}

bool AMyPawn::ServerSetYaw_Validate(float value)
{
	return true;
}

void AMyPawn::OnRep_SetYaw()
{
	SetYaw();
}

void AMyPawn::SetYaw()
{
	auto rotation = FRotator::ZeroRotator;
	rotation.Yaw = Yaw;
	SetActorRotation(rotation);
}