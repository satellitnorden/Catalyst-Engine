//Header file.
#include <Content/Procedural/ProceduralMaterialGenerator.h>

//Math.
#include <Math/General/RandomNumberGenerator.h>

//STD.
#include <cmath>

/*
*	Generates.
*/
void ProceduralMaterialGenerator::Generate(const Input &input, Output *const RESTRICT output) NOEXCEPT
{
	//Set up the textures.
	Texture2D<Vector4<float32>> albedo_thickness_texture{ input._Resolution };
	Texture2D<Vector4<float32>> normal_map_displacement_texture{ input._Resolution };
	Texture2D<Vector4<float32>> material_properties_texture{ input._Resolution };

	//Set default values.
	for (uint32 Y{ 0 }; Y < input._Resolution; ++Y)
	{
		for (uint32 X{ 0 }; X < input._Resolution; ++X)
		{
			albedo_thickness_texture.At(X, Y) = Vector4<float32>(input._BaseAlbedo, 1.0f);
			normal_map_displacement_texture.At(X, Y) = Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f);
			material_properties_texture.At(X, Y) = Vector4<float32>(input._BaseRoughness, input._BaseMetallic, 1.0f, input._BaseEmissive);
		}
	}

	//Apply all layers.
	for (uint64 layer_index{ 0 }; layer_index < input._Layers.Size(); ++layer_index)
	{
		//Cache the layer.
		const Input::Layer &layer{ input._Layers[layer_index] };

		//Set up the random number generator.
		RandomNumberGenerator random_number_generator{ layer_index };

		for (uint32 Y{ 0 }; Y < input._Resolution; ++Y)
		{
			for (uint32 X{ 0 }; X < input._Resolution; ++X)
			{
				//Calculate the weight for this pixel.
				float32 weight{ 0.0f };

				switch (layer._Type)
				{
					case Input::Layer::Type::NONE:
					{
						//Nothing to do here. (:

						break;
					}

					case Input::Layer::Type::WHITE_NOISE:
					{
						weight = random_number_generator.RandomFloat<float32>();

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}

				//Blend in the albedo.
				for (uint8 element_index{ 0 }; element_index < 3; ++element_index)
				{
					albedo_thickness_texture.At(X, Y)[element_index] = BaseMath::LinearlyInterpolate<float32>(albedo_thickness_texture.At(X, Y)[element_index], layer._Albedo[element_index], weight);
				}

				//Blend in the displacement.
				normal_map_displacement_texture.At(X, Y)._W = BaseMath::LinearlyInterpolate<float32>(normal_map_displacement_texture.At(X, Y)._W, 1.0f, weight);

				//Blend in the material properties.
				material_properties_texture.At(X, Y)._X = BaseMath::LinearlyInterpolate<float32>(material_properties_texture.At(X, Y)._X, layer._Roughness, weight);
				material_properties_texture.At(X, Y)._Y = BaseMath::LinearlyInterpolate<float32>(material_properties_texture.At(X, Y)._Y, layer._Metallic, weight);
				material_properties_texture.At(X, Y)._W = BaseMath::LinearlyInterpolate<float32>(material_properties_texture.At(X, Y)._W, layer._Emissive, weight);
			}
		}
	}

	//Normalize the displacement.
	{
		float32 minimum_displacement{ FLOAT32_MAXIMUM };
		float32 maximum_displacement{ -FLOAT32_MAXIMUM };

		for (uint32 Y{ 0 }; Y < input._Resolution; ++Y)
		{
			for (uint32 X{ 0 }; X < input._Resolution; ++X)
			{
				minimum_displacement = BaseMath::Minimum<float32>(minimum_displacement, normal_map_displacement_texture.At(X, Y)._W);
				maximum_displacement = BaseMath::Maximum<float32>(maximum_displacement, normal_map_displacement_texture.At(X, Y)._W);
			}
		}

		if (minimum_displacement != maximum_displacement)
		{
			for (uint32 Y{ 0 }; Y < input._Resolution; ++Y)
			{
				for (uint32 X{ 0 }; X < input._Resolution; ++X)
				{
					normal_map_displacement_texture.At(X, Y)._W = BaseMath::Scale(normal_map_displacement_texture.At(X, Y)._W, minimum_displacement, maximum_displacement, 0.0f, 1.0f);
				}
			}
		}

		else
		{
			for (uint32 Y{ 0 }; Y < input._Resolution; ++Y)
			{
				for (uint32 X{ 0 }; X < input._Resolution; ++X)
				{
					normal_map_displacement_texture.At(X, Y)._W = 0.5f;
				}
			}
		}
	}

	//Calculate the normal from the displacement.
	{
		const float32 texel_distance{ 1.0f / static_cast<float32>(input._Resolution) };

		for (uint32 Y{ 0 }; Y < input._Resolution; ++Y)
		{
			for (uint32 X{ 0 }; X < input._Resolution; ++X)
			{
				const float32 left{ normal_map_displacement_texture.At(X > 0 ? X - 1 : X, Y)._W * input._DisplacementScale };
				const float32 right{ normal_map_displacement_texture.At(X < (input._Resolution - 1) ? X + 1 : X, Y)._W * input._DisplacementScale };
				const float32 down{ normal_map_displacement_texture.At(X, Y > 0 ? Y - 1 : Y)._W * input._DisplacementScale };
				const float32 up{ normal_map_displacement_texture.At(X, Y < (input._Resolution - 1) ? Y + 1 : Y)._W * input._DisplacementScale };

				const Vector3<float32> normal{ Vector3<float32>::Normalize(Vector3<float32>(left - right, down - up, texel_distance * 2.0f)) };

				normal_map_displacement_texture.At(X, Y)._X = normal._X * 0.5f + 0.5f;
				normal_map_displacement_texture.At(X, Y)._Y = normal._Y * 0.5f + 0.5f;
				normal_map_displacement_texture.At(X, Y)._Z = normal._Z * 0.5f + 0.5f;
			}
		}
	}

	//Fill in the ambient occlusion (just copied from the displacement, for now...
	for (uint32 Y{ 0 }; Y < input._Resolution; ++Y)
	{
		for (uint32 X{ 0 }; X < input._Resolution; ++X)
		{
			material_properties_texture.At(X, Y)._Z = normal_map_displacement_texture.At(X, Y)._W;
		}
	}

	//Convert the textures into the output.
	ConvertTexture(albedo_thickness_texture, &output->_AlbedoThicknessTexture, true);
	ConvertTexture(normal_map_displacement_texture, &output->_NormalMapDisplacementTexture, false);
	ConvertTexture(material_properties_texture, &output->_MaterialPropertiesTexture, false);
}

/*
*	Converts a texture.
*/
void ProceduralMaterialGenerator::ConvertTexture(const Texture2D<Vector4<float32>> &input, Texture2D<Vector4<uint8>> *const RESTRICT output, const bool apply_gamma_correction) NOEXCEPT
{
	output->Initialize(input.GetResolution());

	for (uint32 Y{ 0 }; Y < output->GetResolution(); ++Y)
	{
		for (uint32 X{ 0 }; X < output->GetResolution(); ++X)
		{
			for (uint8 element_index{ 0 }; element_index < 4; ++element_index)
			{
				float32 element{ input.At(X, Y)[element_index] };

				if (apply_gamma_correction && element_index < 3)
				{
					element = std::pow(element, 2.2f);
				}

				output->At(X, Y)[element_index] = static_cast<uint8>(element * static_cast<float32>(UINT8_MAXIMUM));
			}
		}
	}
}