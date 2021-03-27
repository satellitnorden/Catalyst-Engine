#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Resources/Utilities/ResourceBuildingUtilities.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Readers/FBXReader.h>
#include <File/Readers/JPGReader.h>
#include <File/Readers/PNGReader.h>
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Geometry/Ray.h>

//Systems.
#include <Systems/ResourceSystem.h>

/*
*	Calculates the average color of the texture at the given file path.
*/
NO_DISCARD Vector4<float32> ResourceBuildingUtilities::CalculateAverageColor(const char *const RESTRICT texture_file_path) NOEXCEPT
{
	//Read the texture.
	Texture2D<Vector4<float32>> texture;

	switch (File::GetExtension(texture_file_path))
	{
		case File::Extension::JPG:
		{
			JPGReader::Read(texture_file_path, &texture);

			break;
		}

		case File::Extension::PNG:
		{
			PNGReader::Read(texture_file_path, &texture);

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Calculate the average color.
	const float32 dimensions_reciprocal{ 1.0f / (texture.GetWidth() * texture.GetHeight()) };
	Vector4<float32> average_color{ 0.0f, 0.0f, 0.0f, 0.0f };

	for (uint32 Y{ 0 }; Y < texture.GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < texture.GetWidth(); ++X)
		{
			average_color += texture.At(X, Y) * dimensions_reciprocal;
		}
	}

	//Return the average color.
	return average_color;
}

/*
*	Creates an impostor material, with the associated textures.
*/
void ResourceBuildingUtilities::BuildImpostorMaterial(const BuildImpostorMaterialParameters &parameters) NOEXCEPT
{
	//First of all, read the model file.
	ModelFile model_file;
	FBXReader::Read(parameters._ModelFilePath, &model_file);

	//Transform all vertices in all meshes.
	for (ModelFile::Mesh &mesh : model_file._Meshes)
	{
		for (Vertex &vertex : mesh._Vertices)
		{
			if (parameters._ModelTransformation != MatrixConstants::IDENTITY )
			{
				vertex.Transform(parameters._ModelTransformation, 0.0f);
			}
		}
	}

	//Determine the model space axis aligned bounding box.
	AxisAlignedBoundingBox3D axis_aligned_bounding_box;

	{
		//Iterate over all vertices in all meshes and expand the bounding box.
		for (ModelFile::Mesh &mesh : model_file._Meshes)
		{
			for (Vertex &vertex : mesh._Vertices)
			{
				axis_aligned_bounding_box.Expand(vertex._Position);
			}
		}
	}

	//Read the albedo textures.
	StaticArray<Texture2D<Vector4<float32>>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> albedo_textures;

	for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
	{
		if (!parameters._AlbedoTextureFilePaths[i])
		{
			continue;
		}

		switch (File::GetExtension(parameters._AlbedoTextureFilePaths[i]))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._AlbedoTextureFilePaths[i], &albedo_textures[i]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._AlbedoTextureFilePaths[i], &albedo_textures[i]);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	//Read the mask textures.
	StaticArray<Texture2D<Vector4<float32>>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> mask_textures;

	for (uint8 i{ 0 }; i < RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL; ++i)
	{
		if (!parameters._MaskTextureFilePaths[i])
		{
			continue;
		}

		switch (File::GetExtension(parameters._MaskTextureFilePaths[i]))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._MaskTextureFilePaths[i], &mask_textures[i]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._MaskTextureFilePaths[i], &mask_textures[i]);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	//The albedo texture should have 1.0f in it's alpha channel.
	for (Texture2D<Vector4<float32>>& albedo_texture : albedo_textures)
	{
		for (uint32 Y{ 0 }; Y < albedo_texture.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < albedo_texture.GetWidth(); ++X)
			{
				albedo_texture.At(X, Y)._A = 1.0f;
			}
		}
	}
	
	//Create the impostor textures!
	Texture2D<Vector4<float32>> impostor_albedo_texture{ parameters._Dimensions._Width, parameters._Dimensions._Height };
	Texture2D<Vector4<float32>> impostor_mask_texture{ parameters._Dimensions._Width, parameters._Dimensions._Height };

	for (uint32 Y{ 0 }; Y < parameters._Dimensions._Height; ++Y)
	{
		for (uint32 X{ 0 }; X < parameters._Dimensions._Width; ++X)
		{
			//Calculate the normalized coordinate.
			const Vector2<float32> normalized_coordinate{ static_cast<float32>(X) / static_cast<float32>(parameters._Dimensions._Width - 1), static_cast<float32>(Y) / static_cast<float32>(parameters._Dimensions._Height - 1) };
			
			//Calculate the camera position.
			Vector3<float32> camera_position;

			camera_position._X = CatalystBaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Minimum._X, axis_aligned_bounding_box._Maximum._X, normalized_coordinate._X);
			camera_position._Y = CatalystBaseMath::LinearlyInterpolate(axis_aligned_bounding_box._Minimum._Y, axis_aligned_bounding_box._Maximum._Y, normalized_coordinate._Y);
			camera_position._Z = axis_aligned_bounding_box._Minimum._Z - 1.0f;

			//Construct the ray.
			Ray ray;

			ray.SetOrigin(camera_position);
			ray.SetDirection(Vector3<float32>(0.0f, 0.0f, 1.0f));

			//Intersect every triangle and check for hits.
			float32 intersection_distance{ FLOAT32_MAXIMUM };
			uint64 intersected_mesh_index{ 0 };
			Vector2<float32> intersected_texture_coordinate;

			for (uint64 mesh_index{ 0 }, size{ model_file._Meshes.Size() }; mesh_index < size; ++mesh_index)
			{
				//Cache the mesh.
				const ModelFile::Mesh& mesh{ model_file._Meshes[mesh_index] };

				for (uint32 index{ 0 }; index < mesh._Indices.Size(); index += 3)
				{
					//Construct the triangle.
					Triangle triangle;

					triangle._Vertices[0] = mesh._Vertices[mesh._Indices[index + 0]]._Position;
					triangle._Vertices[1] = mesh._Vertices[mesh._Indices[index + 1]]._Position;
					triangle._Vertices[2] = mesh._Vertices[mesh._Indices[index + 2]]._Position;

					//Intersect the triangle.
					float32 temporary_intersection_distance;

					if (CatalystGeometryMath::RayTriangleIntersection(ray, triangle, &temporary_intersection_distance))
					{
						if (intersection_distance > temporary_intersection_distance)
						{
							//Calculate the texture coordinate.
							Vector2<float32> texture_coordinate;

							{
								//Calculate the intersected texture coordinate.
								Vector3<float32> intersection_point = ray._Origin + ray._Direction * temporary_intersection_distance;

								//Calculate the barycentric coordinate.
								Vector3<float32> barycentric_coordinate{ CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, intersection_point) };

								//Calculate the texture coordinate.
								texture_coordinate =  mesh._Vertices[mesh._Indices[index + 0]]._TextureCoordinate * barycentric_coordinate[0] + mesh._Vertices[mesh._Indices[index + 1]]._TextureCoordinate * barycentric_coordinate[1] + mesh._Vertices[mesh._Indices[index + 2]]._TextureCoordinate * barycentric_coordinate[2];

							}
							//Check the mask texture if this was really a hit.
							bool was_really_a_hit{ true };

							if (mask_textures[mesh_index].GetWidth() > 0)
							{
								const Vector4<float32> mask_value{ mask_textures[mesh_index].Sample(texture_coordinate, AddressMode::CLAMP_TO_EDGE) };

								was_really_a_hit = mask_value[0] >= 0.5f;
							}

							if (was_really_a_hit)
							{
								intersection_distance = temporary_intersection_distance;
								intersected_mesh_index = mesh_index;
								intersected_texture_coordinate = texture_coordinate;
							}
						}
					}
				}
			}

			//Was there a hit?
			if (intersection_distance != FLOAT32_MAXIMUM)
			{
				//Fill in the textures.
				impostor_albedo_texture.At(X, Y) = albedo_textures[intersected_mesh_index].Sample(intersected_texture_coordinate, AddressMode::CLAMP_TO_EDGE);
				impostor_mask_texture.At(X, Y) = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f);
			}

			else
			{
				//Fill in the textures.
				impostor_albedo_texture.At(X, Y) = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
				impostor_mask_texture.At(X, Y) = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
			}
		}
	}

	//Blur the black pixels of the albedo until there are none left!
	for (;;)
	{
		uint64 number_of_black_pixels{ 0 };

		Texture2D<Vector4<float32>> temporary_impostor_albedo_texture{ impostor_albedo_texture };

		for (uint32 Y{ 0 }; Y < parameters._Dimensions._Height; ++Y)
		{
			for (uint32 X{ 0 }; X < parameters._Dimensions._Width; ++X)
			{
				Vector4<float32> &impostor_albedo_texel{ impostor_albedo_texture.At(X, Y) };

				if (impostor_albedo_texel == Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f))
				{
					Vector4<float32> total_color{ 0.0f, 0.0f, 0.0f, 0.0f };
					float32 total_weight{ 0.0f };

					for (int8 y{ -1 }; y <= 1; ++y)
					{
						for (int8 x{ -1 }; x <= 1; ++x)
						{
							const uint32 sub_x{ CatalystBaseMath::Clamp<uint32>(X + x, 0, temporary_impostor_albedo_texture.GetWidth() - 1) };
							const uint32 sub_y{ CatalystBaseMath::Clamp<uint32>(Y + y, 0, temporary_impostor_albedo_texture.GetHeight() - 1) };

							const Vector4<float32> &temporary_impostor_albedo_texel{ temporary_impostor_albedo_texture.At(sub_x, sub_y) };

							if (temporary_impostor_albedo_texel != Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f))
							{
								total_color += temporary_impostor_albedo_texel;
								total_weight += 1.0f;
							}
						}
					}

					//Normalize the total color.
					if (total_weight > 0.0f)
					{
						total_color /= total_weight;

						//Write the blurred value.
						impostor_albedo_texel = total_color;
					}

					++number_of_black_pixels;
				}
			}
		}

		//If there are no black pixels left, it's done, finally!
		if (number_of_black_pixels == 0)
		{
			break;
		}
	}

	//Write the textures.
	{
		char buffer[128];
		sprintf_s(buffer, "%s\\%s_Impostor_Albedo.png", parameters._RawTexturesFilePath, parameters._Identifier);

		PNGWriter::Write(impostor_albedo_texture, buffer);
	}

	{
		char buffer[128];
		sprintf_s(buffer, "%s\\%s_Impostor_Mask.png", parameters._RawTexturesFilePath, parameters._Identifier);

		PNGWriter::Write(impostor_mask_texture, buffer);
	}

	//Build the impostor albedo texture.
	{
		//Build the texture 2D.
		Texture2DBuildParameters texture_2d_build_parameters;

		char output_buffer[128];
		sprintf_s(output_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\Impostors\\%s_Impostor_Albedo_Texture2D", parameters._Identifier);
		texture_2d_build_parameters._Output = output_buffer;
		char identifier_buffer[128];
		sprintf_s(identifier_buffer, "%s_Impostor_Albedo_Texture2D", parameters._Identifier);
		texture_2d_build_parameters._ID = identifier_buffer;
		texture_2d_build_parameters._DefaultWidth = 0;
		texture_2d_build_parameters._DefaultHeight = 0;
		char file_1_buffer[128];
		sprintf_s(file_1_buffer, "%s\\%s_Impostor_Albedo.png", parameters._RawTexturesFilePath, parameters._Identifier);
		texture_2d_build_parameters._File1 = file_1_buffer;
		texture_2d_build_parameters._File2 = nullptr;
		texture_2d_build_parameters._File3 = nullptr;
		texture_2d_build_parameters._File4 = nullptr;
		texture_2d_build_parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		texture_2d_build_parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		texture_2d_build_parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		texture_2d_build_parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		texture_2d_build_parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		texture_2d_build_parameters._ApplyGammaCorrection = true;
		texture_2d_build_parameters._TransformFunction = nullptr;
		texture_2d_build_parameters._BaseMipmapLevel = 0;
		texture_2d_build_parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(texture_2d_build_parameters);
	}

	//Build the impostor opacity texture.
	{
		//Build the texture 2D.
		Texture2DBuildParameters texture_2d_build_parameters;

		char output_buffer[128];
		sprintf_s(output_buffer, "..\\..\\..\\Resources\\Intermediate\\Textures\\Impostors\\%s_Impostor_Opacity_Texture2D", parameters._Identifier);
		texture_2d_build_parameters._Output = output_buffer;
		char identifier_buffer[128];
		sprintf_s(identifier_buffer, "%s_Impostor_Opacity_Texture2D", parameters._Identifier);
		texture_2d_build_parameters._ID = identifier_buffer;
		texture_2d_build_parameters._DefaultWidth = 0;
		texture_2d_build_parameters._DefaultHeight = 0;
		char file_1_buffer[128];
		sprintf_s(file_1_buffer, "%s\\%s_Impostor_Mask.png", parameters._RawTexturesFilePath, parameters._Identifier);
		texture_2d_build_parameters._File1 = file_1_buffer;
		texture_2d_build_parameters._File2 = nullptr;
		texture_2d_build_parameters._File3 = nullptr;
		texture_2d_build_parameters._File4 = nullptr;
		texture_2d_build_parameters._Default = Vector4<float32>(0.0f, 0.0f, 0.0f, 0.0f);
		texture_2d_build_parameters._ChannelMappings[0] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::RED);
		texture_2d_build_parameters._ChannelMappings[1] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::GREEN);
		texture_2d_build_parameters._ChannelMappings[2] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::BLUE);
		texture_2d_build_parameters._ChannelMappings[3] = Texture2DBuildParameters::ChannelMapping(Texture2DBuildParameters::File::FILE_1, Texture2DBuildParameters::Channel::ALPHA);
		texture_2d_build_parameters._ApplyGammaCorrection = true;
		texture_2d_build_parameters._TransformFunction = nullptr;
		texture_2d_build_parameters._BaseMipmapLevel = 0;
		texture_2d_build_parameters._MipmapLevels = 1;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildTexture2D(texture_2d_build_parameters);
	}

	//Build the impostor material.
	{
		//Build the material.
		MaterialBuildParameters material_build_parameters;

		char output_buffer[128];
		sprintf_s(output_buffer, "..\\..\\..\\Resources\\Intermediate\\Materials\\%s_Impostor_Material", parameters._Identifier);
		material_build_parameters._Output = output_buffer;
		char identifier_buffer[128];
		sprintf_s(identifier_buffer, "%s_Impostor_Material", parameters._Identifier);
		material_build_parameters._ID = identifier_buffer;
		material_build_parameters._Type = MaterialResource::Type::MASKED;
		material_build_parameters._AlbedoThicknessComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
		char albedo_identifier_buffer[128];
		sprintf_s(albedo_identifier_buffer, "%s_Impostor_Albedo_Texture2D", parameters._Identifier);
		material_build_parameters._AlbedoThicknessComponent._TextureResourceIdentifier = HashString(albedo_identifier_buffer);
		material_build_parameters._NormalMapDisplacementComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
		material_build_parameters._NormalMapDisplacementComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f));
		material_build_parameters._MaterialPropertiesComponent._Type = MaterialResource::MaterialResourceComponent::Type::COLOR;
		material_build_parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f));
		material_build_parameters._OpacityComponent._Type = MaterialResource::MaterialResourceComponent::Type::TEXTURE;
		char opacity_identifier_buffer[128];
		sprintf_s(opacity_identifier_buffer, "%s_Impostor_Opacity_Texture2D", parameters._Identifier);
		material_build_parameters._OpacityComponent._TextureResourceIdentifier = HashString(opacity_identifier_buffer);
		material_build_parameters._EmissiveMultiplier = 0.0f;
		material_build_parameters._DoubleSided = false;

		ResourceSystem::Instance->GetResourceBuildingSystem()->BuildMaterial(material_build_parameters);
	}
}
#endif