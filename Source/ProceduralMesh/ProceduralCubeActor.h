// UE4 Procedural Mesh Generation from the Epic Wiki (https://wiki.unrealengine.com/Procedural_Mesh_Generation)

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralCubeActor.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALMESH_API AProceduralCubeActor : public AActor
{
	GENERATED_BODY()

public:
	AProceduralCubeActor();

	// Allow viewing/changing the Material ot the procedural Mesh in editor (if placed in a level at construction)
	UPROPERTY(VisibleAnywhere, Category=Materials)
	UProceduralMeshComponent* mesh;

	void GenerateCube(const float& InSize, TArray<FProceduralMeshTriangle>& OutTriangles);
};
