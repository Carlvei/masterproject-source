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

double** UHeightMapGenerator::SmoothNoise(double** Noise, uint32_t SizeOnOneAxis, int Octave) {
	double** SmoothNoise = new double* [SizeOnOneAxis];
	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		SmoothNoise[i] = new double[SizeOnOneAxis];
	}

	int Period = 1 << Octave;
	double Frequenzy = 1.0 / Period;

	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		int SampleI0 = i / Period * Period;
		int SampleI1 = (SampleI0 + Period) % SizeOnOneAxis;
		double HorizontalBlend = (i - SampleI0) * Frequenzy;

		for (uint32_t j = 0; j < SizeOnOneAxis; j++) {
			int SampleJ0 = j / Period * Period;
			int SampleJ1 = (SampleJ0 + Period) % SizeOnOneAxis;
			double VerticalBlend = (j - SampleJ0) * Frequenzy;

			double Top = Interpolate(Noise[SampleI0][SampleJ0],
				Noise[SampleI1][SampleJ0],
				HorizontalBlend);
			double Bottom = Interpolate(Noise[SampleI0][SampleJ1],
				Noise[SampleI1][SampleJ1],
				HorizontalBlend);
			SmoothNoise[i][j] = Interpolate(Top, Bottom, VerticalBlend);
		}
	}

	return SmoothNoise;
}

void UHeightMapGenerator::PerlinNoise(uint32_t SizeOnOneAxis) {
	double** Noise = WhiteNoise(SizeOnOneAxis);
	double** PerlinNoise = new double* [SizeOnOneAxis];
	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		PerlinNoise[i] = new double[SizeOnOneAxis];
	}
	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		for (uint32_t j = 0; j < SizeOnOneAxis; j++) {
			PerlinNoise[i][j] = 0;
		}
	}

	double*** SmoothNoises = new double** [Octaves];
	for (int i = 0; i < Octaves; i++) {
		SmoothNoises[i] = SmoothNoise(Noise, SizeOnOneAxis, i);
	}

	for (int octave = Octaves - 1; octave > 0; octave--) {
		Amplitude *= Persistance;

		for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
			for (uint32_t j = 0; j < SizeOnOneAxis; j++) {
				PerlinNoise[i][j] += SmoothNoises[octave][i][j] * Amplitude;
			}
			delete[] SmoothNoises[octave][i];
		}
		delete[] SmoothNoises[octave];
	}
	delete[] SmoothNoises;

	HeightArray = PerlinNoise;
}

double** UHeightMapGenerator::WhiteNoise(uint32_t SizeOnOneAxis) {
	double** Noise = new double* [SizeOnOneAxis];
	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		Noise[i] = new double[SizeOnOneAxis];
	}

	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		for (uint32_t j = 0; j < SizeOnOneAxis; j++) {
			Noise[i][j] = (float)rand() / RAND_MAX;
		}
	}

	return Noise;
}

void UHeightMapGenerator::GenerateHeightMap(uint32_t SizeOnOneAxis) {
	PerlinNoise(SizeOnOneAxis, 8);
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
