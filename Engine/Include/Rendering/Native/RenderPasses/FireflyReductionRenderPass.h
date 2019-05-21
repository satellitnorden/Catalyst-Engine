#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/FireflyReductionGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class FireflyReductionRenderPass final : public RenderPass
{
	
public:

	//Singleton declaration.
	DECLARE_SINGLETON(FireflyReductionRenderPass);

	/*
	*	Default constructor.
	*/
	FireflyReductionRenderPass() NOEXCEPT;

private:

	//The firefly reduction graphics pipelines.
	StaticArray<FireflyReductionGraphicsPipeline, 2> _FireflyReductionGraphicsPipelines;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

};