#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class PostProcessingSystem final
{

public:

	/*
	*	Returns the depth of field focus distance.
	*/
	FORCE_INLINE NO_DISCARD float32 GetDepthOfFieldFocusDistance() const NOEXCEPT
	{
		return _DepthOfFieldFocusDistance;
	}

	/*
	*	Sets the depth of field focus distance.
	*/
	FORCE_INLINE void SetDepthOfFieldFocusDistance(const float32 value) NOEXCEPT
	{
		_DepthOfFieldFocusDistance = value;
	}

	/*
	*	Returns the depth of field size.
	*/
	FORCE_INLINE NO_DISCARD float32 GetDepthOfFieldSize() const NOEXCEPT
	{
		return _DepthOfFieldSize;
	}

	/*
	*	Sets the depth of field size.
	*/
	FORCE_INLINE void SetDepthOfFieldSize(const float32 value) NOEXCEPT
	{
		_DepthOfFieldSize = value;
	}

private:

	//The depth of field focus distance.
	float32 _DepthOfFieldFocusDistance{ 1'024.0f };

	//The depth of field size.
	float32 _DepthOfFieldSize{ 0.001f };

};