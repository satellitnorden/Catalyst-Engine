//Header file.
#include <File/Types/ModelFile.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

/*
*	Post processes this model file.
*/
void ModelFile::PostProcess() NOEXCEPT
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

			if (Vector3<float32>::DotProduct(mesh._Vertices[vertex_index]._Normal, mesh._Vertices[vertex_index]._Tangent) == -1.0f)
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

	//Calculate the tangents.
	for (ModelFile::Mesh& mesh : _Meshes)
	{
		for (uint64 index_index{ 0 }; index_index < mesh._Indices.Size(); index_index += 3)
		{
			if (mesh._Vertices[mesh._Indices[index_index + 0]]._Tangent.IsZero()
				|| mesh._Vertices[mesh._Indices[index_index + 1]]._Tangent.IsZero()
				|| mesh._Vertices[mesh._Indices[index_index + 2]]._Tangent.IsZero())
			{
				StaticArray<Vector3<float32>, 3> positions;

				positions[0] = mesh._Vertices[mesh._Indices[index_index + 0]]._Position;
				positions[1] = mesh._Vertices[mesh._Indices[index_index + 1]]._Position;
				positions[2] = mesh._Vertices[mesh._Indices[index_index + 2]]._Position;

				StaticArray<Vector2<float32>, 3> texture_coordinates;

				texture_coordinates[0] = mesh._Vertices[mesh._Indices[index_index + 0]]._TextureCoordinate;
				texture_coordinates[1] = mesh._Vertices[mesh._Indices[index_index + 1]]._TextureCoordinate;
				texture_coordinates[2] = mesh._Vertices[mesh._Indices[index_index + 2]]._TextureCoordinate;

				const Vector3<float32> tangent{ RenderingUtilities::CalculateTangent(positions, texture_coordinates) };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					if (mesh._Vertices[mesh._Indices[index_index + i]]._Tangent.IsZero())
					{
						mesh._Vertices[mesh._Indices[index_index + i]]._Tangent = tangent;
					}
				}
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
			//Swap the Y and Z axis.
			for (ModelFile::Mesh &mesh : _Meshes)
			{
				for (Vertex &vertex : mesh._Vertices)
				{
					Swap(&vertex._Position._Y, &vertex._Position._Z);
					Swap(&vertex._Normal._Y, &vertex._Normal._Z);
					Swap(&vertex._Tangent._Y, &vertex._Tangent._Z);
				}

				for (uint64 i{ 0 }; i < mesh._Indices.Size(); i += 3)
				{
					Swap(&mesh._Indices[i], &mesh._Indices[i + 1]);
				}
			}

			//Modify the texture coordinates.
			for (ModelFile::Mesh &mesh : _Meshes)
			{
				for (Vertex &vertex : mesh._Vertices)
				{
					vertex._TextureCoordinate._Y = 1.0f - vertex._TextureCoordinate._Y;
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
			for (uint32 sub_index{ master_index + 1 }; sub_index < static_cast<uint32>(mesh._Vertices.Size());)
			{
				if (mesh._Vertices[master_index] == mesh._Vertices[sub_index])
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