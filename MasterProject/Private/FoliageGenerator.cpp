// Fill out your copyright notice in the Description page of Project Settings.


#include "FoliageGenerator.h"
#include "Engine/StaticMeshActor.h"

void SpawnActor(UWorld* World, UStaticMesh* Mesh, FVector Location, const char* path) {
	AStaticMeshActor* NewActor = World -> SpawnActor<AStaticMeshActor>();
	NewActor -> SetFolderPath(path);
	NewActor -> SetActorLocation(Location);
	UStaticMeshComponent* MeshComponent = NewActor -> GetStaticMeshComponent();
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(Mesh);
	}
}

float random() {
	return (float)rand() / RAND_MAX;
}

// Sets default values for this component's properties
UFoliageGenerator::UFoliageGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFoliageGenerator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFoliageGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFoliageGenerator::GenerateFoliage(uint32_t TotalSizeOnOneAxis, double** HeightArray, FVector Scale) {
	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();

	for (uint32_t x = 0; x < TotalSizeOnOneAxis; x++) {
		for (uint32_t y = 0; y < TotalSizeOnOneAxis; y++) {
			float rand = random();
			if (rand < SpawnProbability) {
				SpawnActor(World, Mesh, FVector(x * 128, y * 128, HeightArray[x][y]), "GeneratedLandscape/Foliage");
			}
		}
	}
}