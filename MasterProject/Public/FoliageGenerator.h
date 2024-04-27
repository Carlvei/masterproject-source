// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FoliageGenerator.generated.h"

USTRUCT(BlueprintType)
struct FStaticMeshInput {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString FolderName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere)
	float SpawnProbability = 0.0001;

	UPROPERTY(EditAnywhere)
	float MinSize = 1;

	UPROPERTY(EditAnywhere)
	float MaxSize = 1;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MASTERPROJECT_API UFoliageGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFoliageGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DoGenerateFoliage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStaticMeshInput> Meshes;

	UWorld* World;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GenerateFoliage(uint32_t TotalSizeOnOneAxis, double** HeightArray, FVector Scale);

	virtual std::string GenerateFolderName(FString MeshInput);

	virtual void ComputePointInArray(uint32_t x, uint32_t y, FVector Scale, double** HeightArray);
};
