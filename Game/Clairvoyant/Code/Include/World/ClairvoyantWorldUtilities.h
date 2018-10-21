#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

namespace ClairvoyantWorldUtilities
{

	/*
	*	Generates a transformation.
	*/
	bool GenerateTransformation(const bool grass, const bool sand, const bool snow, const bool underwater, const float minimumAngle, const float height, const float minimumScale, const float maximumScale, const Vector3 &rotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT;

}