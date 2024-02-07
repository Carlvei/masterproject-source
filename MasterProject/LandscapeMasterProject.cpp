// Fill out your copyright notice in the Description page of Project Settings.
#include <iostream>
#include <array>
#include <random>
#include "LandscapeMasterProject.h"
#include <Editor/LandscapeEditor/Public/LandscapeEditorObject.h>
#include "Landscape.h"
#include <math.h>

#include <iostream>
#include <conio.h>

UE_DISABLE_OPTIMIZATION
double interpolate(double a, double b, double alpha) {
	return a * (1 - alpha) + alpha * b;
}

double** smooth_noise(double** noise, int w, int h, int octave) {
	double** smooth_noise = new double* [w];
	for (int i = 0; i < w; i++) {
		smooth_noise[i] = new double[h];
	}

	int period = 1 << octave;
	double frequency = 1.0 / period;

	for (int i = 0; i < w; i++) {
		int sample_i0 = i / period * period;
		int sample_i1 = (sample_i0 + period) % w;
		double horizontal_blend = (i - sample_i0) * frequency;

		for (int j = 0; j < h; j++) {
			int sample_j0 = j / period * period;
			int sample_j1 = (sample_j0 + period) % h;
			double vertical_blend = (j - sample_j0) * frequency;

			double top = interpolate(noise[sample_i0][sample_j0],
				noise[sample_i1][sample_j0],
				horizontal_blend);
			double bottom = interpolate(noise[sample_i0][sample_j1],
				noise[sample_i1][sample_j1],
				horizontal_blend);
			smooth_noise[i][j] = interpolate(top, bottom, vertical_blend);
		}
	}

	return smooth_noise;
}

double** perlin_noise(double** noise, int w, int h, int octaves,
	double persistance, double amplitude, double totalamplitude) {
	double** perlin_noise = new double* [w];
	for (int i = 0; i < w; i++) {
		perlin_noise[i] = new double[h];
	}
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			perlin_noise[i][j] = 0;
		}
	}

	double*** smooth_noises = new double** [octaves];
	for (int i = 0; i < octaves; i++) {
		smooth_noises[i] = smooth_noise(noise, w, h, i);
	}

	for (int octave = octaves - 1; octave > 0; octave--) {
		amplitude *= persistance;
		totalamplitude += amplitude;

		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				perlin_noise[i][j] += smooth_noises[octave][i][j] * amplitude;
			}
			delete[] smooth_noises[octave][i];
		}
		delete[] smooth_noises[octave];
	}
	delete[] smooth_noises;
	
	/*
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			perlin_noise[i][j] /= totalamplitude;
		}
	}
	*/
	return perlin_noise;
}

double** white_noise(int w, int h) {
	double** noise = new double* [w];
	for (int i = 0; i < w; i++) {
		noise[i] = new double[h];
	}

	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			noise[i][j] = (float)rand() / RAND_MAX;
		}
	}

	return noise;
}


// Sets default values
ALandscapeMasterProject::ALandscapeMasterProject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALandscapeMasterProject::BeginPlay()
{
	Super::BeginPlay();


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

	TArray<uint16> HeightData;

	const int w = TotalSizeOnOneAxis;
	const int h = TotalSizeOnOneAxis;

	double** noise = white_noise(w, h);
	double** perlinnoise = perlin_noise(noise, w, h, 8, Persistance, Amplitude, TotalAmplitude);
	for (int iterations = 0; iterations < SmoothingIterations; iterations++) {
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				if (x != 0 && x != w - 1 && y != 0 && y != h - 1) {
					double sum = perlinnoise[x - 1][y - 1] +
						perlinnoise[x - 1][y] +
						perlinnoise[x - 1][y + 1] +
						perlinnoise[x][y - 1] +
						perlinnoise[x][y] +
						perlinnoise[x][y + 1] +
						perlinnoise[x + 1][y - 1] +
						perlinnoise[x + 1][y] +
						perlinnoise[x + 1][y + 1];

					double average = sum / (double)9;

					perlinnoise[x][y] = average;
				}
			}
		}
	}

	// Code that does things will go here(function calls to add sea-level etc.)
	/*
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(1,1);

	uint32_t size = TotalSizeOnOneAxis;
	uint32_t step = 4096;
	uint32_t half;
	uint32_t randomScale = 1;

	uint32_t FullSize = size * size;
	uint32_t* map = new uint32_t[FullSize];

	for (uint32_t x = 0; x < size; x++) {
		for (uint32_t y = 0; y < size; y++) {
			*(map + x * size + y) = 0;
		}
	}


	for (uint32_t x = 0; x < size; x += step) {
		for (uint32_t y = 0; y < size; y += step) {
			int test = dist6(rng);
			*(map + x * size + y) = test;
		}
	}



	while (step > 1)
	{
		half = step / 2;

		// diamondstep
		for (uint32_t x = half; x < size; x += step) {
			for (uint32_t y = half; y < size; y += step) {
				float value = 0;
				value = *(map + (x - half) * size + (y- half)) +
					*(map + (x + half) * size + (y - half)) +
					*(map + (x - half) * size + (y + half)) +
					*(map + (x + half) * size + (y + half));

				value /= 4.0f;
				value += dist6(rng) * randomScale;
				*(map + x * size + y) = value;
			}
		}

		for (uint32_t x = 0; x < size; x += half) {
			for (uint32_t y = (x + half) % step; y < size; y += step) {
				float value = 0;
				uint32_t count = 0;

				uint32_t idx = (x - half) * size + y;
				if (idx < (size * size) && idx >= 0) {
					value += *(map + idx);
					count++;
				}
				idx = (x + half) * size + y;
				if (idx < (size * size)) {
					value += *(map + idx);
					count++;
				}
				idx = x * size + y - half;
				if (idx < (size * size) && (y - half) >= 0) {
					value += *(map + idx);
					count++;
				}
				idx = x * size + y + half;
				if (idx < (size * size) && y + half < size) {
					value += *(map + idx);
					count++;
				}

				value /= count;
				value += dist6(rng) * randomScale;
				*(map + x * size + y) = value;
			}
		}

		randomScale /= 2.f;
		step /= 2;
	}
	*/
	HeightData.SetNum(TotalSizeOnOneAxis * TotalSizeOnOneAxis);

	int32 counter = 0;
	for (uint32_t x = 0; x < TotalSizeOnOneAxis; x++) {
		for (uint32_t y = 0; y < TotalSizeOnOneAxis; y++) {
			int index = x * TotalSizeOnOneAxis + y;

			HeightData[index] = perlinnoise[x][y]  + HeightDataValue;
		}
	}
	HeightDataPerLayers.Add(FGuid(), MoveTemp(HeightData));
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
	Landscape->SetActorScale3D(ScaleVector);

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

}

// Called every frame
void ALandscapeMasterProject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

