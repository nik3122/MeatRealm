// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "DeathmatchGameMode.generated.h"

struct FMRHitResult;
struct FActorSpawnParameters;
class AHeroCharacter;
class AHeroController;
class AProjectile;

UCLASS()
class MEATREALM_API ADeathmatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADeathmatchGameMode();
	
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;
	bool ShouldSpawnAtStartSpot(AController* Player) override;

private:
	TMap<uint32, AHeroController*> ConnectedHeroControllers;
	TMap<uint32, FDelegateHandle> OnPlayerDieHandles;

	void OnPlayerTakeDamage(FMRHitResult Hit);
	bool EndGameIfFragLimitReached() const;
};
