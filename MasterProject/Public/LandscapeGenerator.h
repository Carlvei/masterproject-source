// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeightMapGenerator.h"
#include "Engine/StaticMeshActor.h"
#include <Editor/LandscapeEditor/Public/LandscapeEditorObject.h>
#include "LandscapeGenerator.generated.h"


UENUM()
enum class QuadsPerSectionOnOneAxis : uint16 {
	I1 = 1 UMETA(DisplayName = "1"),
	I3 = 3 UMETA(DisplayName = "3"),
	I7 = 7 UMETA(DisplayName = "7"),
	I15 = 15 UMETA(DisplayName = "15"),
	I31 = 31 UMETA(DisplayName = "31"),
	I63 = 63 UMETA(DisplayName = "63"),
	I127 = 127 UMETA(DisplayName = "127"),
	I255 = 255 UMETA(DisplayName = "255"), // das Sinnvollte um eine gute Größe zu bekommen, zumindest in Verbindung mit ComponentsOnOneAxis::16 oder 32 (wobei 32 recht lang braucht)
	I511 = 511 UMETA(DisplayName = "511") // absolut nicht empfohlen derzeit, die Sections die nahe am Spieler sind werden dann scheinbar einfach nicht geladen.
};

UENUM()
enum class SectionsPerComponent : uint16 {
	I1 = 1 UMETA(DisplayName = "1"), // ist das einzige das derzeit geht
	I4 = 4 UMETA(DisplayName = "4")
};

UENUM()
enum class ComponentsOnOneAxis : uint16 {
	I1 = 1 UMETA(DisplayName = "1"),
	I2 = 2 UMETA(DisplayName = "2"),
	I4 = 4 UMETA(DisplayName = "4"),
	I8 = 8 UMETA(DisplayName = "8"),
	I16 = 16 UMETA(DisplayName = "16"), //empfohlen
	I32 = 32 UMETA(DisplayName = "32"),
	I64 = 64 UMETA(DisplayName = "64")  // absolut keine gute Idee
};

UENUM()
enum class GenerateNewLandscape : uint16 {
	YES = 1 UMETA(DisplayName = "Yes"),
	NO = 0 UMETA(DisplayName = "No")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MASTERPROJECT_API ULandscapeGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULandscapeGenerator();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UHeightMapGenerator* HeightMapGenerator;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector ScaleVector = FVector(128, 128, 128);

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	SectionsPerComponent SectionsPerComponentInput = SectionsPerComponent::I1; // Sections / component

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	ComponentsOnOneAxis ComponentsOnOneAxisInput = ComponentsOnOneAxis::I16;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	QuadsPerSectionOnOneAxis QuadsPerSectionOnOneAxisInput = QuadsPerSectionOnOneAxis::I255;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterialInstance* GroundMaterial;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	GenerateNewLandscape GenerateNewLandscape = GenerateNewLandscape::YES;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	float SpawnThreshhold = 0.0001;

	uint32_t TotalComponents;

	uint32_t QuadsPerSection;

	uint32_t QuadsPerComponent;

	uint32_t ComponentSize;

	uint32_t TotalSizeOnOneAxis;

	uint32_t SectionsPerComponent;

	uint32_t ComponentsOnOneAxis;

	uint32_t QuadsPerSectionOnOneAxis;

	ALandscape* Landscape;

	TMap<FGuid, TArray<uint16>> HeightDataPerLayers;

	TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;

	UWorld* World;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void CalculateParameters();

	virtual void GenerateLandscape(FTransform LandscapeTransform);

private: 
	virtual void GenerateHeightmap();

	virtual void GenerateMaterialImportLayers();

	virtual void InitializeWorldContext();
		
};
