// UE4 Procedural Mesh Generation from the Epic Wiki (https://wiki.unrealengine.com/Procedural_Mesh_Generation)

#include "ProceduralMesh.h"
#include "DynamicMeshBuilder.h"
#include "GeneratedMeshComponent.h"
#include "Runtime/Launch/Resources/Version.h"

/** Vertex Buffer */
class FGeneratedMeshVertexBuffer : public FVertexBuffer
{
public:
	TArray<FDynamicMeshVertex> Vertices;

	virtual void InitRHI()
	{
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 3
		FRHIResourceCreateInfo CreateInfo;
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.Num() * sizeof(FDynamicMeshVertex), BUF_Static, CreateInfo);
#else
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.Num() * sizeof(FDynamicMeshVertex), NULL, BUF_Static);
#endif
		// Copy the vertex data into the vertex buffer.
		void* VertexBufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Vertices.Num() * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, Vertices.GetTypedData(), Vertices.Num() * sizeof(FDynamicMeshVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}
};

/** Index Buffer */
class FGeneratedMeshIndexBuffer : public FIndexBuffer
{
public:
	TArray<int32> Indices;

	virtual void InitRHI()
	{
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 3
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), BUF_Static, CreateInfo);
#else
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), NULL, BUF_Static);
#endif
		// Write the indices to the index buffer.
		void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
		FMemory::Memcpy(Buffer, Indices.GetTypedData(), Indices.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

/** Vertex Factory */
class FGeneratedMeshVertexFactory : public FLocalVertexFactory
{
public:
	FGeneratedMeshVertexFactory()
	{
	}

	/** Initialization */
	void Init(const FGeneratedMeshVertexBuffer* VertexBuffer)
	{
		// Commented out to enable building light of a level (but no backing is done for the procedural mesh itself)
        //check(!IsInRenderingThread());

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitGeneratedMeshVertexFactory,
			FGeneratedMeshVertexFactory*, VertexFactory, this,
			const FGeneratedMeshVertexBuffer*, VertexBuffer, VertexBuffer,
			{
			    // Initialize the vertex factory's stream components.
			    DataType NewData;
			    NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
			    NewData.TextureCoordinates.Add(
				    FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex, TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
				    );
			    NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
			    NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
			    NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
			    VertexFactory->SetData(NewData);
			});
	}
};



//////////////////////////////////////////////////////////////////////////

UGeneratedMeshComponent::UGeneratedMeshComponent(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool UGeneratedMeshComponent::SetGeneratedMeshTriangles(const TArray<FGeneratedMeshTriangle>& Triangles)
{
	GeneratedMeshTris = Triangles;

	UpdateCollision();

	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();

	return true;
}


FPrimitiveSceneProxy* UGeneratedMeshComponent::CreateSceneProxy()
{

	/** Scene proxy defined only inside the scope of this one function */
	class FGeneratedMeshSceneProxy : public FPrimitiveSceneProxy
	{
	public:
		FGeneratedMeshSceneProxy(UGeneratedMeshComponent* Component)
			: FPrimitiveSceneProxy(Component)
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 5
			, MaterialRelevance(Component->GetMaterialRelevance(GetScene()->GetFeatureLevel()))
#else
			, MaterialRelevance(Component->GetMaterialRelevance())
#endif
		{
			const FColor VertexColor(255, 255, 255);

			// Add each triangle to the vertex/index buffer
			for(int TriIdx = 0; TriIdx<Component->GeneratedMeshTris.Num(); TriIdx++)
			{
				FGeneratedMeshTriangle& Tri = Component->GeneratedMeshTris[TriIdx];

				const FVector Edge01 = (Tri.Vertex1 - Tri.Vertex0);
				const FVector Edge02 = (Tri.Vertex2 - Tri.Vertex0);

				const FVector TangentX = Edge01.SafeNormal();
				const FVector TangentZ = (Edge02 ^ Edge01).SafeNormal();
				const FVector TangentY = (TangentX ^ TangentZ).SafeNormal();

				FDynamicMeshVertex Vert0;
				Vert0.Position = Tri.Vertex0;
				Vert0.Color = VertexColor;
				Vert0.SetTangents(TangentX, TangentY, TangentZ);
				int32 VIndex = VertexBuffer.Vertices.Add(Vert0);
				IndexBuffer.Indices.Add(VIndex);

				FDynamicMeshVertex Vert1;
				Vert1.Position = Tri.Vertex1;
				Vert1.Color = VertexColor;
				Vert1.SetTangents(TangentX, TangentY, TangentZ);
				VIndex = VertexBuffer.Vertices.Add(Vert1);
				IndexBuffer.Indices.Add(VIndex);

				FDynamicMeshVertex Vert2;
				Vert2.Position = Tri.Vertex2;
				Vert2.Color = VertexColor;
				Vert2.SetTangents(TangentX, TangentY, TangentZ);
				VIndex = VertexBuffer.Vertices.Add(Vert2);
				IndexBuffer.Indices.Add(VIndex);
			}

			// Init vertex factory
			VertexFactory.Init(&VertexBuffer);

			// Enqueue initialization of render resource
			BeginInitResource(&VertexBuffer);
			BeginInitResource(&IndexBuffer);
			BeginInitResource(&VertexFactory);

			// Grab material
			Material = Component->GetMaterial(0);
			if(Material == NULL)
			{
				Material = UMaterial::GetDefaultMaterial(MD_Surface);
			}
		}

		virtual ~FGeneratedMeshSceneProxy()
		{
			VertexBuffer.ReleaseResource();
			IndexBuffer.ReleaseResource();
			VertexFactory.ReleaseResource();
		}

		virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View)
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_GeneratedMeshSceneProxy_DrawDynamicElements);

			const bool bWireframe = View->Family->EngineShowFlags.Wireframe;

			auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
				GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
				FLinearColor(0, 0.5f, 1.f)
				);

			FMaterialRenderProxy* MaterialProxy = NULL;
			if(bWireframe)
			{
				MaterialProxy = &WireframeMaterialInstance;
			}
			else
			{
				MaterialProxy = Material->GetRenderProxy(IsSelected());
			}

			// Draw the mesh.
			FMeshBatch Mesh;
			FMeshBatchElement& BatchElement = Mesh.Elements[0];
			BatchElement.IndexBuffer = &IndexBuffer;
			Mesh.bWireframe = bWireframe;
			Mesh.VertexFactory = &VertexFactory;
			Mesh.MaterialRenderProxy = MaterialProxy;
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 5
			BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
#else
			BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true);
#endif
			BatchElement.FirstIndex = 0;
			BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
			BatchElement.MinVertexIndex = 0;
			BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
			Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			Mesh.Type = PT_TriangleList;
			Mesh.DepthPriorityGroup = SDPG_World;
			PDI->DrawMesh(Mesh);
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View)
		{
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = IsShown(View);
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bDynamicRelevance = true;
			MaterialRelevance.SetPrimitiveViewRelevance(Result);
			return Result;
		}

		virtual bool CanBeOccluded() const override
		{
			return !MaterialRelevance.bDisableDepthTest;
		}

			virtual uint32 GetMemoryFootprint(void) const
		{
			return(sizeof(*this) + GetAllocatedSize());
		}

		uint32 GetAllocatedSize(void) const
		{
			return(FPrimitiveSceneProxy::GetAllocatedSize());
		}

	private:

		UMaterialInterface* Material;
		FGeneratedMeshVertexBuffer VertexBuffer;
		FGeneratedMeshIndexBuffer IndexBuffer;
		FGeneratedMeshVertexFactory VertexFactory;

		FMaterialRelevance MaterialRelevance;
	};


	//Only create if have enough tris
	if(GeneratedMeshTris.Num() > 0)
	{
		return new FGeneratedMeshSceneProxy(this);
	}
	else
	{
		return nullptr;
	}
}

int32 UGeneratedMeshComponent::GetNumMaterials() const
{
	return 1;
}


FBoxSphereBounds UGeneratedMeshComponent::CalcBounds(const FTransform & LocalToWorld) const
{
	// Minimum Vector: It's set to the first vertex's position initially (NULL == FVector::ZeroVector might be required and a known vertex vector has intrinsically valid values)
	FVector vecMin = GeneratedMeshTris[0].Vertex0;

	// Maximum Vector: It's set to the first vertex's position initially (NULL == FVector::ZeroVector might be required and a known vertex vector has intrinsically valid values)
	FVector vecMax = GeneratedMeshTris[0].Vertex0;

	// Get maximum and minimum X, Y and Z positions of vectors
	for(int32 TriIdx = 0; TriIdx < GeneratedMeshTris.Num(); TriIdx++)
	{
		vecMin.X = (vecMin.X > GeneratedMeshTris[TriIdx].Vertex0.X) ? GeneratedMeshTris[TriIdx].Vertex0.X : vecMin.X;
		vecMin.X = (vecMin.X > GeneratedMeshTris[TriIdx].Vertex1.X) ? GeneratedMeshTris[TriIdx].Vertex1.X : vecMin.X;
		vecMin.X = (vecMin.X > GeneratedMeshTris[TriIdx].Vertex2.X) ? GeneratedMeshTris[TriIdx].Vertex2.X : vecMin.X;

		vecMin.Y = (vecMin.Y > GeneratedMeshTris[TriIdx].Vertex0.Y) ? GeneratedMeshTris[TriIdx].Vertex0.Y : vecMin.Y;
		vecMin.Y = (vecMin.Y > GeneratedMeshTris[TriIdx].Vertex1.Y) ? GeneratedMeshTris[TriIdx].Vertex1.Y : vecMin.Y;
		vecMin.Y = (vecMin.Y > GeneratedMeshTris[TriIdx].Vertex2.Y) ? GeneratedMeshTris[TriIdx].Vertex2.Y : vecMin.Y;

		vecMin.Z = (vecMin.Z > GeneratedMeshTris[TriIdx].Vertex0.Z) ? GeneratedMeshTris[TriIdx].Vertex0.Z : vecMin.Z;
		vecMin.Z = (vecMin.Z > GeneratedMeshTris[TriIdx].Vertex1.Z) ? GeneratedMeshTris[TriIdx].Vertex1.Z : vecMin.Z;
		vecMin.Z = (vecMin.Z > GeneratedMeshTris[TriIdx].Vertex2.Z) ? GeneratedMeshTris[TriIdx].Vertex2.Z : vecMin.Z;

		vecMax.X = (vecMax.X < GeneratedMeshTris[TriIdx].Vertex0.X) ? GeneratedMeshTris[TriIdx].Vertex0.X : vecMax.X;
		vecMax.X = (vecMax.X < GeneratedMeshTris[TriIdx].Vertex1.X) ? GeneratedMeshTris[TriIdx].Vertex1.X : vecMax.X;
		vecMax.X = (vecMax.X < GeneratedMeshTris[TriIdx].Vertex2.X) ? GeneratedMeshTris[TriIdx].Vertex2.X : vecMax.X;

		vecMax.Y = (vecMax.Y < GeneratedMeshTris[TriIdx].Vertex0.Y) ? GeneratedMeshTris[TriIdx].Vertex0.Y : vecMax.Y;
		vecMax.Y = (vecMax.Y < GeneratedMeshTris[TriIdx].Vertex1.Y) ? GeneratedMeshTris[TriIdx].Vertex1.Y : vecMax.Y;
		vecMax.Y = (vecMax.Y < GeneratedMeshTris[TriIdx].Vertex2.Y) ? GeneratedMeshTris[TriIdx].Vertex2.Y : vecMax.Y;

		vecMax.Z = (vecMax.Z < GeneratedMeshTris[TriIdx].Vertex0.Z) ? GeneratedMeshTris[TriIdx].Vertex0.Z : vecMax.Z;
		vecMax.Z = (vecMax.Z < GeneratedMeshTris[TriIdx].Vertex1.Z) ? GeneratedMeshTris[TriIdx].Vertex1.Z : vecMax.Z;
		vecMax.Z = (vecMax.Z < GeneratedMeshTris[TriIdx].Vertex2.Z) ? GeneratedMeshTris[TriIdx].Vertex2.Z : vecMax.Z;
	}

	FVector vecOrigin = ((vecMax - vecMin) / 2) + vecMin;	/* Origin = ((Max Vertex's Vector - Min Vertex's Vector) / 2 ) + Min Vertex's Vector */
	FVector BoxPoint = vecMax - vecMin;			/* The difference between the "Maximum Vertex" and the "Minimum Vertex" is our actual Bounds Box */
	return FBoxSphereBounds(vecOrigin, BoxPoint, BoxPoint.Size()).TransformBy(LocalToWorld);
}


bool UGeneratedMeshComponent::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	FTriIndices Triangle;

	for(int32 i = 0; i<GeneratedMeshTris.Num(); i++)
	{
		const FGeneratedMeshTriangle& tri = GeneratedMeshTris[i];

		Triangle.v0 = CollisionData->Vertices.Add(tri.Vertex0);
		Triangle.v1 = CollisionData->Vertices.Add(tri.Vertex1);
		Triangle.v2 = CollisionData->Vertices.Add(tri.Vertex2);

		CollisionData->Indices.Add(Triangle);
		CollisionData->MaterialIndices.Add(i);
	}

	CollisionData->bFlipNormals = true;

	return true;
}

bool UGeneratedMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	return (GeneratedMeshTris.Num() > 0);
}

void UGeneratedMeshComponent::UpdateBodySetup()
{
	if(ModelBodySetup == NULL)
	{
		ModelBodySetup = ConstructObject<UBodySetup>(UBodySetup::StaticClass(), this);
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = true;
	}
}

void UGeneratedMeshComponent::UpdateCollision()
{
	if(bPhysicsStateCreated)
	{
		DestroyPhysicsState();
		UpdateBodySetup();
		CreatePhysicsState();

		// Works in Packaged build only since UE4.5:
		ModelBodySetup->InvalidatePhysicsData();
		ModelBodySetup->CreatePhysicsMeshes();
	}
}

UBodySetup* UGeneratedMeshComponent::GetBodySetup()
{
	UpdateBodySetup();
	return ModelBodySetup;
}