//Header file.
#include <File/Types/ModelFile.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//File.
#include <File/Utilities/BlenderUtilities.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

/*
*	Returns if this model file is valid.
*/
NO_DISCARD bool ModelFile::IsValid() const NOEXCEPT
{
	if (_Meshes.Empty())
	{
		return false;
	}

	for (const Mesh &mesh : _Meshes)
	{
		if (mesh._Vertices.Empty())
		{
			return false;
		}

		if (mesh._Indices.Empty())
		{
			return false;
		}
	}

	return true;
}

/*
*	Post processes this model file.
*/
void ModelFile::PostProcess() NOEXCEPT
{
	//Sort the meshes based on their name.
	if (_Meshes.Size() > 1)
	{
		SortingAlgorithms::StandardSort<ModelFile::Mesh>
		(
			_Meshes.Begin(),
			_Meshes.End(),
			nullptr,
			[](const void *const RESTRICT user_data, const ModelFile::Mesh *const RESTRICT A, const ModelFile::Mesh *const RESTRICT B)
			{
				return strcmp(A->_Name.Data(), B->_Name.Data()) < 0;
			}
		);
	}

	//Check for NaN's/invalid values.
	for (ModelFile::Mesh &mesh : _Meshes)
	{
		//Go over all vertices, storing the indices of invalid vertices.
		DynamicArray<uint64> invalid_indices;

		for (uint64 vertex_index{ 0 }; vertex_index < mesh._Vertices.Size(); ++vertex_index)
		{
			if (mesh._Vertices[vertex_index]._Normal.IsZero())
			{
				invalid_indices.Emplace(vertex_index);

				continue;
			}

			if (Vector3<float32>::DotProduct(mesh._Vertices[vertex_index]._Normal, mesh._Vertices[vertex_index]._Tangent) == -1.0f)
			{
				invalid_indices.Emplace(vertex_index);

				continue;
			}

			if (BaseMath::IsNaN(mesh._Vertices[vertex_index]._Position[0])
				|| BaseMath::IsNaN(mesh._Vertices[vertex_index]._Position[1])
				|| BaseMath::IsNaN(mesh._Vertices[vertex_index]._Position[2]))
			{
				invalid_indices.Emplace(vertex_index);

				continue;
			}

			if (BaseMath::IsNaN(mesh._Vertices[vertex_index]._Normal[0])
				|| BaseMath::IsNaN(mesh._Vertices[vertex_index]._Normal[1])
				|| BaseMath::IsNaN(mesh._Vertices[vertex_index]._Normal[2]))
			{
				invalid_indices.Emplace(vertex_index);

				continue;
			}

			if (BaseMath::IsNaN(mesh._Vertices[vertex_index]._Tangent[0])
				|| BaseMath::IsNaN(mesh._Vertices[vertex_index]._Tangent[1])
				|| BaseMath::IsNaN(mesh._Vertices[vertex_index]._Tangent[2]))
			{
				invalid_indices.Emplace(vertex_index);

				continue;
			}

			if (BaseMath::IsNaN(mesh._Vertices[vertex_index]._TextureCoordinate[0])
				|| BaseMath::IsNaN(mesh._Vertices[vertex_index]._TextureCoordinate[1]))
			{
				invalid_indices.Emplace(vertex_index);

				continue;
			}
		}

		//Go over the indices, removing invalid triangles.
		for (uint64 index_index{ 0 }; index_index < mesh._Indices.Size();)
		{
			if (invalid_indices.Exists(mesh._Indices[index_index + 0])
				|| invalid_indices.Exists(mesh._Indices[index_index + 1])
				|| invalid_indices.Exists(mesh._Indices[index_index + 2]))
			{
				mesh._Indices.EraseAt<true>(index_index);
				mesh._Indices.EraseAt<true>(index_index);
				mesh._Indices.EraseAt<true>(index_index);
			}

			else
			{
				index_index += 3;
			}
		}
	}

	//Transform the vertices based on the creator.
	switch (_Creator)
	{
		case ModelFile::Creator::UNKNOWN:
		{
			//Do nothing. (:

			break;
		}

		case ModelFile::Creator::BLENDER:
		{
			//Apply Blender transforms.
			for (ModelFile::Mesh &mesh : _Meshes)
			{
				for (Vertex &vertex : mesh._Vertices)
				{
					BlenderUtilities::Transform(&vertex._Position);
					BlenderUtilities::Transform(&vertex._Normal);
					BlenderUtilities::Transform(&vertex._Tangent);
				}
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}