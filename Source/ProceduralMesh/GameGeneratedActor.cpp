// UE4 Procedural Mesh Generation from the Epic Wiki (https://wiki.unrealengine.com/Procedural_Mesh_Generation)

#include "ProceduralMesh.h"
#include "GameGeneratedActor.h"

AGameGeneratedActor::AGameGeneratedActor(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	TSubobjectPtr<UGeneratedMeshComponent> mesh = PCIP.CreateDefaultSubobject<UGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));

	TArray<FGeneratedMeshTriangle> triangles;

	// Contains the points describing the polyline we are going to rotate
	TArray<FVector> points;

	points.Add(FVector(200, 50, 0));
	points.Add(FVector(150, 60, 0));
	points.Add(FVector(120, 70, 0));
	points.Add(FVector(110, 80, 0));
	points.Add(FVector(80, 70, 0));
	points.Add(FVector(70, 60, 0));
	points.Add(FVector(40, 50, 0));
	points.Add(FVector(30, 40, 0));
	points.Add(FVector(20, 30, 0));
	points.Add(FVector(10, 40, 0));


	// Generate a lathe
	Lathe(points, triangles, 128);
	// Generate a single triangle
//	Triangle(triangles);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/Materials/BaseColor.BaseColor"));
	mesh->SetMaterial(0, Material.Object);

	mesh->SetGeneratedMeshTriangles(triangles);

	RootComponent = mesh;
}


// Generate a single horizontal triangle counterclockwise to point up (invisible from the bottom)
void AGameGeneratedActor::Triangle(TArray<FGeneratedMeshTriangle>& triangles)
{
	FGeneratedMeshTriangle triangle;
	triangle.Vertex0.Position.Set(100.f, -80.f, -60.f);
	triangle.Vertex1.Position.Set(100.f, 80.f, -30.f);
	triangle.Vertex2.Position.Set(200.f, 0.f, 0.f);
	static const FColor Blue(51, 51, 255);
	triangle.Vertex0.Color = Blue;
	triangle.Vertex2.Color = Blue;
	triangle.Vertex2.Color = Blue;
	triangles.Add(triangle);
}

// Generate a lathe by rotating the given polyline
void AGameGeneratedActor::Lathe(const TArray<FVector>& points, TArray<FGeneratedMeshTriangle>& triangles, int segments)
{
	UE_LOG(LogClass, Log, TEXT("AGameGeneratedActor::Lathe POINTS %d"), points.Num());

	TArray<FVector> verts;

	// precompute some trig
	float angle = FMath::DegreesToRadians(360.0f / segments);
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
	for(int i = 0; i < points.Num(); i++)
	{
		wp.Add(points[i]);
	}

	// Add a first and last point on the axis to complete the triangles
	FVector p0(wp[0].X, 0, 0);
	FVector pLast(wp[wp.Num() - 1].X, 0, 0);

	FGeneratedMeshTriangle tri;
	// for each segment draw the triangles clockwise for normals pointing out or counterclockwise for the opposite (this here does CW)
	for(int segment = 0; segment<segments; segment++)
	{

		for(int i = 0; i<points.Num() - 1; i++)
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
				triangles.Add(tri);
			}

			tri.Vertex0.Position = p1;
			tri.Vertex1.Position = p1r;
			tri.Vertex2.Position = p2;
			triangles.Add(tri);

			tri.Vertex0.Position = p2;
			tri.Vertex1.Position = p1r;
			tri.Vertex2.Position = p2r;
			triangles.Add(tri);

			if(i == points.Num() - 2)
			{
				tri.Vertex0.Position = p2;
				tri.Vertex1.Position = p2r;
				tri.Vertex2.Position = pLast;
				triangles.Add(tri);
				wp[i + 1] = p2r;
			}

			wp[i] = p1r;
		}
	}
}