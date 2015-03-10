// UE4 Procedural Mesh Generation from the Epic Wiki (https://wiki.unrealengine.com/Procedural_Mesh_Generation)
//
// A lathed object is a 3D model whose vertex geometry is produced by rotating a set of points around a fixed axis.
// (from Wikipedia http://en.wikipedia.org/wiki/Lathe_(graphics))

#include "ProceduralMesh.h"
#include "ProceduralLatheActor.h"

AProceduralLatheActor::AProceduralLatheActor()
{
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralLathe"));

	// Apply a material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/Materials/BaseColor.BaseColor"));
	mesh->SetMaterial(0, Material.Object);

	// Contains the points describing the polyline we are going to rotate
	TArray<FVector> points;

	points.Add(FVector(190, 50, 0));
	points.Add(FVector(140, 60, 0));
	points.Add(FVector(110, 70, 0));
	points.Add(FVector(100, 80, 0));
	points.Add(FVector(70, 70, 0));
	points.Add(FVector(50, 60, 0));
	points.Add(FVector(30, 50, 0));
	points.Add(FVector(20, 40, 0));
	points.Add(FVector(10, 30, 0));
	points.Add(FVector( 0, 40, 0));

	// Generate a Lathe from rotating the given points
	TArray<FProceduralMeshTriangle> triangles;
	GenerateLathe(points, 128, triangles);
	mesh->SetProceduralMeshTriangles(triangles);

	RootComponent = mesh;
}

// Generate a lathe by rotating the given polyline
void AProceduralLatheActor::GenerateLathe(const TArray<FVector>& InPoints, const int InSegments, TArray<FProceduralMeshTriangle>& OutTriangles)
{
	UE_LOG(LogClass, Log, TEXT("AProceduralLatheActor::Lathe POINTS %d"), InPoints.Num());

	TArray<FVector> verts;

	// precompute some trig
	float angle = FMath::DegreesToRadians(360.0f / InSegments);
	float sinA = FMath::Sin(angle);
	float cosA = FMath::Cos(angle);

	/*
	This implementation is rotation around the X Axis, other formulas below

	Z Axis Rotation
	x' = x*cos q - y*sin q
	y' = x*sin q + y*cos q
	z' = z

	X Axis Rotation
	y' = y*cos q - z*sin q
	z' = y*sin q + z*cos q
	x' = x

	Y Axis Rotation
	z' = z*cos q - x*sin q
	x' = z*sin q + x*cos q
	y' = y
	*/

	// Working point array, in which we keep the rotated line we draw with
	TArray<FVector> wp;
	for(int i = 0; i < InPoints.Num(); i++)
	{
		wp.Add(InPoints[i]);
	}

	// Add a first and last point on the axis to complete the OutTriangles
	FVector p0(wp[0].X, 0, 0);
	FVector pLast(wp[wp.Num() - 1].X, 0, 0);

	FProceduralMeshTriangle tri;
	// for each segment draw the OutTriangles clockwise for normals pointing out or counterclockwise for the opposite (this here does CW)
	for(int segment = 0; segment<InSegments; segment++)
	{

		for(int i = 0; i<InPoints.Num() - 1; i++)
		{
			FVector p1 = wp[i];
			FVector p2 = wp[i + 1];
			FVector p1r(p1.X, p1.Y*cosA - p1.Z*sinA, p1.Y*sinA + p1.Z*cosA);
			FVector p2r(p2.X, p2.Y*cosA - p2.Z*sinA, p2.Y*sinA + p2.Z*cosA);

			static const FColor Red(255, 51, 51);
			tri.Vertex0.Color = Red;
			tri.Vertex1.Color = Red;
			tri.Vertex2.Color = Red;

			if(i == 0)
			{
				tri.Vertex0.Position = p1;
				tri.Vertex1.Position = p0;
				tri.Vertex2.Position = p1r;
				OutTriangles.Add(tri);
			}

			tri.Vertex0.Position = p1;
			tri.Vertex1.Position = p1r;
			tri.Vertex2.Position = p2;
			OutTriangles.Add(tri);

			tri.Vertex0.Position = p2;
			tri.Vertex1.Position = p1r;
			tri.Vertex2.Position = p2r;
			OutTriangles.Add(tri);

			if(i == InPoints.Num() - 2)
			{
				tri.Vertex0.Position = p2;
				tri.Vertex1.Position = p2r;
				tri.Vertex2.Position = pLast;
				OutTriangles.Add(tri);
				wp[i + 1] = p2r;
			}

			wp[i] = p1r;
		}
	}
}