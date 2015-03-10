// UE4 Procedural Mesh Generation from the Epic Wiki (https://wiki.unrealengine.com/Procedural_Mesh_Generation)
//

#include "ProceduralMesh.h"
#include "ProceduralCubeActor.h"

AProceduralCubeActor::AProceduralCubeActor()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralCube"));

	// Apply a simple material directly using the VertexColor as its BaseColor input
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("Material'/Game/Materials/BaseColor.BaseColor'"));
	// TODO Apply a real material with textures, using UVs
//	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("Material'/Game/Materials/M_Concrete_Poured.M_Concrete_Poured'"));
	mesh->SetMaterial(0, Material.Object);

	// Generate a cube
	TArray<FProceduralMeshTriangle> triangles;
	GenerateCube(100.f, triangles);
	mesh->SetProceduralMeshTriangles(triangles);

	RootComponent = mesh;
}

// Generate a full cube
void AProceduralCubeActor::GenerateCube(const float& InSize, TArray<FProceduralMeshTriangle>& OutTriangles)
{
	// The 8 vertices of the cube
	FVector p0 = FVector(	0.f,	0.f,	0.f);
	FVector p1 = FVector(	0.f,	0.f, InSize);
	FVector p2 = FVector(InSize,	0.f, InSize);
	FVector p3 = FVector(InSize,	0.f,	0.f);
	FVector p4 = FVector(InSize, InSize,	0.f);
	FVector p5 = FVector(InSize, InSize, InSize);
	FVector p6 = FVector(	0.f, InSize, InSize);
	FVector p7 = FVector(	0.f, InSize,	0.f);

	FProceduralMeshVertex v0;
	FProceduralMeshVertex v1;
	FProceduralMeshVertex v2;
	FProceduralMeshVertex v3;
	v0.U = 0.f; v0.V = 0.f;
	v1.U = 0.f; v1.V = 1.f;
	v2.U = 1.f; v2.V = 1.f;
	v3.U = 1.f; v3.V = 0.f;

	FProceduralMeshTriangle t1;
	FProceduralMeshTriangle t2;

	// front face
	v0.Position = p0;
	v1.Position = p1;
	v2.Position = p2;
	v3.Position = p3;
	t1.Vertex0 = v0;
	t1.Vertex1 = v1;
	t1.Vertex2 = v2;
	t2.Vertex0 = v0;
	t2.Vertex1 = v2;
	t2.Vertex2 = v3;
	OutTriangles.Add(t1);
	OutTriangles.Add(t2);

	// back face
	v0.Position = p4;
	v1.Position = p5;
	v2.Position = p6;
	v3.Position = p7;
	t1.Vertex0 = v0;
	t1.Vertex1 = v1;
	t1.Vertex2 = v2;
	t2.Vertex0 = v0;
	t2.Vertex1 = v2;
	t2.Vertex2 = v3;
	OutTriangles.Add(t1);
	OutTriangles.Add(t2);

	// left face
	v0.Position = p7;
	v1.Position = p6;
	v2.Position = p1;
	v3.Position = p0;
	t1.Vertex0 = v0;
	t1.Vertex1 = v1;
	t1.Vertex2 = v2;
	t2.Vertex0 = v0;
	t2.Vertex1 = v2;
	t2.Vertex2 = v3;
	OutTriangles.Add(t1);
	OutTriangles.Add(t2);

	// right face
	v0.Position = p3;
	v1.Position = p2;
	v2.Position = p5;
	v3.Position = p4;
	t1.Vertex0 = v0;
	t1.Vertex1 = v1;
	t1.Vertex2 = v2;
	t2.Vertex0 = v0;
	t2.Vertex1 = v2;
	t2.Vertex2 = v3;
	OutTriangles.Add(t1);
	OutTriangles.Add(t2);

	// top face
	v0.Position = p1;
	v1.Position = p6;
	v2.Position = p5;
	v3.Position = p2;
	t1.Vertex0 = v0;
	t1.Vertex1 = v1;
	t1.Vertex2 = v2;
	t2.Vertex0 = v0;
	t2.Vertex1 = v2;
	t2.Vertex2 = v3;
	OutTriangles.Add(t1);
	OutTriangles.Add(t2);

	// bottom face
	v0.Position = p3;
	v1.Position = p4;
	v2.Position = p7;
	v3.Position = p0;
	t1.Vertex0 = v0;
	t1.Vertex1 = v1;
	t1.Vertex2 = v2;
	t2.Vertex0 = v0;
	t2.Vertex1 = v2;
	t2.Vertex2 = v3;
	OutTriangles.Add(t1);
	OutTriangles.Add(t2);
}