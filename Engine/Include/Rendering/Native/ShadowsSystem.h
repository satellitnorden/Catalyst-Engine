#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Matrix.h>
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Frustum.h>
#include <Rendering/Native/RenderInputStream.h>

class ShadowsSystem final
{

public:

	//Constants.
	constexpr static uint64 MAXIMUM_NUMBER_OF_SHADOW_MAP_DATA{ 24 };

	/*
	*	Shadow map data class defunition.
	*/
	class ShadowMapData final
	{

	public:

		//The world to light matrix.
		Matrix4x4 _WorldToLightMatrix;

		//The frustum.
		Frustum _Frustum;

		//The distance.
		float32 _Distance;

		//The depth buffer.
		DepthBufferHandle _DepthBuffer;

		//The render target.
		RenderTargetHandle _RenderTarget;

		//The render target index.
		uint32 _RenderTargetIndex;

		////////////////////////////
		// DIRECTIONAL LIGHT DATA //
		////////////////////////////
		Vector3<float32> _Direction;

	};

	/*
	*	Post-initializes the shadows system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the shadows system during the pre render update phase.
	*/
	void PreRenderUpdate() NOEXCEPT;

	/*
	*	Returns the number of shadow map data.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetNumberOfShadowMapData() const NOEXCEPT
	{
		return static_cast<uint32>(_ShadowMapData.Size());
	}

	/*
	*	Returns the shadow map data at the given index.
	*/
	FORCE_INLINE NO_DISCARD const ShadowMapData &GetShadowMapData(const uint8 index) const NOEXCEPT
	{
		return _ShadowMapData[index];
	}

private:

	//The shadow map data.
	DynamicArray<ShadowMapData> _ShadowMapData;

	/*
	*	Gathers an opaque model input stream.
	*/
	void GatherOpaqueModelInputStream
	(
		const uint8 shadow_map_data_index,
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

	/*
	*	Gathers a masked model input stream.
	*/
	void GatherMaskedModelInputStream
	(
		const uint8 shadow_map_data_index,
		RenderInputStream* const RESTRICT input_stream
	) NOEXCEPT;

};