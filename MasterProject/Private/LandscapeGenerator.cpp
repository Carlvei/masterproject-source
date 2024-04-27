// Fill out your copyright notice in the Description page of Project Settings.


#include "LandscapeGenerator.h"
#include "HeightMapGenerator.h"

#include <iostream>
#include <random>



// Sets default values for this component's properties
ULandscapeGenerator::ULandscapeGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	HeightMapGenerator = CreateDefaultSubobject<UHeightMapGenerator>(TEXT("HeightMapGenerator"));
	FoliageGenerator = CreateDefaultSubobject<UFoliageGenerator>(TEXT("FoliageGenerator"));
	// ...
}


// Called when the game starts
void ULandscapeGenerator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULandscapeGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULandscapeGenerator::CalculateParameters() {
	SectionsPerComponent = static_cast<int>(SectionsPerComponentInput); // 4
	ComponentsOnOneAxis = static_cast<int>(ComponentsOnOneAxisInput); // 32
	QuadsPerSectionOnOneAxis = static_cast<int>(QuadsPerSectionOnOneAxisInput); // 63

	TotalComponents = ComponentsOnOneAxis * ComponentsOnOneAxis; // 1024
	QuadsPerSection = QuadsPerSectionOnOneAxis * QuadsPerSectionOnOneAxis; // 3969
	QuadsPerComponent = QuadsPerSection * SectionsPerComponent; // 3969 * 4 = 15876
	ComponentSize = sqrt(QuadsPerComponent); // 126
	TotalSizeOnOneAxis = ComponentSize * ComponentsOnOneAxis + 1; // 
}

void ULandscapeGenerator::GenerateHeightmap() {
	HeightMapGenerator->GenerateNewHeightMap(TotalSizeOnOneAxis);
	HeightDataPerLayers.Add(FGuid(), MoveTemp(HeightMapGenerator->UHeightData));
}

void ULandscapeGenerator::GenerateMaterialImportLayers() {
	TArray<FLandscapeImportLayerInfo> MaterialImportLayers;
	MaterialImportLayers.Reserve(0);
	// ComputeHeightData will also modify/expand material layers data, which is why we create MaterialLayerDataPerLayers after calling ComputeHeightData
	MaterialLayerDataPerLayers.Add(FGuid(), MoveTemp(MaterialImportLayers));
}

void ULandscapeGenerator::InitializeWorldContext() {
	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();
}

void ULandscapeGenerator::GenerateLandscape(FTransform LandscapeTransform) {

	if (GenerateNewLandscape) {
		CalculateParameters();
		GenerateHeightmap();
		GenerateMaterialImportLayers();
		InitializeWorldContext();
	
		Landscape = World -> SpawnActor<ALandscape>();
		Landscape->SetFolderPath("GeneratedLandscape");
		Landscape->bCanHaveLayersContent = false;
		Landscape->LandscapeMaterial = GroundMaterial;
		Landscape->SetActorTransform(LandscapeTransform);
		Landscape->SetActorScale3D(ScaleVector);

		Landscape->Import(FGuid::NewGuid(), 0, 0, TotalSizeOnOneAxis - 1, TotalSizeOnOneAxis - 1, SectionsPerComponent, QuadsPerSectionOnOneAxis, HeightDataPerLayers, nullptr, MaterialLayerDataPerLayers, ELandscapeImportAlphamapType::Additive);

		Landscape->StaticLightingLOD = FMath::DivideAndRoundUp(FMath::CeilLogTwo((TotalSizeOnOneAxis * TotalSizeOnOneAxis) / (2048 * 2048) + 1), (uint32)2);
		// Register all the landscape components
		ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo();
		LandscapeInfo->UpdateLayerInfoMap(Landscape);
		Landscape->RegisterAllComponents();

		// Need to explicitly call PostEditChange on the LandscapeMaterial property or the landscape proxy won't update its material
		FPropertyChangedEvent MaterialPropertyChangedEvent(FindFieldChecked< FProperty >(Landscape->GetClass(), FName("LandscapeMaterial")));
		Landscape->PostEditChangeProperty(MaterialPropertyChangedEvent);
		Landscape->PostEditChange();

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Landscape was created!"));

		if (FoliageGenerator->DoGenerateFoliage) {
			GenerateFoliage();
		}
	}
}

void ULandscapeGenerator::GenerateFoliage() {
	FoliageGenerator->GenerateFoliage(TotalSizeOnOneAxis, HeightMapGenerator->HeightArray, ScaleVector);
}