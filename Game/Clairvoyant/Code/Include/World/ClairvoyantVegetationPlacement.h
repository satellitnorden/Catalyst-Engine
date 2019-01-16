#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/ArrayProxy.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

//World.
#include <World/ClairvoyantWorldConstants.h>

namespace ClairvoyantVegetationPlacement
{

	/*
	*	Terrain material weight definition.
	*/
	class TerrainMaterialWeight final
	{

	public:

		/*
		*	Default constructor, prohibited - must be constructed with the proper arguments.
		*/
		constexpr TerrainMaterialWeight() NOEXCEPT = delete;

		/*
		*	Constructor taking the material and the weight.
		*/
		constexpr TerrainMaterialWeight(const ClairvoyantTerrainMaterial initialMaterial, const float initialWeight) NOEXCEPT
			:
			_Material(initialMaterial),
			_Weight(initialWeight)
		{

		}

		//The material.
		ClairvoyantTerrainMaterial _Material;

		//The weight.
		float _Weight;

	};

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

		//The material weights.
		ArrayProxy<TerrainMaterialWeight> _MaterialWeights;

	};

	/*
	*	Generates a transformation.
	*/
	bool GenerateTransformation(const TransformationGenerationProperties &properties, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT;

}