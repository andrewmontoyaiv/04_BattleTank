// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "TankAimingComponent.generated.h"

// forward declaration, similar to:  class BATTLETANK_API UTankAimingComponent : public UActorComponent
// makes dependancies explicit, without creating a chain of dependancies
class UTankBarrel;
class UTankTurret;

// holds barrels properties and elevate method
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLETANK_API UTankAimingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTankAimingComponent();

	void SetBarrelReference(UTankBarrel* BarrelToSet);

	void SetTurretReference(UTankTurret* TurretToSet);
	// Be on the lookout for setting things a third time. If this is the case, consider doing it another way

	void AimAt(FVector HitLocation, float LaunchSpeed) const;

private:
	UTankBarrel* Barrel = nullptr;

	UTankTurret* Turret = nullptr;

	void MoveBarrelTowards(FVector AimDirection) const;
};
