#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

class ProceduralMaterialGenerator final
{

public:

	/*
	*	Input class definition.
	*/
	class Input final
	{

	public:

		/*
		*	Layer class definition.
		*/
		class Layer final
		{

		public:

			//Enumeration covering all types.
			enum class Type : uint8
			{
				NONE,

				WHITE_NOISE
			};

			//The albedo.
			Vector3<float32> _Albedo{ 0.5f, 0.5f, 0.5f };

			//The roughness.
			float32 _Roughness{ 1.0f };

			//The metallic.
			float32 _Metallic{ 0.0f };

			//The emissive.
			float32 _Emissive{ 0.0f };

			//The type.
			Type _Type{ Type::NONE };

		};

		//The resolution.
		uint32 _Resolution{ 256 };

		//The base albedo.
		Vector3<float32> _BaseAlbedo{ 0.5f, 0.5f, 0.5f };

		//The base roughness.
		float32 _BaseRoughness{ 1.0f };

		//The base metallic.
		float32 _BaseMetallic{ 0.0f };

		//The base emissive.
		float32 _BaseEmissive{ 0.0f };

		//The displacement scale.
		float32 _DisplacementScale{ 0.025f };

		//The layers.
		DynamicArray<Layer> _Layers;

	};

	/*
	*	Output class definition.
	*/
	class Output final
	{

	public:

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
	*	Converts a texture.
	*/
	static void ConvertTexture(const Texture2D<Vector4<float32>> &input, Texture2D<Vector4<uint8>> *const RESTRICT output, const bool apply_gamma_correction) NOEXCEPT;

};