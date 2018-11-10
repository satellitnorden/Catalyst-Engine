#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

class ClairvoyantLocationArchitect final
{

public:

	/*
	*	Initializes the Clairvoyant location architect.
	*/
	static void Initialize() NOEXCEPT;

private:

	//The offsets resolution.
	constexpr static uint8 OFFSETS_RESOLUTION{ 10 };

	//The offsets.
	static DynamicArray<Vector3> _Offsets;

	/*
	*	Initializes the offsets.
	*/
	static void InitializeOffsets() NOEXCEPT;

	/*
	*	Given an axis-aligned bounding box and an extent, find the most appropiate position.
	*/
	static Vector3 FindMostAppropriatePosition(const AxisAlignedBoundingBox &box, const float extent) NOEXCEPT;

};