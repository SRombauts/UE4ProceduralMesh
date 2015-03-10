// UE4 Procedural Mesh Generation from the Epic Wiki (https://wiki.unrealengine.com/Procedural_Mesh_Generation)

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralLatheActor.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALMESH_API AProceduralLatheActor : public AActor
{
	GENERATED_BODY()

public:
	AProceduralLatheActor();

	// Allow viewing/changing the Material ot the procedural Mesh in editor (if placed in a level at construction)
	UPROPERTY(VisibleAnywhere, Category=Materials)
	UProceduralMeshComponent* mesh;

	void GenerateLathe(const TArray<FVector>& InPoints, const int InSegments, TArray<FProceduralMeshTriangle>& OutTriangles);
};
