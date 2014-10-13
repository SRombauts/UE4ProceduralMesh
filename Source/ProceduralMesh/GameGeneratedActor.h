// UE4 Procedural Mesh Generation from the Epic Wiki (https://wiki.unrealengine.com/Procedural_Mesh_Generation)

#pragma once

#include "GameFramework/Actor.h"
#include "GeneratedMeshComponent.h"
#include "GameGeneratedActor.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALMESH_API AGameGeneratedActor : public AActor
{
	GENERATED_UCLASS_BODY()

	void Lathe(const TArray<FVector>& points, TArray<FGeneratedMeshTriangle>& triangles, int segments = 64);
};
