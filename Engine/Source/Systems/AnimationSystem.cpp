//Header file.
#include <Systems/AnimationSystem.h>

//Core.
#include <Core/Containers/StaticArray.h>

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
	
}