// Fill out your copyright notice in the Description page of Project Settings.


#include "HeightMapGenerator.h"
#include <math.h>


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
				PerlinNoise[i][j] += SmoothNoises[octave][i][j] * MaxHeight;
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
	PerlinNoise(SizeOnOneAxis);
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

void UHeightMapGenerator::GenerateNewHeightMap(uint32_t SizeOnOneAxis) {
	const int GRID_SIZE = 400;

	HeightArray = new double* [SizeOnOneAxis];
	for (uint32_t i = 0; i < SizeOnOneAxis; i++) {
		HeightArray[i] = new double[SizeOnOneAxis];
	}

	float max = 0;
	float min = 100;

	int test;
	for (uint32_t x = 0; x < SizeOnOneAxis; x++)
	{
		for (uint32_t y = 0; y < SizeOnOneAxis; y++)
		{
			HeightArray[x][y] = (float)rand() / RAND_MAX;
		}
		test = x;
	}

	float rx = ((float) rand() / RAND_MAX) * 20000000;
	float ry = ((float) rand() / RAND_MAX) * 20000000;

	for (uint32_t x = 0; x < SizeOnOneAxis; x++)
	{
		for (uint32_t y = 0; y < SizeOnOneAxis; y++)
		{
			int xi;
			int xj;

			if (TrueRandomness) {
				xi = x + rx;
				xj = y + ry;
			}
			else {
				xi = x;
				xj = y;
			}

			float val = 0;

			float freq = Frequency;
			float amp = Amplitude;

			for (uint32_t i = 0; i < Octaves; i++)
			{
				val += perlin(xi * freq / GRID_SIZE, xj * freq / GRID_SIZE) * amp;

				freq *= 2;
				amp /= 2;

			}

			// Contrast
			val = (((val + 1.0f) * 0.5f));

			// Clipping
			if (val < -1.0f)
				val = -1.0f;

			HeightArray[x][y] = val * MaxHeight;

			if (val > max) {
				max = val;
			}
			if (val < min) {
				min = val;
			}
		}
	}

	SmoothHeightMap(SizeOnOneAxis);
	TranslateIntoTArray(SizeOnOneAxis);
}

vector2 UHeightMapGenerator::randomGradient(int ix, int iy) {
	
	// No precomputed gradients mean this works for any number of grid coordinates
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;
	unsigned a = ix, b = iy;
	a *= 3284157443;

	b ^= a << s | a >> (w - s);
	b *= 1911520717;

	a ^= b << s | b >> (w - s);
	a *= 2048419325;
	float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

	// Create the vector from the angle
	vector2 v;
	v.x = sin(random);
	v.y = cos(random);

	return v;
}

// Computes the dot product of the distance and gradient vectors.
float UHeightMapGenerator::dotGridGradient(int ix, int iy, float x, float y) {
	// Get gradient from integer coordinates
	vector2 gradient = randomGradient(ix, iy);

	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;

	// Compute the dot-product
	return (dx * gradient.x + dy * gradient.y);
}

float UHeightMapGenerator::interpolate(float a0, float a1, float w)
{
	return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}


// Sample Perlin noise at coordinates x, y
float UHeightMapGenerator::perlin(float x, float y) {

	// Determine grid cell corner coordinates
	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	// Compute Interpolation weights
	float sx = x - (float)x0;
	float sy = y - (float)y0;

	// Compute and interpolate top two corners
	float n0 = dotGridGradient(x0, y0, x, y);
	float n1 = dotGridGradient(x1, y0, x, y);
	float ix0 = interpolate(n0, n1, sx);

	// Compute and interpolate bottom two corners
	n0 = dotGridGradient(x0, y1, x, y);
	n1 = dotGridGradient(x1, y1, x, y);
	float ix1 = interpolate(n0, n1, sx);

	// Final step: interpolate between the two previously interpolated values, now in y
	float value = interpolate(ix0, ix1, sy);

	return value;
}