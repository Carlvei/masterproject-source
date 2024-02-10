// Fill out your copyright notice in the Description page of Project Settings.


#include "HeightMapGenerator.h"

// Sets default values for this component's properties
UHeightMapGenerator::UHeightMapGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHeightMapGenerator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHeightMapGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

double UHeightMapGenerator::Interpolate(double a, double b, double alpha) {
	return a * (1 - alpha) + alpha * b;
}

double** UHeightMapGenerator::SmoothNoise(double** noise, uint32_t SizeOnOneAxis, int octave) {
	double** smooth_noise = new double* [SizeOnOneAxis];
	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		smooth_noise[i] = new double[SizeOnOneAxis];
	}

	int period = 1 << octave;
	double frequency = 1.0 / period;

	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		int sample_i0 = i / period * period;
		int sample_i1 = (sample_i0 + period) % SizeOnOneAxis;
		double horizontal_blend = (i - sample_i0) * frequency;

		for (uint32_t j = 0; j < SizeOnOneAxis; j++) {
			int sample_j0 = j / period * period;
			int sample_j1 = (sample_j0 + period) % SizeOnOneAxis;
			double vertical_blend = (j - sample_j0) * frequency;

			double top = Interpolate(noise[sample_i0][sample_j0],
				noise[sample_i1][sample_j0],
				horizontal_blend);
			double bottom = Interpolate(noise[sample_i0][sample_j1],
				noise[sample_i1][sample_j1],
				horizontal_blend);
			smooth_noise[i][j] = Interpolate(top, bottom, vertical_blend);
		}
	}

	return smooth_noise;
}

void UHeightMapGenerator::PerlinNoise(uint32_t SizeOnOneAxis, int octaves, double persistance, double amplitude, double totalamplitude) {
	double** noise = WhiteNoise(SizeOnOneAxis);
	double** perlin_noise = new double* [SizeOnOneAxis];
	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		perlin_noise[i] = new double[SizeOnOneAxis];
	}
	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		for (uint32_t j = 0; j < SizeOnOneAxis; j++) {
			perlin_noise[i][j] = 0;
		}
	}

	double*** smooth_noises = new double** [octaves];
	for (int i = 0; i < octaves; i++) {
		smooth_noises[i] = SmoothNoise(noise, SizeOnOneAxis, i);
	}

	for (int octave = octaves - 1; octave > 0; octave--) {
		amplitude *= persistance;
		totalamplitude += amplitude;

		for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
			for (uint32_t j = 0; j < SizeOnOneAxis; j++) {
				perlin_noise[i][j] += smooth_noises[octave][i][j] * amplitude;
			}
			delete[] smooth_noises[octave][i];
		}
		delete[] smooth_noises[octave];
	}
	delete[] smooth_noises;

	HeightArray = perlin_noise;
}

double** UHeightMapGenerator::WhiteNoise(uint32_t SizeOnOneAxis) {
	double** noise = new double* [SizeOnOneAxis];
	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		noise[i] = new double[SizeOnOneAxis];
	}

	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		for (uint32_t j = 0; j < SizeOnOneAxis; j++) {
			noise[i][j] = (float)rand() / RAND_MAX;
		}
	}

	return noise;
}

void UHeightMapGenerator::GenerateHeightMap(uint32_t SizeOnOneAxis) {
	PerlinNoise(SizeOnOneAxis, 8, Persistance, Amplitude, TotalAmplitude);
	SmoothHeightMap(SizeOnOneAxis);
	TranslateIntoTArray(SizeOnOneAxis);
}

void UHeightMapGenerator::SmoothHeightMap(uint32_t SizeOnOneAxis) {
	for (uint32_t iterations = 0; iterations < SmoothingIterations; iterations++) {
		for (uint32_t x = 0; x < SizeOnOneAxis; x++) {
			for (uint32_t y = 0; y < SizeOnOneAxis; y++) {
				if (x != 0 && x != SizeOnOneAxis - 1 && y != 0 && y != SizeOnOneAxis - 1) {
					double sum = HeightArray[x - 1][y - 1] +
						HeightArray[x - 1][y] +
						HeightArray[x - 1][y + 1] +
						HeightArray[x][y - 1] +
						HeightArray[x][y] +
						HeightArray[x][y + 1] +
						HeightArray[x + 1][y - 1] +
						HeightArray[x + 1][y] +
						HeightArray[x + 1][y + 1];

					double average = sum / (double)9;

					HeightArray[x][y] = average;
				}
			}
		}
	}
}

void UHeightMapGenerator::TranslateIntoTArray(uint32_t SizeOnOneAxis) {
	UHeightData.SetNum(SizeOnOneAxis * SizeOnOneAxis);

	for (uint32_t x = 0; x < SizeOnOneAxis; x++) {
		for (uint32_t y = 0; y < SizeOnOneAxis; y++) {
			int index = x + SizeOnOneAxis * y;

			UHeightData[index] = HeightArray[x][y] + HeightDataValue;
		}
	}
}
