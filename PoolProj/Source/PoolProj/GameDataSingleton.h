// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "GameDataSingleton.generated.h"

class USoundBase;

UCLASS(Blueprintable)
class POOLPROJ_API UGameDataSingleton : public UObject
{
	GENERATED_BODY()
	
public:

	static UGameDataSingleton* GetInstance() { return instance ? instance : (instance = Cast<UGameDataSingleton>(GEngine->GameSingleton)); }

	USoundBase* GetGainPointSfx() const { return GainPointSfx; }

	USoundBase* GetLostPointSfx() const { return LostPointSfx; }

	USoundBase* GetStrikeSfx() const { return StrikeSfx; }

protected:

	static UGameDataSingleton* instance;

	UPROPERTY(EditDefaultsOnly, Category = "GameDataSingleton")
		USoundBase* GainPointSfx;

	UPROPERTY(EditDefaultsOnly, Category = "GameDataSingleton")
		USoundBase* LostPointSfx;

	UPROPERTY(EditDefaultsOnly, Category = "GameDataSingleton")
		USoundBase* StrikeSfx;
};