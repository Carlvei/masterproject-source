// Fill out your copyright notice in the Description page of Project Settings.
#include "Landscape.h"
#include "LandscapeMasterProject.h"

#include <math.h>
#include <conio.h>


#include <iostream>
#include <array>



UE_DISABLE_OPTIMIZATION
// Sets default values
ALandscapeMasterProject::ALandscapeMasterProject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	LandscapeGenerator = CreateDefaultSubobject<ULandscapeGenerator>(TEXT("LandscapeGenerator"));

}

// Called when the game starts or when spawned
void ALandscapeMasterProject::BeginPlay()
{
	Super::BeginPlay();

	LandscapeGenerator->GenerateLandscape(FTransform(FVector(0)));
}

// Called every frame
void ALandscapeMasterProject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


