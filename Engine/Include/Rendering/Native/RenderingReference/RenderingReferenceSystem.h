#if defined(CATALYST_ENABLE_RENDERING_REFERENCE)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/Texture2D.h>

class RenderingReferenceSystem final
{

public:

	/*
	*	Updates the rendering reference system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT;

	/*
	*	Returns whether or not rendering reference is in progress.
	*/
	FORCE_INLINE NO_DISCARD bool IsRenderingReferenceInProgress() const NOEXCEPT
	{
		return _RenderingReferenceInProgress;
	}

private:

	//Denotes whether or not rendering reference is in progress.
	bool _RenderingReferenceInProgress{ false };

	//The rendering reference texture.
	Texture2D<Vector4<float>> _RenderingReferenceTexture;

	//The rendering reference texture handle.
	Texture2DHandle _RenderingReferenceTextureHandle{ EMPTY_HANDLE };

	//The rendering reference texture index.
	uint32 _RenderingReferenceTextureIndex;

	//The number of iterations.
	uint32 _Iterations{ 0 };

	/*
	*	Updates the rendering reference.
	*/
	void UpdateRenderingReference() NOEXCEPT;

	/*
	*	Calculates a texel.
	*/
	void CalculateTexel(const uint32 X, const uint32 Y) NOEXCEPT;

	/*
	*	Calculates the ray direction.
	*/
	Vector3<float> CalculateRayDirection(const uint32 X, const uint32 Y) NOEXCEPT;

};
#endif