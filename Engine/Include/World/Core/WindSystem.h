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

	/*
	*	Returns the wind direction.
	*/
	FORCE_INLINE NO_DISCARD const Vector3<float32> &GetWindDirection() const NOEXCEPT
	{
		return _WindDirection;
	}

	/*
	*	Returns the wind speed.
	*/
	FORCE_INLINE NO_DISCARD float32 GetWindSpeed() const NOEXCEPT
	{
		return _WindSpeed;
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

	/*
	*	Wind uniform data class definition.
	*/
	class WindUniformData final
	{

	public:

		//The previous wind direction.
		Vector4<float32> _PreviousWindDirectionSpeed;

		//The current wind direction.
		Vector4<float32> _CurrentWindDirectionSpeed;

		//The previous wind time.
		float32 _PreviousWindTime;

		//The current wind time.
		float32 _CurrentWindTime;

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

	//The wind uniform data.
	WindUniformData _WindUniformData;

};