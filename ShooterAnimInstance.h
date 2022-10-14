// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),
	EOS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UShooterAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:

	// Handle turning in place variables
	void TurnInPlace();

	// Hanfdle calculations for leaning while running
	void Lean(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	// Offset yaw used for strafing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	// Offset yaw the frame before we stop moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// Yaw of the character this frame
	float TIPCharacterYaw;

	// Yaw of the character the previous frame; Only updated when standing still and not in air
	float TIPCharacterYawLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	// Rotation curve value this frame
	float RotationCurve;

	float RotationCurveLastFrame;

	// The pitch of the aim rotation used for Aim Offset
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	// True when reloadinf, used to prevent aim offset while reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	// Offset state; used to determine which aim offset to use
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = " Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	// Character yaw this frame
	FRotator CharacterRotation;

	// Yaw of the character the previous frame
	FRotator CharacterRotationLastFrame;

	// Yaw delta used for leaning in the running blendspace
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"))
	float YawDelta;

	// True when crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

		// True when equipping
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
		bool bEquipping;

	// Change the recoil weight based on turning in place and aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;

	// True when turningin place
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bTurningInPlace;

	// Weapon type for the currently equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EWeaponType EquippedWeaponType;

	//true when not reloading or equipping
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bShouldUseFABRIK;
};
