// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeightMapGenerator.generated.h"

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
	FVector ScaleVector = FVector(128, 128, 128);

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	uint16 Amplitude = 3000;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	uint16 TotalAmplitude = 0;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	double Persistance = 0.55;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	uint16 SmoothingIterations = 1;

	double** HeightArray;

	TArray<uint16> UHeightData;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GenerateHeightMap(uint32_t SizeOnOneAxis);

private:
	virtual double Interpolate(double a, double b, double alpha);

	virtual double** SmoothNoise(double** noise, uint32_t SizeOnOneAxis, int octave);

	virtual void PerlinNoise(uint32_t SizeOnOneAxis, int octaves,
		double persistance, double amplitude, double totalamplitude);

	virtual double** WhiteNoise(uint32_t SizeOnOneAxis);

	virtual void TranslateIntoTArray(uint32_t SizeOnOneAxis);

	virtual void SmoothHeightMap(uint32_t SizeOnOneAxis);
};