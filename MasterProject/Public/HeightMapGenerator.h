// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeightMapGenerator.generated.h"

typedef struct {
	float x, y;
} vector2;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MASTERPROJECT_API UHeightMapGenerator : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHeightMapGenerator();

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	uint16 HeightDataValue = 32768;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	uint16 MaxHeight = 10000;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	double Amplitude = 1;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	double Frequency = 1;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	double Persistance = 0.55;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	uint16 SmoothingIterations = 1;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	uint16 Octaves = 12;

	double** HeightArray;

	TArray<uint16> UHeightData;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	bool TrueRandomness;



protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GenerateHeightMap(uint32_t SizeOnOneAxis);

	virtual void GenerateNewHeightMap(uint32_t SizeOnOneAxis);

private:
	virtual double Interpolate(double a, double b, double alpha);

	virtual double** SmoothNoise(double** noise, uint32_t SizeOnOneAxis, int octave);

	virtual void PerlinNoise(uint32_t SizeOnOneAxis);

	virtual double** WhiteNoise(uint32_t SizeOnOneAxis);

	virtual void TranslateIntoTArray(uint32_t SizeOnOneAxis);

	virtual void SmoothHeightMap(uint32_t SizeOnOneAxis);

	virtual vector2 randomGradient(int ix, int iy);

	virtual float dotGridGradient(int ix, int iy, float x, float y);

	virtual float interpolate(float a0, float a1, float w);

	virtual float perlin(float x, float y);
};