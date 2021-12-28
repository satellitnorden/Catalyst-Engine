#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/GraphicsPipelines/DebugGraphicsPipeline.h>
#include <Rendering/Native/RenderPasses/RenderPass.h>

class DebugRenderPass final : public RenderPass
{
	
public:

	//Enumeration covering all modes.
	enum class Mode : uint8
	{
		NONE,
		ALBEDO,
		THICKNESS,
		NORMAL,
		DEPTH,
		ROUGHNESS,
		METALLIC,
		AMBIENT_OCCLUSION,
		EMISSIVE
	};

	//Singleton declaration.
	DECLARE_SINGLETON(DebugRenderPass);

	/*
	*	Default constructor.
	*/
	DebugRenderPass() NOEXCEPT;

	/*
	*	Sets the mode.
	*/
	FORCE_INLINE void SetMode(const Mode value) NOEXCEPT
	{
		_RequestedMode = value;
	}

private:

	//The requested mode.
	Mode _RequestedMode{ Mode::NONE };

	//The current mode.
	Mode _CurrentMode{ Mode::NONE };

	//The debug graphics pipeline.
	DebugGraphicsPipeline _DebugGraphicsPipeline;

	/*
	*	Initializes this render pass.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Executes this render pass.
	*/
	void Execute() NOEXCEPT;

	/*
	*	Terminates this render pass.
	*/
	void Terminate() NOEXCEPT;

};
#endif