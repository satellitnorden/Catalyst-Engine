//Header file.
#include <Content/Procedural/ProceduralModelGenerator.h>

//STD.
#include <cmath>

/*
*	Generates.
*/
void ProceduralModelGenerator::Generate(const Input &input, Output *const RESTRICT output) NOEXCEPT
{
	switch (input._Type)
	{
		case Input::Type::FERN:
		{
			GenerateFern(input, output);

			break;
		}
	
		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
}

/*
*	Calculates normals & tangents.
*/
void ProceduralModelGenerator::CalculateNormalsTangents(Output *const RESTRICT output) NOEXCEPT
{
	//Reset the normal & tangents for all vertices.
	for (Vertex &vertex : output->_Vertices)
	{
		vertex._Normal = Vector3<float32>(0.0f, 0.0f, 0.0f);
		vertex._Tangent = Vector3<float32>(0.0f, 0.0f, 0.0f);
	}

	//Initialize a list of how many triangles each vertex is referenced by.
	DynamicArray<uint32> vertex_references;
	vertex_references.Upsize<false>(output->_Vertices.Size());
	Memory::Set(vertex_references.Data(), 0, sizeof(uint32) * vertex_references.Size());

	//Iterate over all triangles.
	for (uint64 index_index{ 0 }; index_index < output->_Indices.Size(); index_index += 3)
	{
		const uint32 index_0{ output->_Indices[index_index + 0] };
		const uint32 index_1{ output->_Indices[index_index + 1] };
		const uint32 index_2{ output->_Indices[index_index + 2] };

		Vertex &vertex_0{ output->_Vertices[index_0] };
		Vertex &vertex_1{ output->_Vertices[index_1] };
		Vertex &vertex_2{ output->_Vertices[index_2] };

		const Vector3<float32> position_edge_0{ vertex_1._Position - vertex_0._Position };
		const Vector3<float32> position_edge_1{ vertex_2._Position - vertex_0._Position };

		const Vector3<float32> normal{ Vector3<float32>::Normalize(Vector3<float32>::CrossProduct(position_edge_0, position_edge_1)) };

		vertex_0._Normal += normal;
		vertex_1._Normal += normal;
		vertex_2._Normal += normal;

		const Vector2<float32> texture_coordinate_edge_0{ vertex_1._TextureCoordinate - vertex_0._TextureCoordinate };
		const Vector2<float32> texture_coordinate_edge_1{ vertex_2._TextureCoordinate - vertex_0._TextureCoordinate };

		const float32 tangent_factor{ 1.0f / (texture_coordinate_edge_0._X * texture_coordinate_edge_1._Y - texture_coordinate_edge_1._X * texture_coordinate_edge_0._Y) };

		const Vector3<float32> tangent
		{
			(texture_coordinate_edge_1._Y * position_edge_0._X - texture_coordinate_edge_0._Y * position_edge_1._X) * tangent_factor,
			(texture_coordinate_edge_1._Y * position_edge_0._Y - texture_coordinate_edge_0._Y * position_edge_1._Y) * tangent_factor,
			(texture_coordinate_edge_1._Y * position_edge_0._Z - texture_coordinate_edge_0._Y * position_edge_1._Z) * tangent_factor
		};

		vertex_0._Tangent += tangent;
		vertex_1._Tangent += tangent;
		vertex_2._Tangent += tangent;

		++vertex_references[index_0];
		++vertex_references[index_1];
		++vertex_references[index_2];
	}

	//Now normalize all normals & tangents.
	for (uint64 vertex_index{ 0 }; vertex_index < output->_Vertices.Size(); ++vertex_index)
	{
		output->_Vertices[vertex_index]._Normal /= static_cast<float32>(vertex_references[vertex_index]);
		output->_Vertices[vertex_index]._Tangent /= static_cast<float32>(vertex_references[vertex_index]);
	}
}

/*
*	Converts a texture.
*/
void ProceduralModelGenerator::ConvertTexture(const Texture2D<Vector4<float32>> &input, Texture2D<Vector4<uint8>> *const RESTRICT output, const bool apply_gamma_correction) NOEXCEPT
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

/*
*	Generates the 'FERN' type.
*/
void ProceduralModelGenerator::GenerateFern(const Input &input, Output *const RESTRICT output) NOEXCEPT
{
	//Generate each branch.
	for (uint32 branch_index{ 0 }; branch_index < input._Fern._Branches; ++branch_index)
	{
		//Cache the start vertex for this branch.
		const uint32 branch_start_vertex{ static_cast<uint32>(output->_Vertices.Size()) };

		//Calculate the grid size.
		const Vector2<uint32> grid_size
		{
			1 + input._Fern._HorizontalSubDivisions,
			1 + input._Fern._VerticalSubDivisions
		};

		//Add all segments.
		for (uint32 vertical_segment{ 0 }; vertical_segment <= grid_size._Y; ++vertical_segment)
		{
			//Calculate the vertical scalar.
			const float32 vertical_scalar{ static_cast<float32>(vertical_segment) / static_cast<float32>(grid_size._Y) };

			//Calculate the thickness.
			float32 thickness;

			if (vertical_scalar < input._Fern._ThicknessMiddlePoint)
			{
				const float32 thickness_scalar{ vertical_scalar / input._Fern._ThicknessMiddlePoint };

				thickness = BaseMath::InverseSquare(thickness_scalar) * input._Fern._Thickness;
			}

			else
			{
				const float32 thickness_scalar{ (vertical_scalar - input._Fern._ThicknessMiddlePoint) / (1.0f - input._Fern._ThicknessMiddlePoint) };

				thickness = (1.0f - BaseMath::Square(thickness_scalar)) * input._Fern._Thickness;
			}

			for (uint32 horizontal_segment{ 0 }; horizontal_segment <= grid_size._X; ++horizontal_segment)
			{
				//Cache the vertex index.
				const uint32 vertex_index{ static_cast<uint32>(output->_Vertices.Size()) };

				//Add the vertex.
				output->_Vertices.Emplace();
				Vertex &vertex{ output->_Vertices.Back() };

				//Calculate the position.
				vertex._Position._X = vertical_segment == 0 || vertical_segment == grid_size._Y ? 0.0f : BaseMath::LinearlyInterpolate(-thickness * 0.5f, thickness * 0.5f, static_cast<float32>(horizontal_segment) / static_cast<float32>(grid_size._X));
				vertex._Position._Y = input._Fern._Bend * (2.0f * (1.0f - vertical_scalar) * vertical_scalar);
				vertex._Position._Z = static_cast<float32>(vertical_segment) / static_cast<float32>(grid_size._Y);

				//Calculate the texture coordinate.
				vertex._TextureCoordinate._X = static_cast<float32>(horizontal_segment) / static_cast<float32>(grid_size._X);
				vertex._TextureCoordinate._Y = static_cast<float32>(vertical_segment) / static_cast<float32>(grid_size._Y);

				//Add the indices.
				if (vertical_segment < grid_size._Y && horizontal_segment < grid_size._X)
				{
					output->_Indices.Emplace(vertex_index + 0);
					output->_Indices.Emplace(vertex_index + (grid_size._X + 1) + 0);
					output->_Indices.Emplace(vertex_index + (grid_size._X + 1) + 1);

					output->_Indices.Emplace(vertex_index + 0);
					output->_Indices.Emplace(vertex_index + (grid_size._X + 1) + 1);
					output->_Indices.Emplace(vertex_index + 1);
				}
			}
		}

		//Calculate the rotation.
		EulerAngles rotation;

		rotation._Roll = -input._Fern._Tilt;
		rotation._Yaw = static_cast<float32>(branch_index) / static_cast<float32>(input._Fern._Branches) * BaseMathConstants::DOUBLE_PI;
		rotation._Pitch = 0.0f;

		//Rotate the vertices.
		for (uint32 vertex_index{ branch_start_vertex }; vertex_index < static_cast<uint32>(output->_Vertices.Size()); ++vertex_index)
		{
			output->_Vertices[vertex_index]._Position.Rotate(rotation);
		}
	}

	//Calculate the normals & tangents.
	CalculateNormalsTangents(output);

	//Generate the textures.
	{
		//Set up the textures.
		Texture2D<Vector4<float32>> albedo_thickness_texture{ input._TextureResolution };
		Texture2D<Vector4<float32>> normal_map_displacement_texture{ input._TextureResolution };
		Texture2D<Vector4<float32>> material_properties_texture{ input._TextureResolution };
		Texture2D<Vector4<float32>> opacity_texture{ input._TextureResolution };

		//Set default values.
		for (uint32 Y{ 0 }; Y < input._TextureResolution; ++Y)
		{
			for (uint32 X{ 0 }; X < input._TextureResolution; ++X)
			{
				albedo_thickness_texture.At(X, Y) = Vector4<float32>(input._Fern._LeafAlbedo, 0.0f);
				normal_map_displacement_texture.At(X, Y) = Vector4<float32>(0.5f, 0.5f, 1.0f, 0.0f);
				material_properties_texture.At(X, Y) = Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f);
				opacity_texture.At(X, Y) = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
			}
		}

		//Draw in the stalk.
		{
			//Calculate the stalk width.
			const uint32 stalk_width{ BaseMath::Round<uint32>(static_cast<float32>(input._TextureResolution) * input._Fern._StalkThickness) };
			const uint32 half_stalk_width{ stalk_width / 2 };

			const uint32 texture_center{ input._TextureResolution / 2 };

			const uint32 stalk_start{ texture_center - half_stalk_width };
			const uint32 stalk_end{ texture_center + half_stalk_width };

			for (uint32 Y{ 0 }; Y < input._TextureResolution; ++Y)
			{
				for (uint32 X{ stalk_start }; X <= stalk_end; ++X)
				{
					const float32 normalized_distance_to_center{ BaseMath::SmoothStep<1>(BaseMath::Absolute(static_cast<float32>(X) - static_cast<float32>(texture_center)) / static_cast<float32>(half_stalk_width)) };
					ASSERT(normalized_distance_to_center >= 0.0f && normalized_distance_to_center <= 1.0f, "This shouldn't happen!");

					albedo_thickness_texture.At(X, Y) = Vector4<float32>(input._Fern._StalkAlbedo, BaseMath::LinearlyInterpolate(1.0f, 0.5f, normalized_distance_to_center));
					normal_map_displacement_texture.At(X, Y)._W = 1.0f - normalized_distance_to_center;
					material_properties_texture.At(X, Y) = Vector4<float32>(input._Fern._StalkRoughness, 0.0f, 1.0f, 0.0f);
					opacity_texture.At(X, Y) = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f);
				}
			}
		}

		//Draw in the leaves.
		{
			const uint32 leaf_height{ BaseMath::Floor<uint32>(static_cast<float32>(input._TextureResolution) / static_cast<float32>(input._Fern._Leaves)) };

			for (uint32 leaf_index{ 0 }; leaf_index < input._Fern._Leaves; ++leaf_index)
			{
				for (uint8 side_index{ 0 }; side_index < 2; ++side_index)
				{
					const uint32 start_x{ side_index == 0 ? 0 : input._TextureResolution / 2 };
					const uint32 end_x{ side_index == 0 ? input._TextureResolution / 2 : input._TextureResolution };
					const uint32 start_y{ leaf_index * leaf_height };
					const uint32 end_y{ (leaf_index + 1) * leaf_height };
					const uint32 middle_y{ start_y + ((end_y - start_y) / 2) };

					for (uint32 Y{ start_y }; Y <= end_y; ++Y)
					{
						const float32 normalized_distance_to_middle{ BaseMath::Absolute(static_cast<float32>(Y) - middle_y) / static_cast<float32>((end_y - start_y) / 2) };

						for (uint32 X{ start_x }; X <= end_x; ++X)
						{
							const float32 x_scalar{ static_cast<float32>(X - start_x) / static_cast<float32>(end_x - start_x) };
							const float32 thickness{ 2.0f * (1.0f - x_scalar) * x_scalar };

							if (normalized_distance_to_middle <= thickness)
							{
								albedo_thickness_texture.At(X, Y) = Vector4<float32>(input._Fern._LeafAlbedo, 0.5f);
								normal_map_displacement_texture.At(X, Y)._W = BaseMath::SmoothStep<1>(1.0f - normalized_distance_to_middle);
								material_properties_texture.At(X, Y) = Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f);
								opacity_texture.At(X, Y) = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f);
							}
						}
					}
				}
			}
		}

		//Calculate the normal from the displacement.
		for (uint32 Y{ 0 }; Y < input._TextureResolution; ++Y)
		{
			for (uint32 X{ 0 }; X < input._TextureResolution; ++X)
			{
				constexpr float32 MULTIPLIER{ 4.0f };

				const float32 left	{ normal_map_displacement_texture.At(X > 0 ? X - 1 : X, Y)._W * MULTIPLIER };
				const float32 right	{ normal_map_displacement_texture.At(X < (input._TextureResolution - 1) ? X + 1 : X, Y)._W * MULTIPLIER };
				const float32 down	{ normal_map_displacement_texture.At(X, Y > 0 ? Y - 1 : Y)._W * MULTIPLIER };
				const float32 up	{ normal_map_displacement_texture.At(X, Y < (input._TextureResolution - 1) ? Y + 1 : Y)._W * MULTIPLIER };

				const Vector3<float32> normal{ Vector3<float32>::Normalize(Vector3<float32>(left - right, down - up, 2.0f)) };

				normal_map_displacement_texture.At(X, Y)._X = normal._X * 0.5f + 0.5f;
				normal_map_displacement_texture.At(X, Y)._Y = normal._Y * 0.5f + 0.5f;
				normal_map_displacement_texture.At(X, Y)._Z = normal._Z * 0.5f + 0.5f;
			}
		}

		ConvertTexture(albedo_thickness_texture, &output->_AlbedoThicknessTexture, true);
		ConvertTexture(normal_map_displacement_texture, &output->_NormalMapDisplacementTexture, false);
		ConvertTexture(material_properties_texture, &output->_MaterialPropertiesTexture, false);
		ConvertTexture(opacity_texture, &output->_OpacityTexture, false);
	}
}