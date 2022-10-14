// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),
	ECS_MAX UMETA(DisplayName = "Default Max")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()
	// Scene component to use for its location for interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	// Number of items interping to.at this scene component location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	//Take Combat Damage
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Called for forwards/Backwards input/

	void MoveForward(float Value);

	// Called for side to side input//
	void MoveRight(float Value);

	/*
	Called via input to turn at a given rate
	@param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/*
	Called via input to look up/down at a given rate
	@param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	// Rotate controller based on mouse X movement
	// @param Value The input value from mouse movement
	void Turn(float Value);

	// Rotate controller based on mouse Y movement
	// @param Value The input value from mouse movement

	void LookUp(float Value);

	//Call when the fire button is pressed/
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	// Set bAiming to true or false
	void AimingButtonPressed(); 
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

	// Set BaseTurnRate and BaseLookUpRate based on aiming//
	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();


	UFUNCTION()
	void FinishedCrosshairBulletFire();

	void FireButtonPressed();

	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	// Line trace for items under the crosshairs
	bool TraceUnderCrosshairs(FHitResult& OutHiResult, FVector& OutHitLocation);

	//Trace for items if OverlappedItemCOunt > 0
	void TraceForItems();

	// Spawns a default weapon and equips it
	class AWeapon* SpawnDefaultWeapon();

	// Takes a weapon and attaches it to the mesh
	void EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping = false);

	// Detach weapon and let it fall to the ground
	void DropWeapon();

	void SelectButtonPressed();
	void SelectedButtonReleased();

	// Drops currently equipeped Weapon and Equips TraceHitItem 
	void SwapWeapon(AWeapon* WeaponToSwap);

	// Initialize the Ammo map with ammo values
	void InitializeAmmoMap();

	// Check if the weapon has ammo
	bool WeaponHasAmmo();

	// FireWeapon functions
	void PlayFireSound();
	void SendBullet();
	void PlayGunfireMontage();

	// Bound to the R key
	void ReloadButtonPressed();

	void ReloadWeapon();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
		void FinishEquipping();

	// Check to see if we have ammo of the EquippedWeapon's ammo type
	bool CarryingAmmo();

	// Called from animation BP with grab clip notify
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	// Called from animation BP with release clip notify
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	// Transform of the clip when first grabbed the clip during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	// Scene component to attach to the character hand's during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	void CrouchButtonPressed();

	virtual void Jump() override;

	// Interps capsule half height when crouching / standing
	void InterpCapsuleHalfHeight(float DeltaTime);

	void Aim();

	void StopAiming();

	void PickupAmmo(class AAmmo* Ammo);

	void InitializedInterpLocations();

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	int32 GetEmptyInventorySlot();

	void HighlightedInventorySlot();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	UFUNCTION(BlueprintCallable)
		void EndStun();

	void Die();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	void AnimDeath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float BaseTurnRate;

	//Randomized gunshot sound cue
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float BaseLookUpRate;

	//Turn rate while not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float HipTurnRate;

	// Look up rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float HipLookUpRate;

	//Turn rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float AimingTurnRate;

	//Look up rate while not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float AimingLookUpRate;

	// Scale factor for mouse look sensitivity. Turn rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;
	// Scale factor for mouse look sensitivity. Look up rate when not aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate;

	// Scale factor for mouse look sensitivity. Turn rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;
	// Scale factor for mouse look sensitivity. Look up rate when aiming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimimngLookUpRate;

	// Montage for firing the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// Particles spawned upon bullet impact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	//Smoke trail for bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// true when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;


	// Default camera field of view value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFOV;

		// Field of view value for when zoomed in
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV;

	// Current field of view this frame
	float CameraCurrentFOV;

		// Interpt speed for zooming when aiming 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	// Determines the spread of the crosshairs
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	// Velocity component for crosshairs spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	// In air component for crosshairs spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	// Aim component for crosshairs spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	// Shooting component for crosshairs spread
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	// Left mouse button pressed
	bool bFireButtonPressed;

	// True when we can fire. False when waiting for the timer
	bool bShouldFire;

	// Set a timer between gunshots
	FTimerHandle AutoFireTimer;

	// True if we should trace every frame for items
	bool bShouldTraceForItems;

	// Number of overlapped AItems
	int8 OverlappedItemCount;

	// the AItem we hit last frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;


	// Currently equipped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	 AWeapon* EquippedWeapon;

	// Set this in Blueprints for the default Weapon class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon>DefaultWeaponClass;

	// The item currently hit by our trace ub TraceForItems (Could be null)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;
	
	// Distance outward from the camera for the interp destination
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
		float CameraInterpDistance;

	// Distance upward from the camera for the interp destination
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	// Map to keep track of ammo of the different ammo types
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	// Starting amount of 9mm ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	// Starting amount of AR ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	// Combat state can only fire or reload if unoccupied
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	// Montage for reload animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* EquipMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	// Regular movement speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;

	// Crouch movement speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float CrouchMovementSpeed;

	// Current half height of the capsule
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CurrentCapsuleHalfHeight;

	// Half height og the capsule when not crouching 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	//Half height of the capsule when crouching 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	// Ground friction while not crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;

	// Ground friction while crouching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

	// Used for knowning when the aiming button is pressed
	bool bAimingButtonPressed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneComponent* InterpComp6;

	//Array of interp location structs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess ="true"))
	TArray<FInterpLocation> InterpLocations;

	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	// Time to wait before we can play another pickup sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;

	// Time to wait before we can play another equip sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;



	// Delegate for senfing slot information to InventoryBar when equipping
	UPROPERTY(BlueprintAssignable, Category = Delegate, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	// Delegate for senfing slot information for playing the icon animation
	UPROPERTY(BlueprintAssignable, Category = Delegate, meta = (AllowPrivateAccess = "true"))
		FHighlightIconDelegate HighlightIconDelegate;

	// The index for the currently highlighted slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot;

	//Character health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;

	//Character Max health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float MaxHealth;

	// Sound made when character gets hit by a melee attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* MeleeImpactSound;

	// Blood splatter particles for melee hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticles;

	// Hit react anim montage for when the character is stunned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactMontage;

	// Chance of being stun when hit by an enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunChance;

	//Montage for character death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DeathMontage;

	// true when character dies
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bDeath;

public:
	// Return CameraBoom Subobject;
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetAiming() const { return bAiming; }
	 
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	// Adds/Subtracts to/from OverlappedItemCount and updates bShouldaTraceForItems
	void IncrementOverlappedItemCount(int8 Amount);

	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	
	FInterpLocation GetInterpLocation(int32 Index);

	// Returns the index in InterpLocarions array with the lowest count
	int32 GetInterpLocationIndex();

	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }


	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

	// An array of AItems for the Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;
	const int32 INVENTORY_CAPACITY{ 6 };

	void UnHighlightedInventorySlot();

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	FORCEINLINE USoundCue* GetMeleeImpactSound() const { return MeleeImpactSound; }

	FORCEINLINE UParticleSystem* GetBloodParticles() const { return BloodParticles; }

	void Stun();

	FORCEINLINE float GetStunChance() const { return StunChance; }

	FORCEINLINE bool GetDeath() const { return bDeath; }

	FORCEINLINE float GetHealth() const { return Health; }
};
