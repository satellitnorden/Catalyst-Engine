//Header file.
#include <File/Types/AnimationFile.h>

//File.
#include <File/Utilities/BlenderUtilities.h>

/*
*	Post processes this animated model file.
*/
void AnimationFile::PostProcess() NOEXCEPT
{
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
			for (AnimationChannel &channel : _Channels)
			{
				for (AnimationKeyframe &keyframe : channel._Keyframes)
				{
					BlenderUtilities::Transform(&keyframe._BoneTransform._Translation);
					BlenderUtilities::Transform(&keyframe._BoneTransform._Rotation);
					BlenderUtilities::Transform(&keyframe._BoneTransform._Scale);
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