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

	//Transform the data based on the creator.
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
				}
			}

			for (Bone &bone : _Skeleton._Bones)
			{
				BlenderUtilities::Transform(&bone._BindTransform);
				bone._InverseBindTransform = bone._BindTransform;
				bone._InverseBindTransform.Inverse();
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	/*
	*	Animated vertices with fewer bone influences will have a bone index of -1, which becomes UINT32_MAXIMUM when converted to unsigned.
	*	Fix those up by setting both the index and the weight to zero.
	*/
	for (Mesh &mesh : _Meshes)
	{
		for (AnimatedVertex &vertex : mesh._Vertices)
		{
			for (uint8 bone_index{ 0 }; bone_index < 4; ++bone_index)
			{
				if (vertex._BoneIndices[bone_index] == UINT32_MAXIMUM)
				{
					vertex._BoneIndices[bone_index] = 0;
					vertex._BoneWeights[bone_index] = 0.0f;
				}
			}
		}
	}
}