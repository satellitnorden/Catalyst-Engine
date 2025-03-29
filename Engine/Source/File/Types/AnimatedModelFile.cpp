//Header file.
#include <File/Types/AnimatedModelFile.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

/*
*	Post processes this animated model file.
*/
void AnimatedModelFile::PostProcess() NOEXCEPT
{
	//Sort the meshes based on their name.
	if (_Meshes.Size() > 1)
	{
		SortingAlgorithms::StandardSort<Mesh>
		(
			_Meshes.Begin(),
			_Meshes.End(),
			nullptr,
			[](const void *const RESTRICT user_data, const Mesh *const RESTRICT A, const Mesh *const RESTRICT B)
			{
				return strcmp(A->_Name.Data(), B->_Name.Data()) < 0;
			}
		);
	}

	//Transform the vertices based on the creator.
	switch (_Creator)
	{
		case Creator::UNKNOWN:
		{
			//Do nothing. (:

			break;
		}

		case Creator::BLENDER:
		{
			//Swap the Y and Z axis.
			for (Mesh &mesh : _Meshes)
			{
				for (AnimatedVertex &vertex : mesh._Vertices)
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
			for (Mesh &mesh : _Meshes)
			{
				for (AnimatedVertex &vertex : mesh._Vertices)
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
}