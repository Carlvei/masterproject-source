// Fill out your copyright notice in the Description page of Project Settings.


#include "FoliageGenerator.h"
#include "Engine/StaticMeshActor.h"
#include <random>

void SpawnActor(UWorld* World, UStaticMesh* Mesh, FVector Location, std::string path, FVector MeshScale) {
	AStaticMeshActor* NewActor = World -> SpawnActor<AStaticMeshActor>();
	NewActor -> SetFolderPath(path.c_str());
	NewActor -> SetActorLocation(Location);
	NewActor->SetActorScale3D(MeshScale);
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
			ComputePointInArray(x, y, Scale, HeightArray);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Foliage was generated!"));
}

void UFoliageGenerator::ComputePointInArray(uint32_t x, uint32_t y, FVector Scale, double** HeightArray) {
	FStaticMeshInput MeshInput = Meshes[0];
	float rand = random();

	if (rand < MeshInput.SpawnProbability) {
		std::random_device LocationRandomDevice; // obtain a random number from hardware
		std::mt19937 LocationRandomGen(LocationRandomDevice()); // seed the generator
		std::uniform_real_distribution<> LocationRandomDistr(-1, 1);

		float RandLocationX = (LocationRandomDistr(LocationRandomGen) * Scale.X) / 2;
		float RandLocationY = (LocationRandomDistr(LocationRandomGen) * Scale.Y) / 2;

		FVector Location = FVector(x * Scale.X,
			y * Scale.Y,
			HeightArray[x][y]);

		FVector MeshScale;
		if (MeshInput.MinSize != 1 || MeshInput.MaxSize != 1) {
			std::random_device rd; // obtain a random number from hardware
			std::mt19937 gen(rd()); // seed the generator
			std::uniform_real_distribution<> distr(MeshInput.MinSize, MeshInput.MaxSize);

			float scale = distr(gen);
			MeshScale = FVector(scale, scale, scale);
		}
		else {
			MeshScale = FVector(1, 1, 1);
		}

		SpawnActor(World, MeshInput.Mesh, Location, GenerateFolderName(MeshInput.FolderName), MeshScale);

		/*
		for (FStaticMeshInput MeshInput : Meshes) {
			float rand = random();

			if (rand < MeshInput.SpawnProbability) {
				std::random_device LocationRandomDevice; // obtain a random number from hardware
				std::mt19937 LocationRandomGen(LocationRandomDevice()); // seed the generator
				std::uniform_real_distribution<> LocationRandomDistr(-1, 1);

				float RandLocationX = (LocationRandomDistr(LocationRandomGen) * Scale.X) / 2;
				float RandLocationY = (LocationRandomDistr(LocationRandomGen) * Scale.Y) / 2;

				FVector Location = FVector(x * Scale.X + RandLocationX,
					y * Scale.Y + RandLocationY,
					HeightArray[x][y]);

				FVector MeshScale;
				if (MeshInput.MinSize != 1 || MeshInput.MaxSize != 1) {
					std::random_device rd; // obtain a random number from hardware
					std::mt19937 gen(rd()); // seed the generator
					std::uniform_real_distribution<> distr(MeshInput.MinSize, MeshInput.MaxSize);

					float scale = distr(gen);
					MeshScale = FVector(scale, scale, scale);
				} else {
					MeshScale = FVector(1, 1, 1);
				}

				SpawnActor(World, MeshInput.Mesh, Location, GenerateFolderName(MeshInput.FolderName), MeshScale);
			}
		}
		*/
	}
}

std::string UFoliageGenerator::GenerateFolderName(FString FolderName) {
	std::string Result = "GeneratedLandscape/Foliage";

	if (!FolderName.IsEmpty()) {
		Result += std::string("/");
		Result += std::string(TCHAR_TO_ANSI(*FolderName));
	}

	return Result;
}