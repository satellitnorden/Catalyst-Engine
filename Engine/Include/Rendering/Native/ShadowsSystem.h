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

	/*
	*	Directional light cascade class definition.
	*/
	class DirectionalLightCascade final
	{

	public:

		//The world to light matrix.
		Matrix4x4 _WorldToLightMatrix;

		//The frustum.
		Frustum _Frustum;

		//The distance.
		float32 _Distance;

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
	*	Returns whether or not directional light cascades exists.
	*/
	FORCE_INLINE NO_DISCARD bool DirectionalLightCascadesExist() const NOEXCEPT
	{
		return _DirectionalLightCascadesExist;
	}

	/*
	*	Returns the directional light cascade at the given index.
	*/
	FORCE_INLINE NO_DISCARD const DirectionalLightCascade &GetDirectionalLightCascade(const uint8 index) const NOEXCEPT
	{
		return _DirectionalLightCascades[index];
	}

private:

	//Denotes whether or not directional light cascades exists.
	bool _DirectionalLightCascadesExist{ false };

	//The directional light cascades.
	StaticArray<DirectionalLightCascade, 4> _DirectionalLightCascades;

	/*
	*	Gathers an opaque model input stream.
	*/
	void GatherOpaqueModelInputStream
	(
		const uint8 light_index,
		RenderInputStream *const RESTRICT input_stream
	) NOEXCEPT;

	/*
	*	Gathers a masked model input stream.
	*/
	void GatherMaskedModelInputStream
	(
		const uint8 light_index,
		RenderInputStream* const RESTRICT input_stream
	) NOEXCEPT;

};