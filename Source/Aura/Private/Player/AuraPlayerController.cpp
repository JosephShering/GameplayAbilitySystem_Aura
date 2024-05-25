// Copyright Pyroblast Games

#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Type::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);

	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D Axis = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();

	const FRotator Yaw = FRotator(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, Axis.Y);
		ControlledPawn->AddMovementInput(RightDirection, Axis.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if(!HitResult.bBlockingHit)
	{
		return;
	}

	LastActor = ThisActor;
	ThisActor = HitResult.GetActor();

	/**
	 * Line Trace from cursor
	 */

	if(ThisActor && !LastActor)
	{
		ThisActor->HighlightActor();	
	}

	if(!ThisActor && LastActor)
	{
		LastActor->UnHighlightActor();
	}

	if(ThisActor && LastActor && ThisActor != LastActor)
	{
		LastActor->UnHighlightActor();
		ThisActor->HighlightActor();
	}
}

