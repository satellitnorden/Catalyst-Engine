#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

//World.
#include <World/ClairvoyantWorldConstants.h>

namespace ClairvoyantVegetationPlacement
{

	/*
	*	Transformation generation properties. definition.
	*/
	class TransformationGenerationProperties final
	{

	public:

		//The rotation.
		Vector3<float> _Rotation;

		//The minimum scale.
		float _MinimumScale;

		//The maximum scale.
		float _MaximumScale;

		//The material.
		ClairvoyantTerrainMaterial _Material;

	};

	/*
	*	Generates a transformation.
	*/
	bool GenerateTransformation(const TransformationGenerationProperties &properties, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT;

}