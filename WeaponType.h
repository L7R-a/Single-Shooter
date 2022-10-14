#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AsaultRifle"),
	EWT_Pistol UMETA (DisplayName = "Psitol"),
	EWT_MAX UMETA(DisplayName = "DefaultMax")
};