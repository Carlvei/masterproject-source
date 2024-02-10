// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Public/LandscapeGenerator.h"
#include "LandscapeMasterProject.generated.h"


UCLASS()
class MASTERPROJECT_API ALandscapeMasterProject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALandscapeMasterProject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ULandscapeGenerator* LandscapeGenerator;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};


