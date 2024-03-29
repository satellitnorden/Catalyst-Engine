//Header file.
#include <Systems/AnimationSystem.h>

//Core.
#include <Core/Containers/StaticArray.h>

//Animation.
#include <Animation/AnimationCore.h>

//Components.
#include <Components/Components/AnimatedModelComponent.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(AnimationSystem);

/*
*	Post initializes the animation system.
*/
void AnimationSystem::PostInitialize() NOEXCEPT
{
	//Create the animation data render data table layout.
	StaticArray<RenderDataTableLayoutBinding, 1> bindings
	{
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::VERTEX),
	};

	RenderingSystem::Instance->CreateRenderDataTableLayout(bindings.Data(), static_cast<uint32>(bindings.Size()), &_AnimationDataRenderDataTableLayout);
}

/*
*	Updates the animation system during the render update phase.
*/
void AnimationSystem::RenderUpdate() NOEXCEPT
{
	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Update all animated models.
	for (AnimatedModelInstanceData &instance_data : AnimatedModelComponent::Instance->InstanceData())
	{
		UpdateAnimatedModel(delta_time, instance_data);
	}
}

/*
*	Updates an animated model.
*/
void AnimationSystem::UpdateAnimatedModel(const float32 delta_time, AnimatedModelInstanceData &instance_data) NOEXCEPT
{
	/*
	StaticArray<Matrix4x4, AnimationConstants::MAXIMUM_BONE_TRANSFORMS> bone_transforms;

	if (instance_data._CurrentAnimationResource)
	{
		//Update the current animation time.
		instance_data._CurrentAnimationTime += delta_time;

		//Wrap around if the animation is looping.
		if (true) //IsLooping() or whater...
		{
			while (instance_data._CurrentAnimationTime >= instance_data._CurrentAnimationResource->_Duration)
			{
				instance_data._CurrentAnimationTime -= instance_data._CurrentAnimationResource->_Duration;
			}
		}

		//For every keyframe channel, calculate it's bone transform.
		for (const Pair<HashString, DynamicArray<AnimationKeyframe>> &keyframes : instance_data._CurrentAnimationResource->_Keyframes)
		{
			for (const AnimationKeyframe &keyframe : keyframes._Second)
			{
				if (keyframe._Timestamp >= instance_data._CurrentAnimationTime)
				{
					//Find the bone index.
					uint32 bone_index{ 0 };

					if (FindBoneIndex(&instance_data._AnimatedModelResource->_Skeleton._RootBone, keyframes._First, &bone_index))
					{
						bone_transforms[bone_index] = Matrix4x4(keyframe._BoneTransform._Position, keyframe._BoneTransform._Rotation);
					}

					break;
				}
			}
		}
	}

	const void *const RESTRICT dataChunks[]{ bone_transforms.Data() };
	const uint64 dataSizes[]{ sizeof(Matrix4x4) * AnimationConstants::MAXIMUM_BONE_TRANSFORMS };

	RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &instance_data._AnimationDataBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()]);
	*/
}

/*
*	Finds the bone index of the animated model with the given name.
*/
bool AnimationSystem::FindBoneIndex(const Bone* const RESTRICT bone, const HashString name, uint32* const RESTRICT bone_index) NOEXCEPT
{
	/*
	if (bone->_Name == name)
	{
		*bone_index = bone->_Index;

		return true;
	}

	for (const Bone& child : bone->_Children)
	{
		if (FindBoneIndex(&child, name, bone_index))
		{
			return true;
		}
	}

	return false;
	*/
	return false;
}