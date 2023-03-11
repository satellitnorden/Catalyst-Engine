#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class WindSystem final
{

public:

	/*
	*	Initializes the wind system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the wind system during render.
	*/
	void RenderUpdate() NOEXCEPT;

	/*
	*	Returns the render data table layout.
	*/
	FORCE_INLINE NO_DISCARD RenderDataTableLayoutHandle GetRenderDataTableLayout() const NOEXCEPT
	{
		return _RenderDataTableLayout;
	}

	/*
	*	Returns the current render data table.
	*/
	FORCE_INLINE NO_DISCARD RenderDataTableHandle GetCurrentRenderDataTable() const NOEXCEPT
	{
		return _CurrentRenderDataTable;
	}

private:

	/*
	*	Wind rendering data class definition.
	*/
	class WindRenderingData final
	{

	public:

		//The wind direction.
		Vector3<float32> _WindDirection;

		//The wind speed.
		float32 _WindSpeed;

	};

	//The wind direction.
	Vector3<float32> _WindDirection;

	//The wind speed.
	float32 _WindSpeed;

	//The render data table layout.
	RenderDataTableLayoutHandle _RenderDataTableLayout;

	//The render data tables.
	DynamicArray<RenderDataTableHandle> _RenderDataTables;

	//The current render data table.
	RenderDataTableHandle _CurrentRenderDataTable;

	//The uniform buffers.
	DynamicArray<BufferHandle> _UniformBuffers;

	//The current wind rendering data.
	WindRenderingData _CurrentWindRenderingData;

};