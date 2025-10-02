//Header file.
#include <Content/Procedural/ProceduralModelGenerator.h>

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
			1,
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
}