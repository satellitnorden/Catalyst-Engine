#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

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

		//The axis-aligned bounding box.
		const AxisAlignedBoundingBox *RESTRICT _AxisAlignedBoundingBox;

		//The minimum angle.
		float _MinimumAngle;

		//The minimum scale.
		float _MinimumScale;

		//The maximum scale.
		float _MaximumScale;

		//The transformation.
		Matrix4 *RESTRICT _Transformation;

	};

	/*
	*	Generates a transformation.
	*/
	bool GenerateTransformation(const TransformationGenerationProperties &properties) NOEXCEPT;

}