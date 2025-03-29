//Header file.
#include <File/Types/AnimatedModelFile.h>

//Core.
#include <Core/Algorithms/SortingAlgorithms.h>

//File.
#include <File/Utilities/BlenderUtilities.h>

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
			//Apply Blender transforms.
			for (Mesh &mesh : _Meshes)
			{
				for (AnimatedVertex &vertex : mesh._Vertices)
				{
					BlenderUtilities::Transform(&vertex._Position);
					BlenderUtilities::Transform(&vertex._Normal);
					BlenderUtilities::Transform(&vertex._Tangent);
					BlenderUtilities::Transform(&vertex._TextureCoordinate);
				}

				for (uint64 i{ 0 }; i < mesh._Indices.Size(); i += 3)
				{
					Swap(&mesh._Indices[i], &mesh._Indices[i + 1]);
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