// UE4 Procedural Mesh Generation from the Epic Wiki (https://wiki.unrealengine.com/Procedural_Mesh_Generation)
//

#include "ProceduralMesh.h"
#include "ProceduralTriangleActor.h"

AProceduralTriangleActor::AProceduralTriangleActor()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralTriangle"));

	// Apply a simple material directly using the VertexColor as its BaseColor input
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("Material'/Game/Materials/BaseColor.BaseColor'"));
	// TODO Apply a real material with textures, using UVs
//	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("Material'/Game/Materials/M_Concrete_Poured.M_Concrete_Poured'"));
	mesh->SetMaterial(0, Material.Object);

	// Generate a single triangle
	TArray<FProceduralMeshTriangle> triangles;
	GenerateTriangle(triangles);
	mesh->SetProceduralMeshTriangles(triangles);

	RootComponent = mesh;
}

// Generate a single horizontal triangle counterclockwise to point up (one face, visible only from the top, not from the bottom)
void AProceduralTriangleActor::GenerateTriangle(TArray<FProceduralMeshTriangle>& OutTriangles)
{
	FProceduralMeshTriangle triangle;
	triangle.Vertex0.Position.Set(  0.f, -80.f, 0.f);
	triangle.Vertex1.Position.Set(  0.f,  80.f, 0.f);
	triangle.Vertex2.Position.Set(100.f,  0.f,  0.f);
	static const FColor Blue(51, 51, 255);
	triangle.Vertex0.Color = Blue;
	triangle.Vertex1.Color = Blue;
	triangle.Vertex2.Color = Blue;
	triangle.Vertex0.U = 0.0f;
	triangle.Vertex0.V = 0.0f;
	triangle.Vertex1.U = 1.0f;
	triangle.Vertex1.V = 0.0f;
	triangle.Vertex2.U = 0.5f;
	triangle.Vertex2.V = 0.75f;
	OutTriangles.Add(triangle);
}
