#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Vector2.h>

//Rendering.
#include <Rendering/Engine Layer/Render Passes/RenderPass.h>

class BloomVerticalBlurRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(BloomVerticalBlurRenderPass);

	/*
	*	Default constructor.
	*/
	BloomVerticalBlurRenderPass() NOEXCEPT;

private:

	/*
	*	Gaussian blur data definition.
	*/
	class GaussianBlurData final
	{

	public:

		//The direction.
		Vector2 direction{ 1.0f, 0.0f };

		//The inverse resolution.
		Vector2 inverseResolution;
	};

	//The gaussian blur data.
	GaussianBlurData data;

	/*
	*	Initializes the bloom vertical render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Renders the bloom vertical blur.
	*/
	void RenderInternal() NOEXCEPT;

};