#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/Vertex.h>

/*
*	Class that can procedurally generate models with the given input.
*/
class ProceduralModelGenerator final
{

public:

	/*
	*	Input class definition.
	*/
	class Input final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			NONE,

			FERN
		};

		//The type.
		Type _Type{ Type::NONE };

		//The texture resolution.
		uint32 _TextureResolution{ 1'024 };

		//Fern input.
		struct
		{
			//The number of branches.
			uint32 _Branches{ 8 };

			//The thickness.
			float32 _Thickness{ 0.5f };

			//The thickness middle point.
			float32 _ThicknessMiddlePoint{ 0.25f };

			//The bend.
			float32 _Bend{ 0.375f };

			//The bend middle point.
			float32 _BendMiddlePoint{ 0.25f };

			//The tilt.
			float32 _Tilt{ BaseMath::DegreesToRadians(12.0f) };

			//The number of vertical subdivisions per branch.
			uint32 _VerticalSubDivisions{ 4 };
		} _Fern;

	};

	/*
	*	Output class definition.
	*/
	class Output final
	{

	public:

		//The vertices.
		DynamicArray<Vertex> _Vertices;

		//The indices.
		DynamicArray<uint32> _Indices;

		//The albedo/thickness texture.
		Texture2D<Vector4<uint8>> _AlbedoThicknessTexture;

		//The normal map/displacement texture.
		Texture2D<Vector4<uint8>> _NormalMapDisplacementTexture;

		//The material properties texture.
		Texture2D<Vector4<uint8>> _MaterialPropertiesTexture;

	};

	/*
	*	Generates.
	*/
	static void Generate(const Input &input, Output *const RESTRICT output) NOEXCEPT;

private:

	/*
	*	Calculates normals & tangents.
	*/
	static void CalculateNormalsTangents(Output *const RESTRICT output) NOEXCEPT;

	/*
	*	Converts a texture.
	*/
	static void ConvertTexture(const Texture2D<Vector4<float32>> &input, Texture2D<Vector4<uint8>> *const RESTRICT output, const bool apply_gamma_correction) NOEXCEPT;

	/*
	*	Generates the 'FERN' type.
	*/
	static void GenerateFern(const Input &input, Output *const RESTRICT output) NOEXCEPT;

};