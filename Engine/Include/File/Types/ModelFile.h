#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/Vertex.h>

/*
*	Class representing the raw data read from a model file (for example .fbx files).
*/
class ModelFile final
{

public:

	//Enumeration covering all creators.
	enum class Creator : uint8
	{
		BLENDER,

		UNKNOWN
	};

	/*
	*	Mesh class definition.
	*/
	class Mesh final
	{

	public:

		//The vertices.
		DynamicArray<Vertex> _Vertices;

		//The indices.
		DynamicArray<uint32> _Indices;

	};

	//The creator.
	Creator _Creator{ Creator::UNKNOWN };

	//The meshes.
	DynamicArray<Mesh> _Meshes;

	/*
	*	Returns if this model file is valid.
	*/
	FORCE_INLINE NO_DISCARD bool IsValid() const NOEXCEPT
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
	void PostProcess() NOEXCEPT
	{
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

				if (mesh._Vertices[vertex_index]._Tangent.IsZero())
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (mesh._Vertices[vertex_index]._Normal == mesh._Vertices[vertex_index]._Tangent)
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Position[0])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Position[1])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Position[2]))
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Normal[0])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Normal[1])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Normal[2]))
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Tangent[0])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Tangent[1])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._Tangent[2]))
				{
					invalid_indices.Emplace(vertex_index);

					continue;
				}

				if (CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._TextureCoordinate[0])
					|| CatalystBaseMath::IsNaN(mesh._Vertices[vertex_index]._TextureCoordinate[1]))
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
					mesh._Indices.EraseAt<true>(index_index + 0);
					mesh._Indices.EraseAt<true>(index_index + 1);
					mesh._Indices.EraseAt<true>(index_index + 2);
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
				const Matrix4x4 TRANSFORMATION{ VectorConstants::ZERO, EulerAngles(CatalystBaseMath::DegreesToRadians(90.0f), CatalystBaseMath::DegreesToRadians(180.0f), 0.0f), VectorConstants::ONE };

				for (ModelFile::Mesh &mesh : _Meshes)
				{
					for (Vertex &vertex : mesh._Vertices)
					{
						vertex.Transform(TRANSFORMATION, 0.0f);
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

		//Join identical vertices.
		for (ModelFile::Mesh &mesh : _Meshes)
		{
			for (uint32 master_index{ 0 }; master_index < static_cast<uint32>(mesh._Vertices.Size()); ++master_index)
			{
				//Iterate over all the other vertices see if there's a duplicate.
				for (uint32 sub_index{ 0 }; sub_index < static_cast<uint32>(mesh._Vertices.Size());)
				{
					if (master_index != sub_index
						&& mesh._Vertices[master_index] == mesh._Vertices[sub_index])
					{
						//Find the indices that points to the sub index, and redirect it to the master index.
						for (uint32 &index : mesh._Indices)
						{
							if (index == sub_index)
							{
								index = master_index;
							}
						}

						//Find the indices that points to the last index, and redirect it to the sub index.
						for (uint32 &index : mesh._Indices)
						{
							if (index == mesh._Vertices.LastIndex())
							{
								index = sub_index;
							}
						}

						//Erase the offending index.
						mesh._Vertices.EraseAt<false>(sub_index);
					}

					else
					{
						++sub_index;
					}
				}
			}
		}
	}

};