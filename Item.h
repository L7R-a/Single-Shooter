// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity  : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),
	EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DiplayName = "Weapon"),
	EIT_MAX UMETA (DisplayName = "Default Max")
};

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfStars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencil;
};

UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when overlapping AreaSphere
	UFUNCTION()
		void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	// Called when End Overlapping AreaSphere
	UFUNCTION()
		void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);

	// Sets the ActiveStars array of bools based on rarity
	void SetActiveStars();

	// Set properties of the item's components based on State
	virtual void SetItemProperties(EItemState State);

	//Called when ItemIterpTimer is finished
	void FinishInterping();

	// Handles item interpolation when in the EquipInterping state
	void ItemInterp(float DeltaTime);

	// Get interp location based on the item type
	FVector GetInterpLocation();

	void PlayPickupSound(bool bForcePlaySound = false);

	virtual void OnConstruction(const FTransform& Transform) override;

	void EnableGlowMaterial();

	void UpdatePulse();

	void ResetPulseTimer();

	void StartPulseTimer();


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called in AShooterCharacter::GetPickupItem
	void PlayEquipSound(bool bForcePlaySound = false);

private:
	// Skeletal mesh for the item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ItemMesh;

	// Line trace cikkudes with box to show HUD widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionBox;

	// Popup widget for when the player looks at the item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickupWidget;

	// Enables item tracing when overlapped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AreaSphere;

	// The name that appears in the Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FString ItemName;

	// Item count (Ammo, etc)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 ItemCount;

	// Item rarity - determines number of stars in pickup widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rarity, meta = (AllowPrivateAccess = "true"))
		EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		TArray<bool> ActiveStars;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		EItemState ItemState;

	// The curve asset to use for the item's Z location when interping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class UCurveFloat* ItemZCurve;

	// Starting location when interping begins
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FVector ItemInterpStartLocation;

	// Target interp location in front of the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		FVector CameraTargetLocation;

	// True when interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		bool bInterping;

	// Plays when we start interping
	FTimerHandle ItemInterpTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		float ZCurveTime;

	// Pointer to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class AShooterCharacter* Character;

	// X and Y for the Item while interpinf in the EquipInterping state
	float ItemInterpX;
	float ItemInterpY;

	// Initial Yaw offset between the camera and the interping item
	float InterpInitialYawOffset;

	// Curve used to scale the item when interping
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UCurveFloat* ItemScaleCurve;

	// Sound play when the item is pick up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class USoundCue* PickupSound;

	// Sound played when the item is equipped
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		USoundCue* EquipSound;

	// Enum for the type of item this item is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		EItemType ItemType;

	// Index of the interp location this item is interping to
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 InterpLocIndex;

	// Index for the material we would like to change at runtime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		int32 MaterialIndex;

	// Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* DynamicMaterialInstance;

	// Mar=terial Instance used with the Dynamic Material Instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* MaterialInstance;

	bool bCanChangeCustomDepth;

	// Curve to drive the dynamic material parameters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		class UCurveVector* PulseCurve;

	FTimerHandle PulseTimer;

	// Time for the PulseTimer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		float PulseCurveTime;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		float GlowAmount;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		float FresnelExponent;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		float FresnelReflectFraction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
		UCurveVector* InterpPulseCurve;

	// Icon for this item in the inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		UTexture2D* IconItem;

	// Ammo Icon for this item in the Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		int32 AmmoCount;

	// Slot for the inventory array
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		int32 SlotIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, meta = (AllowPrivateAccess = "true"))
		bool bCharacterInventoryIsFull;

	// Item Rarity data table
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		class UDataTable* ItemRarityDataTable;

	// Color in the glow material
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
		FLinearColor GlowColor;

	// Light Color in the pickup widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
		FLinearColor LightColor;

	// Dark Color in the pickup widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
		FLinearColor DarkColor;

	//Number of stars in the pickup widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
		int32 NumberOfStars;

	//Background icon for the inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
		UTexture2D* IconBackground;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	// Called from the AShooterCHaracter class
	void StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound = false);

	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE void SetPickupSound(USoundCue* Sound) { PickupSound = Sound; }
	FORCEINLINE void SetEquipSound(USoundCue* Sound) { EquipSound = Sound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }
	FORCEINLINE void SetItemName(FString Name) { ItemName = Name; }
	//Set  Item Icon for the inventory
	FORCEINLINE void SetIconItem(UTexture2D* Icon) { IconItem = Icon; }
	// Set ammo icon for the pickup widget
	FORCEINLINE void SetAmmoIcon(UTexture2D* Icon) { AmmoIcon = Icon; }

	FORCEINLINE void SetMaterialInstance(UMaterialInstance* Instance) { MaterialInstance = Instance; }
	FORCEINLINE UMaterialInstance* GetMaterialInstance() const { return MaterialInstance; }

	FORCEINLINE void SetDynamicMaterialInstance(UMaterialInstanceDynamic* Instance) { DynamicMaterialInstance = Instance; }
	FORCEINLINE UMaterialInstanceDynamic* GetDynamicMaterialInstance() const { return DynamicMaterialInstance; }

	FORCEINLINE FLinearColor GetGlowColor() const { return GlowColor; }
	FORCEINLINE int32 GetMaterialIndex() const { return MaterialIndex; }
	FORCEINLINE void SetMaterialIndex(int32 Index) { MaterialIndex = Index; }

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
	virtual void InitializeCustomDepth();
	void DisableGlowMaterial();

	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }

	FORCEINLINE void SetCharacter(AShooterCharacter* Char) { Character = Char; }

	FORCEINLINE void SetCharacterInventoryFull(bool bFull) {  bCharacterInventoryIsFull = bFull; }
};