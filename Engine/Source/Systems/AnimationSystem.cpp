//Header file.
#include <Systems/AnimationSystem.h>

//Core.
#include <Core/Containers/StaticArray.h>

//Animation.
#include <Animation/AnimationCore.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Systems.
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
void AnimationSystem::RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update all animated models.
	const uint64 number_of_animated_model_components{ ComponentManager::GetNumberOfAnimatedModelComponents() };
	AnimatedModelComponent *RESTRICT component{ ComponentManager::GetAnimatedModelAnimatedModelComponents() };

	for (uint64 i{ 0 }; i < number_of_animated_model_components; ++i, ++component)
	{
		UpdateAnimatedModel(context, component);
	}
}

/*
*	Updates an animated model.
*/
void AnimationSystem::UpdateAnimatedModel(const UpdateContext *const RESTRICT context, AnimatedModelComponent *const RESTRICT component) NOEXCEPT
{
	StaticArray<Matrix4x4, AnimationConstants::MAXIMUM_BONE_TRANSFORMS> bone_transforms;

	if (component->_CurrentAnimationResource)
	{
		//Update the current animation time.
		component->_CurrentAnimationTime += context->_DeltaTime;

		//Wrap around if the animation is looping.
		if (true) //IsLooping() or whater...
		{
			while (component->_CurrentAnimationTime >= component->_CurrentAnimationResource->_Duration)
			{
				component->_CurrentAnimationTime -= component->_CurrentAnimationResource->_Duration;
			}
		}

		//For every keyframe channel, calculate it's bone transform.
		for (const Pair<HashString, DynamicArray<AnimationKeyframe>>& keyframes : component->_CurrentAnimationResource->_Keyframes)
		{
			for (const AnimationKeyframe& keyframe : keyframes._Second)
			{
				if (keyframe._Timestamp >= component->_CurrentAnimationTime)
				{
					//Find the bone index.
					uint32 bone_index{ 0 };

					if (FindBoneIndex(&component->_AnimatedModelResource->_Skeleton._RootBone, keyframes._First, &bone_index))
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

	RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &component->_AnimationDataBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()]);
}

/*
*	Finds the bone index of the animated model with the given name.
*/
bool AnimationSystem::FindBoneIndex(const Bone* const RESTRICT bone, const HashString name, uint32* const RESTRICT bone_index) NOEXCEPT
{
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
}