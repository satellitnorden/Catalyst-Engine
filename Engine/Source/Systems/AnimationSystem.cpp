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
		RenderDataTableLayoutBinding(0, RenderDataTableLayoutBinding::Type::UniformBuffer, 1, ShaderStage::Vertex),
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
	StaticArray<Matrix4, AnimationConstants::MAXIMUM_BONE_TRANSFORMS> bone_transforms;

	if (component->_CurrentAnimation)
	{
		//Animate...
	}

	const void *const RESTRICT dataChunks[]{ bone_transforms.Data() };
	const uint64 dataSizes[]{ sizeof(Matrix4) * AnimationConstants::MAXIMUM_BONE_TRANSFORMS };

	RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &component->_AnimationDataBuffers[RenderingSystem::Instance->GetCurrentFramebufferIndex()]);
}