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
	GENERATED_UCLASS_BODY()

public:
	// Allow viewing/changing the Material ot the procedural Mesh in editor (if placed in a level at construction)
	UPROPERTY(VisibleAnywhere, Category=Materials)
	TSubobjectPtr<UProceduralMeshComponent> mesh;

	void Lathe(const TArray<FVector>& points, TArray<FProceduralMeshTriangle>& triangles, int segments);
};
