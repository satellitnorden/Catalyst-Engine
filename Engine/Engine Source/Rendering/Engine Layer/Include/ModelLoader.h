#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <GameMath.h>

//Rendering.
#include <PhysicalVertex.h>

namespace ModelLoader
{

	/*
	*	Given a path, loads a model and outputs the vertices and indices in the supplied containers.
	*/
	static void LoadModel(const char *RESTRICT modelPath, DynamicArray<PhysicalVertex> &vertices, DynamicArray<uint32> &indices, float &extent) NOEXCEPT
	{
		/*
		vertices.Reserve(1'000'000);
		indices.Reserve(1'000'000);

		Assimp::Importer modelImporter;
		const aiScene *modelScene = modelImporter.ReadFile(modelPath, aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

		ProcessNode(modelScene->mRootNode, modelScene, vertices, indices, extent);
		*/
	}

}