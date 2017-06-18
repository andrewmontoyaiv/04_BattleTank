// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleTank.h"
// it didnt understand the type without this. if you need to call a method, you need to include it. 
// All a header file needs to know is that the type exists. this keeps the compiler happy without needing a chain
// of includes. the cpp only needs the include
#include "TankBarrel.h"	
#include "TankTurret.h"
#include "Projectile.h"
#include "TankAimingComponent.h"


// Sets default values for this component's properties
UTankAimingComponent::UTankAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false; 

	// ...
}

void UTankAimingComponent::Initialize(UTankBarrel* BarrelToSet, UTankTurret* TurretToSet) {
	if (!ensure (BarrelToSet && TurretToSet)) { return; }
	Barrel = BarrelToSet;
	Turret = TurretToSet;
}

void UTankAimingComponent::Fire() {
	if (!ensure(Barrel && ProjectileBlueprint)) { return; }
	UE_LOG(LogTemp, Warning, TEXT("Is fire working?"));
	bool isReloaded = FPlatformTime::Seconds() - LastFireTime > ReloadTimeInSeconds; // also same as getworld, getTimeSeconds
	if (isReloaded) {
		// spawn a projectile from the socket location of the turret
		auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileBlueprint,
			Barrel->GetSocketLocation(FName("Projectile")),
			Barrel->GetSocketRotation(FName("Projectile"))
			);
		Projectile->LaunchProjectile(LaunchSpeed);
		LastFireTime = FPlatformTime::Seconds();
	}
}


void UTankAimingComponent::AimAt(FVector HitLocation) const {
	// auto OurTankName = GetOwner()->GetName();
	// auto BarrelLocation = Barrel->GetComponentLocation();
	// UE_LOG(LogTemp, Warning, TEXT("%s aiming at %s from %s"), *OurTankName, *HitLocation.ToString(), *BarrelLocation.ToString())

	if (!ensure(Barrel)) { return; }  // protect

	FVector OutLaunchVelocity;
	FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));

	// calculate the out launch velocity
	if (UGameplayStatics::SuggestProjectileVelocity(
		Barrel, // or use "this"
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		LaunchSpeed,
		false,
		0.0f,
		0.0f,
		ESuggestProjVelocityTraceOption::DoNotTrace, // you do not need to include params after this point
		FCollisionResponseParams::DefaultResponseParam,
		TArray<AActor*>(),
		false
		)) {
			auto TankName = GetOwner()->GetName();
			auto AimDirection = OutLaunchVelocity.GetSafeNormal();
			// UE_LOG(LogTemp, Warning, TEXT("%s Aiming at %s"), *TankName, *AimDirection.ToString());
			MoveBarrelTowards(AimDirection);
			auto Time = GetWorld()->GetTimeSeconds();
			// UE_LOG(LogTemp, Warning, TEXT("%f: Aim Solution Found"), Time);
		}
	else {
		auto Time = GetWorld()->GetTimeSeconds();
		// UE_LOG(LogTemp, Warning, TEXT("%f: No aim solution found"), Time);
	}

	// if nothings found, do nothing
}

void UTankAimingComponent::MoveBarrelTowards(FVector AimDirection) const {
	if (!ensure (Barrel && Turret)) { return; }
	// work-out difference between current barrel rotation, and AimDirection

	// turning forward vector into rotation
	auto Rotator = Barrel->GetForwardVector().Rotation();
	auto AimAsRotator = AimDirection.Rotation();
	auto DeltaRotator = AimAsRotator - Rotator;

	// auto TurretRotator = Turret->GetForwardVector().Rotation();
	// auto TAimAsRotator = AimDirection.Rotation();

	// UE_LOG(LogTemp, Warning, TEXT("AimAsRotator: %s"), *AimAsRotator.ToString());

	Barrel->Elevate(DeltaRotator.Pitch);
	Turret->Azimuth(DeltaRotator.Yaw);
}
