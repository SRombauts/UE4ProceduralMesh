UE4 Procedural Mesh
===================

UE4 Procedural MeshGeneration from the Epic Wiki, using C++ code and Blueprints
 
 - https://wiki.unrealengine.com/Procedural_Mesh_Generation
 
 This is a very simple demonstration on how to generate procedural meshes and spawn them in game.
 
 It is not to be taken as an example of proper programming technique,
 but only as an indication to help you generate your own meshes.

## C++ Code

- UGeneratedMeshComponent, using FGeneratedMeshTriangle
- AGameGeneratedActor spwaning an example mesh

## Level Blueprint

Spawnin is done on the Level Blueprint, at the BeginPlay Event :

 ![Level Blueprint spawning a GameGeneratedActor](https://raw.githubusercontent.com/SRombauts/UE4ProceduralMesh/master/LevelBlueprint.png "Level Blueprint spawning a GameGeneratedActor")

## Result

 ![GameGeneratedActor](https://raw.githubusercontent.com/SRombauts/UE4ProceduralMesh/master/ProceduralMesh.png "GameGeneratedActor")
