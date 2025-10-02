#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
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
	*	Generates the 'FERN' type.
	*/
	static void GenerateFern(const Input &input, Output *const RESTRICT output) NOEXCEPT;

};