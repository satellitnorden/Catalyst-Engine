#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Rendering.
#include <Rendering/Engine/RenderPasses/RenderPass.h>

class VolumetricFogRenderPass final : public RenderPass
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(VolumetricFogRenderPass);

	/*
	*	Default constructor.
	*/
	VolumetricFogRenderPass() NOEXCEPT;

private:

	/*
	*	Volumetric fog data definition.
	*/
	class VolumetricFogData final
	{

	public:

		//The density.
		float _Density;

		//The ray distance squared.
		float _RayDistanceSquared;

		//The number of ray steps.
		int32 _NumberOfRaySteps;

	};

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The volumetric fog data.
	VolumetricFogData _Data;

	/*
	*	Initializes the volumetric fog render pass.
	*/
	void InitializeInternal() NOEXCEPT;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data table.
	*/
	void CreateRenderDataTable() NOEXCEPT;

	/*
	*	Renders the volumetric fog.
	*/
	void RenderInternal() NOEXCEPT;

};
#endif