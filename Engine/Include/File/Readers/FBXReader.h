#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File.
#include <File/Types/ModelFile.h>

//Third party.
#include <ThirdParty/assimp/Importer.hpp>
#include <ThirdParty/assimp/scene.h>
#include <ThirdParty/assimp/postprocess.h>

class FBXReader final
{

public:

	/*
	*	Reads the model at the given file path. Returns if the read was succesful.
	*/
	FORCE_INLINE static NO_DISCARD bool Read(const char* const RESTRICT file_path, ModelFile *const RESTRICT out_model) NOEXCEPT
	{
		//Initialize the Assimp scene.
		Assimp::Importer importer;
		const aiScene *const RESTRICT scene{ importer.ReadFile(	file_path,	
																aiPostProcessSteps::aiProcess_CalcTangentSpace
																| aiPostProcessSteps::aiProcess_JoinIdenticalVertices
																| aiPostProcessSteps::aiProcess_Triangulate
																| aiPostProcessSteps::aiProcess_GenNormals
																| aiPostProcessSteps::aiProcess_ImproveCacheLocality
																| aiPostProcessSteps::aiProcess_FlipUVs) };

		if (!scene)
		{
			ASSERT(false, "Couldn't read model!");

			return false;
		}

		//Process the node(s).
		ProcessNode(scene->mRootNode, scene, out_model);

		return true;
	}

private:

	/*
	*	Processes a single node.
	*/
	FORCE_INLINE static void ProcessNode(aiNode *RESTRICT node, const aiScene *RESTRICT scene, ModelFile *const RESTRICT out_model) NOEXCEPT
	{
		//Process all meshes.
		for (uint32 i{ 0 }; i < node->mNumMeshes; ++i)
		{
			//Add a new mesh.
			out_model->_Meshes.Emplace();

			ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, &out_model->_Meshes.Back());
		}

		//Process all nodes.
		for (uint32 i{ 0 }; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene, out_model);
		}
	}

	/*
	*	Processes a single mesh.
	*/
	FORCE_INLINE static void ProcessMesh(aiMesh *RESTRICT mesh, const aiScene *RESTRICT scene, ModelFile::Mesh *const RESTRICT out_mesh) NOEXCEPT
	{
		//Process the vertices.
		for (uint32 i{ 0 }; i < mesh->mNumVertices; ++i)
		{
			out_mesh->_Vertices.Emplace(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Position._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Position._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Position._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Normal._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Normal._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Normal._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Tangent._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Tangent._Y), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._Tangent._Z), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._TextureCoordinate._X), "NaN detected!");
			ASSERT(!CatalystBaseMath::IsNaN(out_mesh->_Vertices.Back()._TextureCoordinate._Y), "NaN detected!");
		}

		//Process the indices.
		for (uint32 i{ 0 }; i < mesh->mNumFaces; ++i)
		{
			aiFace &face = mesh->mFaces[i];

			for (uint32 j{ 0 }; j < face.mNumIndices; ++j)
			{
				out_mesh->_Indices.Emplace(face.mIndices[j]);
			}
		}
	}

};
#endif