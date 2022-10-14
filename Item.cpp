// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Curves/CurveVector.h"

// Sets default values
AItem::AItem():
	ItemName(FString("Default")),
	ItemCount (0),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState (EItemState::EIS_Pickup),
	//Item interp variables
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f),
	ItemType(EItemType::EIT_MAX),
	InterpLocIndex(0),
	MaterialIndex(0),
	bCanChangeCustomDepth(true),
	// Dynamic Material Parameters
	GlowAmount(150.f),
	FresnelExponent(3.f),
	FresnelReflectFraction(4.f),
	PulseCurveTime(5.f),
	SlotIndex(0),
	bCharacterInventoryIsFull(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Visibility,
		ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject <UWidgetComponent>(TEXT("Widget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Hide Pickup Widget
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	// Sets ActiveStars array based on Item Rarity
	SetActiveStars();

	//Set up overlap for AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this,&AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	//Set item properties based on itemstate
	SetItemProperties(ItemState);

	// Set custom depth to disable
	InitializeCustomDepth();

	StartPulseTimer();
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::StartPulseTimer()
{
	if (ItemState == EItemState::EIS_Pickup)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
			ShooterCharacter-> UnHighlightedInventorySlot();
		}
	}
}

void AItem::SetActiveStars()
{
	// The 0 element is not used
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
			break;
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Uncommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;	
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		//Set mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//Set collisionbox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility, 
			ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);
		//Set mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set collisionbox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		ItemMesh->SetVisibility(true);
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,
			ECollisionResponse::ECR_Block);
		//Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set collisionbox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility,
			ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(false);
		//Set mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Set collisionbox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_PickedUp:
		PickupWidget->SetVisibility(false);
		// Set mesh properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character)
	{
		//Subtract 1 from the Item Count of the interp location struct
		Character->IncrementInterpLocItemCount(InterpLocIndex, -1);
		Character->GetPickupItem(this);

		Character->UnHighlightedInventorySlot();
	}
	// Set scale back to normal
	SetActorScale3D(FVector(1.f));

	bCanChangeCustomDepth = true;
	DisableCustomDepth();
	DisableGlowMaterial();
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping)return;

	if (Character && ItemZCurve)
	{
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// Get curve value corresponding to ElapsedTime
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		// Get the item initial location when the curve started
		FVector ItemLocation = ItemInterpStartLocation;
		//Get location in front of the camera
		const FVector CameraInterpLocation{GetInterpLocation() };
		// Vector from item to Camera Interp Location, X and Y are zeroed out
		const FVector ItemToCamera{ FVector(0.f,0.f, (CameraInterpLocation - ItemLocation).Z )};
		// Scale factor to multiply with CurveValue
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation{ GetActorLocation() };
		//Interpolated X value
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X,
			CameraInterpLocation.X,
			DeltaTime,
			30.0f);

		//Interpolated Y value
		const float InterpYValue = FMath::FInterpTo(
			CurrentLocation.Y,
			CameraInterpLocation.Y,
			DeltaTime,
			30.f);

		// Set X and Y of ItemLocation to Interped values
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;


		// Adding curve value to the Z component of the Initial location (scaled by DeltaZ)
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		// Camera rotation this frame
		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		//Camera rotation plus initial Yaw Offset
		FRotator ItemRotation{ 0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
		const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
		SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}

}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr) return FVector(0.f);
	
	switch (ItemType)
	{
		case EItemType::EIT_Ammo:
			return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;

		case EItemType::EIT_Weapon:
			return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
			break;

	}
	return FVector();
			
}

void AItem::PlayPickupSound(bool bForcePlaySound)
{
	if (Character)
	{
		if (bForcePlaySound)
		{
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
		else if (Character->ShouldPlayPickupSound())
		{
			Character->StartPickupSoundTimer();
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::OnConstruction(const FTransform& Transform)
{

	// Load the data in the Item Rarity Data Table

	//Pathh to the Item Rarity Data Table
	FString RarityTablePath(TEXT("DataTable'/Game/_Game/DataTable/ItemRarityDataTable.ItemRarityDataTable'"));

	UDataTable* RarityTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *RarityTablePath));

	if (RarityTableObject)
	{
		FItemRarityTable* RarityRow = nullptr;

		switch (ItemRarity)
		{
		case EItemRarity::EIR_Damaged:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Damaged"), TEXT(""));
		break;
		case EItemRarity::EIR_Common:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));
		break;
		case EItemRarity::EIR_Uncommon:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Uncommon"), TEXT(""));
		break;
		case EItemRarity::EIR_Rare:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));
		break;
		case EItemRarity::EIR_Legendary:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Legendary"), TEXT(""));
		break;
		}

		if (RarityRow)
		{
			GlowColor = RarityRow->GlowColor;
			LightColor = RarityRow->LightColor;
			DarkColor = RarityRow->DarkColor;
			NumberOfStars = RarityRow->NumberOfStars;
			IconBackground = RarityRow->IconBackground;
			if (GetItemMesh())
			{
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
			}
		}
	}

	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("FresnelColor"), GlowColor);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);

		EnableGlowMaterial();
	}

}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}
}

void AItem::UpdatePulse()
{
	float ElapsedTime{};
	FVector CurveValue{};
	switch (ItemState)
	{
	case EItemState::EIS_Pickup:
		if (PulseCurve)
		{
		ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
		CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
		break;
		}
	case EItemState:: EIS_EquipInterping:
		if (InterpPulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
			CurveValue = InterpPulseCurve->GetVectorValue(ElapsedTime);
		}
		break;
	}
	if (DynamicMaterialInstance)
	{

	DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X * GlowAmount);
	DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), CurveValue.Y * FresnelExponent);
	DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelReflectFraction"), CurveValue.Z * FresnelReflectFraction);
	}
}

void AItem::DisableGlowMaterial()
{
	if (!DynamicMaterialInstance) return;
	DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
}

void AItem::EnableCustomDepth()
{
	if (!DynamicMaterialInstance) return;
	if (bCanChangeCustomDepth)
	{
	ItemMesh->SetRenderCustomDepth(true);
	}
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(false);
	}
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
	if (Character)
	{
		if (bForcePlaySound)
		{
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
		else if (Character->ShouldPlayEquipSound())
		{
			Character->StartEquipSoundTimer();
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handles ItemInterping when in the EquipInterping state
	ItemInterp(DeltaTime);

	// Get curve values from PulseCurve and set dynamic material parameters
	UpdatePulse();
}

void AItem::StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound)
{
	// Store a handle to the character
	Character = Char;

	// Get array index in InterpLocarions with the lowest item count
	InterpLocIndex = Character->GetInterpLocationIndex();
	// Add 1 to the Item Count for this interp location struct
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);

	PlayPickupSound(bForcePlaySound);

	// Store initial location of the Item
	ItemInterpStartLocation = GetActorLocation();

	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);
	GetWorldTimerManager().ClearTimer(PulseTimer);

	GetWorldTimerManager().SetTimer(
		ItemInterpTimer,
		this,
		&AItem::FinishInterping,
		ZCurveTime);

	// Get Initial Yaw of the camera
	const float CameraRotationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw };
	// Get Initial Yaw of the Item
	const float ItemRotationYaw{ GetActorRotation().Yaw };
	// Initial Yaw offset between camera and Item
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

	bCanChangeCustomDepth = false;
}

