// Fill out your copyright notice in the Description page of Project Settings.
#include "Landscape.h"
#include "LandscapeMasterProject.h"
#include "Engine/StaticMeshActor.h"
#include <Editor/LandscapeEditor/Public/LandscapeEditorObject.h>
#include <math.h>
#include <conio.h>

#include <iostream>
#include <iostream>
#include <array>
#include <random>


UE_DISABLE_OPTIMIZATION

void spawnActor(UWorld* World, UStaticMesh* Mesh, FVector Location) {
	AStaticMeshActor* NewActor = World->SpawnActor<AStaticMeshActor>();

	NewActor->SetActorLocation(Location);
	UStaticMeshComponent* MeshComponent = NewActor->GetStaticMeshComponent();
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(Mesh);
	}
}

float random() {
	return (float)rand() / RAND_MAX;
}


// Sets default values
ALandscapeMasterProject::ALandscapeMasterProject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HeightMapGenerator = CreateDefaultSubobject<UHeightMapGenerator>(TEXT("HeightMapGenerator"));

}

// Called when the game starts or when spawned
void ALandscapeMasterProject::BeginPlay()
{
	Super::BeginPlay();

	if (static_cast<int>(GenerateNewLandscape) == 1) {
		uint32_t SectionsPerComponent = static_cast<int>(SectionsPerComponentInput);
		uint32_t ComponentsOnOneAxis = static_cast<int>(ComponentsOnOneAxisInput);
		uint32_t QuadsPerSectionOnOneAxis = static_cast<int>(QuadsPerSectionOnOneAxisInput);

		uint32_t TotalComponents = ComponentsOnOneAxis * ComponentsOnOneAxis;
		uint32_t QuadsPerSection = QuadsPerSectionOnOneAxis * QuadsPerSectionOnOneAxis;
		uint32_t QuadsPerComponent = QuadsPerSection * SectionsPerComponent;
		uint32_t ComponentSize = sqrt(QuadsPerComponent);
		uint32_t TotalSizeOnOneAxis = ComponentSize * ComponentsOnOneAxis + 1;


		FTransform LandscapeTransform = FTransform(FVector(0));

		TArray<FLandscapeImportLayerInfo> MaterialImportLayers;
		MaterialImportLayers.Reserve(0);

		TMap<FGuid, TArray<uint16>> HeightDataPerLayers;
		TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;

		HeightMapGenerator->GenerateHeightMap(TotalSizeOnOneAxis);

		HeightDataPerLayers.Add(FGuid(), MoveTemp(HeightMapGenerator -> UHeightData));
		// ComputeHeightData will also modify/expand material layers data, which is why we create MaterialLayerDataPerLayers after calling ComputeHeightData
		MaterialLayerDataPerLayers.Add(FGuid(), MoveTemp(MaterialImportLayers));

		//FScopedTransaction Transaction(TEXT("Undo", "Creating New Landscape"));

		UWorld* World = nullptr;
		{
			// We want to create the landscape in the landscape editor mode's world
			FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
			World = EditorWorldContext.World();

		}

		ALandscape* Landscape = World->SpawnActor<ALandscape>();

		Landscape->bCanHaveLayersContent = false;
		Landscape->LandscapeMaterial = GroundMaterial;

		Landscape->SetActorTransform(LandscapeTransform);
		Landscape->SetActorScale3D(HeightMapGenerator -> ScaleVector);

		//const FGuid& InGuid, int32 InMinX, int32 InMinY, int32 InMaxX, int32 InMaxY, int32 InNumSubsections, int32 InSubsectionSizeQuads, const TMap<FGuid, TArray<uint16>>& InImportHeightData,
		//	const TCHAR* const InHeightmapFileName, const TMap<FGuid, TArray<FLandscapeImportLayerInfo>>& InImportMaterialLayerInfos, ELandscapeImportAlphamapType InImportMaterialLayerType, const TArray<struct FLandscapeLayer>* InImportLayers = nullptr

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

		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Landscape was created?!"));
		}
		
		for (uint32_t x = 0; x < TotalSizeOnOneAxis; x++) {
			for (uint32_t y = 0; y < TotalSizeOnOneAxis; y++) {
				float rand = random();
				if (rand < SpawnThreshhold) {
					spawnActor(World, Mesh, FVector(x * 128, y * 128, HeightMapGenerator -> HeightArray[x][y]));
				}
			}
		}
	}
}

// Called every frame
void ALandscapeMasterProject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


