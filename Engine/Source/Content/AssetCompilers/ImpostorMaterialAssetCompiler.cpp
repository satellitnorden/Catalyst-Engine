//Header file.
#include <Content/AssetCompilers/ImpostorMaterialAssetCompiler.h>

//Core.
#include <Core/General/StaticString.h>

//File.
#include <File/Core/File.h>
#include <File/Readers/FBXReader.h>
#include <File/Readers/JPGReader.h>
#include <File/Readers/PNGReader.h>
#include <File/Utilities/TextParsingUtilities.h>
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

//Systems.
#include <Systems/TaskSystem.h>

//STL.
#include <fstream>

/*
*	Impostor material parameters class definition.
*/
class ImpostorMaterialParameters final
{

public:

	//The output.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _Output;

	//The resolution.
	Resolution _Resolution;

	//The super sample.
	uint32 _SuperSample;

	//The model.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _Model;

	//The albedo textures.
	StaticArray<Texture2D<Vector4<float32>>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _AlbedoTextures;

	//The normal map textures.
	StaticArray<Texture2D<Vector4<float32>>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _NormalMapTextures;

	//The roughness textures.
	StaticArray<Texture2D<Vector4<float32>>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _RoughnessTextures;

	//The opacity textures.
	StaticArray<Texture2D<Vector4<float32>>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> _OpacityTextures;

};

/*
*	Default constructor.
*/
ImpostorMaterialAssetCompiler::ImpostorMaterialAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;

	//This asset compiler should trigger a recompile.
	_TriggerRecompile = true;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString ImpostorMaterialAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return HashString("ImpostorMaterial");
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 ImpostorMaterialAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void ImpostorMaterialAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
{
	//Set up the compile data.
	CompileData *const RESTRICT compile_data{ new (_CompileDataAllocator.Allocate()) CompileData() };

	//Set the collection.
	compile_data->_Collection = compile_context._Collection;

	//Set the file path.
	compile_data->_FilePath = compile_context._FilePath;

	//Set the name.
	compile_data->_Name = compile_context._Name;

	//Set the compilation domain.
	compile_data->_CompilationDomain = compile_context._CompilationDomain;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(compile_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		ImpostorMaterialAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
	};
	task->_Arguments = compile_data;
	task->_ExecutableOnSameThread = true;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	compile_context._Tasks->Emplace(task);
}

/*
*	Loads a single asset with the given load context.
*/
NO_DISCARD Asset *const RESTRICT ImpostorMaterialAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	ASSERT(false, "This asset compiler should load no assets of it's own!");

	return nullptr;
}

/*
*	Compiles internally.
*/
void ImpostorMaterialAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	class SignedDistanceFieldData final
	{

	public:

		//The closest texel coordinate.
		Vector2<uint32> _ClosestTexelCoordinate;

		//The closest texel distance.
		float32 _ClosestTexelDistance;

		//Denotes whether or not this texel is outside.
		bool _Outside;

	};

	PROFILING_SCOPE("ImpostorMaterialAssetCompiler::CompileInternal");

	//Set up the parameters.
	ImpostorMaterialParameters parameters;

	//Set some reasonable defaults.
	parameters._SuperSample = 0;

	//Open the input file.
	std::ifstream input_file{ compile_data->_FilePath.Data() };

	//Iterate over the lines and fill in the parameters.
	{
		StaticArray<DynamicString, 8> arguments;
		std::string current_line;

		while (std::getline(input_file, current_line))
		{
			//Is this an output declaration?
			{
				const size_t position{ current_line.find("Output(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 1, "Output() needs one argument!");

					parameters._Output = arguments[0].Data();

					continue;
				}
			}

			//Is this a resolution declaration?
			{
				const size_t position{ current_line.find("Resolution(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 2, "Resolution() needs two arguments!");

					parameters._Resolution._Width = std::stoul(arguments[0].Data());
					parameters._Resolution._Height = std::stoul(arguments[1].Data());

					continue;
				}
			}

			//Is this a super sample declaration?
			{
				const size_t position{ current_line.find("SuperSample(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 1, "SuperSample() needs one argument!");

					parameters._SuperSample = std::stoul(arguments[0].Data());

					continue;
				}
			}

			//Is this a model declaration?
			{
				const size_t position{ current_line.find("Model(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 1, "Model() needs one argument!");

					parameters._Model = arguments[0].Data();

					continue;
				}
			}

			//Is this an albedo texture declaration?
			{
				const size_t position{ current_line.find("AlbedoTexture(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 2, "AlbedoTexture() needs two arguments!");

					const uint8 mesh_index{ static_cast<uint8>(std::stoul(arguments[0].Data())) };

					const File::Extension file_extension{ File::GetExtension(arguments[1].Data()) };

					switch (file_extension)
					{
						case File::Extension::JPG:
						{
							JPGReader::Read(arguments[1].Data(), &parameters._AlbedoTextures[mesh_index]);

							break;
						}

						case File::Extension::PNG:
						{
							PNGReader::Read(arguments[1].Data(), &parameters._AlbedoTextures[mesh_index]);

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}

					continue;
				}
			}

			//Is this an normal map texture declaration?
			{
				const size_t position{ current_line.find("NormalMapTexture(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 2, "NormalMapTexture() needs two arguments!");

					const uint8 mesh_index{ static_cast<uint8>(std::stoul(arguments[0].Data())) };

					const File::Extension file_extension{ File::GetExtension(arguments[1].Data()) };

					switch (file_extension)
					{
						case File::Extension::JPG:
						{
							JPGReader::Read(arguments[1].Data(), &parameters._NormalMapTextures[mesh_index]);

							break;
						}

						case File::Extension::PNG:
						{
							PNGReader::Read(arguments[1].Data(), &parameters._NormalMapTextures[mesh_index]);

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}

					continue;
				}
			}

			//Is this a roughness texture declaration?
			{
				const size_t position{ current_line.find("RoughnessTexture(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 2, "RoughnessTexture() needs two arguments!");

					const uint8 mesh_index{ static_cast<uint8>(std::stoul(arguments[0].Data())) };

					const File::Extension file_extension{ File::GetExtension(arguments[1].Data()) };

					switch (file_extension)
					{
						case File::Extension::JPG:
						{
							JPGReader::Read(arguments[1].Data(), &parameters._RoughnessTextures[mesh_index]);

							break;
						}

						case File::Extension::PNG:
						{
							PNGReader::Read(arguments[1].Data(), &parameters._RoughnessTextures[mesh_index]);

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}

					continue;
				}
			}

			//Is this an opacity texture declaration?
			{
				const size_t position{ current_line.find("OpacityTexture(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 2, "OpacityTexture() needs two arguments!");

					const uint8 mesh_index{ static_cast<uint8>(std::stoul(arguments[0].Data())) };

					const File::Extension file_extension{ File::GetExtension(arguments[1].Data()) };

					switch (file_extension)
					{
						case File::Extension::JPG:
						{
							JPGReader::Read(arguments[1].Data(), &parameters._OpacityTextures[mesh_index]);

							break;
						}

						case File::Extension::PNG:
						{
							PNGReader::Read(arguments[1].Data(), &parameters._OpacityTextures[mesh_index]);

							break;
						}

						default:
						{
							ASSERT(false, "Invalid case!");

							break;
						}
					}

					continue;
				}
			}

			//Couldn't figure out what this line is?
			ASSERT(false, "Unknown line %s", current_line.c_str());
		}
	}

	//Close the input file.
	input_file.close();

	//Apply the super sample.
	parameters._Resolution._Width <<= parameters._SuperSample;
	parameters._Resolution._Height <<= parameters._SuperSample;

	//Load the model.
	ModelFile model;
	
	if (!FBXReader::Read(parameters._Model.Data(), &model))
	{
		ASSERT(false, "Couldn't read %s!", parameters._Model.Data());
	}

	//Set up the axis aligned bounding box.
	AxisAlignedBoundingBox3D axis_aligned_bounding_box;

	for (const ModelFile::Mesh &mesh : model._Meshes)
	{
		for (const Vertex &vertex : mesh._Vertices)
		{
			axis_aligned_bounding_box.Expand(vertex._Position);
		}
	}

	//Create the impostor textures!
	Texture2D<Vector4<float32>> impostor_albedo_texture{ parameters._Resolution._Width, parameters._Resolution._Height };
	Texture2D<Vector4<float32>> impostor_normal_map_texture{ parameters._Resolution._Width, parameters._Resolution._Height };
	Texture2D<Vector4<float32>> impostor_material_properties_texture{ parameters._Resolution._Width, parameters._Resolution._Height };
	Texture2D<Vector4<float32>> impostor_opacity_texture{ parameters._Resolution._Width, parameters._Resolution._Height };

	for (uint32 Y{ 0 }; Y < parameters._Resolution._Height; ++Y)
	{
		for (uint32 X{ 0 }; X < parameters._Resolution._Width; ++X)
		{
			//Calculate the normalized coordinate.
			const Vector2<float32> normalized_coordinate
			{
				(static_cast<float32>(X) + 0.5f) / static_cast<float32>(parameters._Resolution._Width),
				(static_cast<float32>(Y) + 0.5f) / static_cast<float32>(parameters._Resolution._Height)
			};

			//Calculate the minimum/maximum coordinate.
			Vector2<float32> minimum;
			Vector2<float32> maximum;

			{
				minimum._X = axis_aligned_bounding_box._Minimum._X;
				minimum._Y = axis_aligned_bounding_box._Minimum._Y;

				maximum._X = axis_aligned_bounding_box._Maximum._X;
				maximum._Y = axis_aligned_bounding_box._Maximum._Y;

				//Center if around the origin on the X axis.
				const float32 min_distance_from_center{ BaseMath::Absolute(minimum._X) };
				const float32 max_distance_from_center{ BaseMath::Absolute(maximum._X) };

				if (min_distance_from_center > max_distance_from_center)
				{
					maximum._X += (min_distance_from_center - max_distance_from_center);
				}

				else
				{
					minimum._X -= (max_distance_from_center - min_distance_from_center);
				}
			}

			//Calculate the camera position.
			Vector3<float32> camera_position;

			camera_position._X = BaseMath::LinearlyInterpolate(minimum._X, maximum._X, normalized_coordinate._X);
			camera_position._Y = BaseMath::LinearlyInterpolate(minimum._Y, maximum._Y, normalized_coordinate._Y);
			camera_position._Z = axis_aligned_bounding_box._Minimum._Z - 1.0f;

			//Construct the ray.
			Ray ray;

			ray.SetOrigin(camera_position);
			ray.SetDirection(Vector3<float32>(0.0f, 0.0f, 1.0f));

			//Intersect every triangle and check for hits.
			float32 intersection_distance{ FLOAT32_MAXIMUM };
			uint64 intersected_mesh_index{ 0 };
			Vector3<float32> intersected_normal;
			Vector3<float32> intersected_tangent;
			Vector2<float32> intersected_texture_coordinate;

			for (uint64 mesh_index{ 0 }; mesh_index < model._Meshes.Size(); ++mesh_index)
			{
				//Cache the mesh.
				const ModelFile::Mesh &mesh{ model._Meshes[mesh_index] };

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
							//Calculate the vertex properties.
							Vector3<float32> normal;
							Vector3<float32> tangent;
							Vector2<float32> texture_coordinate;

							{
								//Calculate the intersected texture coordinate.
								const Vector3<float32> intersection_point{ ray._Origin + ray._Direction * temporary_intersection_distance };

								//Calculate the barycentric coordinate.
								const Vector3<float32> barycentric_coordinate{ CatalystGeometryMath::CalculateBarycentricCoordinates(triangle, intersection_point) };

								//Calculate the normal.
								normal = mesh._Vertices[mesh._Indices[index + 0]]._Normal * barycentric_coordinate[0] + mesh._Vertices[mesh._Indices[index + 1]]._Normal * barycentric_coordinate[1] + mesh._Vertices[mesh._Indices[index + 2]]._Normal * barycentric_coordinate[2];

								//Calculate the tangent.
								tangent = mesh._Vertices[mesh._Indices[index + 0]]._Tangent * barycentric_coordinate[0] + mesh._Vertices[mesh._Indices[index + 1]]._Tangent * barycentric_coordinate[1] + mesh._Vertices[mesh._Indices[index + 2]]._Tangent * barycentric_coordinate[2];

								//Calculate the texture coordinate.
								texture_coordinate = mesh._Vertices[mesh._Indices[index + 0]]._TextureCoordinate * barycentric_coordinate[0] + mesh._Vertices[mesh._Indices[index + 1]]._TextureCoordinate * barycentric_coordinate[1] + mesh._Vertices[mesh._Indices[index + 2]]._TextureCoordinate * barycentric_coordinate[2];
							}

							//Check the mask texture if this was really a hit.
							bool was_really_a_hit{ true };

							if (parameters._OpacityTextures[mesh_index].GetWidth() > 0)
							{
								const Vector4<float32> mask_value{ parameters._OpacityTextures[mesh_index].Sample(texture_coordinate, AddressMode::REPEAT) };

								was_really_a_hit = mask_value[0] >= 0.5f;
							}

							if (was_really_a_hit)
							{
								intersection_distance = temporary_intersection_distance;
								intersected_mesh_index = mesh_index;
								intersected_normal = normal;
								intersected_tangent = tangent;
								intersected_texture_coordinate = texture_coordinate;
							}
						}
					}
				}
			}

			//Was there a hit?
			if (intersection_distance != FLOAT32_MAXIMUM)
			{
				//Fill in the albedo texture.
				{
					Vector4<float32> albedo{ 1.0f, 1.0f, 1.0f, 1.0f };

					if (parameters._AlbedoTextures[intersected_mesh_index].GetWidth() > 0)
					{
						const Vector4<float32> texture_sample{ parameters._AlbedoTextures[intersected_mesh_index].Sample(intersected_texture_coordinate, AddressMode::REPEAT) };

						albedo._R = texture_sample._R;
						albedo._G = texture_sample._G;
						albedo._B = texture_sample._B;
					}

					impostor_albedo_texture.At(X, Y) = albedo;
				}

				//Fill in the normal map/displacement texture.
				{
					Vector4<float32> normal_map_displacement{ intersected_normal, 0.5f };

					if (parameters._NormalMapTextures[intersected_mesh_index].GetWidth() > 0)
					{
						//Calculate the tangent space matrix.
						Matrix3x3 tangent_space_matrix{ intersected_tangent, Vector3<float32>::CrossProduct(intersected_normal, intersected_tangent), intersected_normal };

						tangent_space_matrix._Matrix[0] = Vector3<float32>::Normalize(tangent_space_matrix._Matrix[0]);
						tangent_space_matrix._Matrix[1] = Vector3<float32>::Normalize(tangent_space_matrix._Matrix[1]);
						tangent_space_matrix._Matrix[2] = Vector3<float32>::Normalize(tangent_space_matrix._Matrix[2]);

						//Sample the normal map.
						Vector3<float32> normal_map_sample;
						
						{
							Vector4<float32> _normal_map_sample{ parameters._NormalMapTextures[intersected_mesh_index].Sample(intersected_texture_coordinate, AddressMode::REPEAT) };

							normal_map_sample._X = _normal_map_sample._X;
							normal_map_sample._Y = _normal_map_sample._Y;
							normal_map_sample._Z = _normal_map_sample._Z;
						}
						
						//Re-scale the normal map sample.
						normal_map_sample._X = normal_map_sample._X * 2.0f - 1.0f;
						normal_map_sample._Y = normal_map_sample._Y * 2.0f - 1.0f;
						normal_map_sample._Z = normal_map_sample._Z * 2.0f - 1.0f;

						//Normalize, to be safe.
						normal_map_sample.Normalize();

						//Calculate the surface normal.
						Vector3<float32> surface_normal{ tangent_space_matrix * normal_map_sample };

						//Flip it.
						surface_normal *= -1.0f;

						//Normalize, to be safe.
						surface_normal.Normalize();

						//Flip the normal if we hit the backside.
						if (Vector3<float32>::DotProduct(ray._Direction, surface_normal) < 0.0f)
						{
							surface_normal *= -1.0f;
						}

						//Re-scale the surface_normal.
						surface_normal._X = surface_normal._X * 0.5f + 0.5f;
						surface_normal._Y = surface_normal._Y * 0.5f + 0.5f;
						surface_normal._Z = surface_normal._Z * 0.5f + 0.5f;

						//Update the normal map/displacement value.
						normal_map_displacement._X = surface_normal._X;
						normal_map_displacement._Y = surface_normal._Y;
						normal_map_displacement._Z = surface_normal._Z;
					}

					impostor_normal_map_texture.At(X, Y) = normal_map_displacement;
				}

				//Fill in the material properties texture.
				{
					Vector4<float32> material_properties{ 1.0f, 0.0f, 1.0f, 0.0 };

					if (parameters._RoughnessTextures[intersected_mesh_index].GetWidth() > 0)
					{
						const Vector4<float32> texture_sample{ parameters._RoughnessTextures[intersected_mesh_index].Sample(intersected_texture_coordinate, AddressMode::REPEAT) };

						material_properties._X = texture_sample._X;
					}

					impostor_material_properties_texture.At(X, Y) = material_properties;
				}

				//Fill in the opacity texture.
				impostor_opacity_texture.At(X, Y) = Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f);
			}

			else
			{
				//Fill in the textures.
				impostor_albedo_texture.At(X, Y) = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
				impostor_normal_map_texture.At(X, Y) = Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f);
				impostor_material_properties_texture.At(X, Y) = Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f);
				impostor_opacity_texture.At(X, Y) = Vector4<float32>(0.0f, 0.0f, 0.0f, 1.0f);
			}
		}
	}

	//For each texel that is outside the opacity mask, calculate the closest pixel to it to "stretch out" the textures.
	for (uint32 Y{ 0 }; Y < impostor_opacity_texture.GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < impostor_opacity_texture.GetWidth(); ++X)
		{
			if (impostor_opacity_texture.At(X, Y)._X < 0.5f)
			{
				Vector2<uint32> closest_texel_coordinate{ UINT32_MAXIMUM, UINT32_MAXIMUM };
				float32 closest_texel_distance{ FLOAT32_MAXIMUM };

				const Vector2<float32> normalized_coordinate
				{
					(static_cast<float32>(X) + 0.5f) / static_cast<float32>(impostor_opacity_texture.GetWidth()),
					(static_cast<float32>(Y) + 0.5f) / static_cast<float32>(impostor_opacity_texture.GetHeight())
				};

				for (uint32 _Y{ 0 }; _Y < impostor_opacity_texture.GetHeight(); ++_Y)
				{
					for (uint32 _X{ 0 }; _X < impostor_opacity_texture.GetWidth(); ++_X)
					{
						if (impostor_opacity_texture.At(_X, _Y)._X >= 0.5f)
						{
							const Vector2<float32> _normalized_coordinate
							{
								(static_cast<float32>(_X) + 0.5f) / static_cast<float32>(impostor_opacity_texture.GetWidth()),
								(static_cast<float32>(_Y) + 0.5f) / static_cast<float32>(impostor_opacity_texture.GetHeight())
							};

							const float32 distance{ Vector2<float32>::LengthSquared(normalized_coordinate - _normalized_coordinate) };

							if (closest_texel_distance > distance)
							{
								closest_texel_coordinate = Vector2<uint32>(_X, _Y);
								closest_texel_distance = distance;
							}
						}
					}
				}

				if (closest_texel_coordinate != Vector2<uint32>(UINT32_MAXIMUM, UINT32_MAXIMUM))
				{
					impostor_albedo_texture.At(X, Y) = impostor_albedo_texture.At(closest_texel_coordinate._X, closest_texel_coordinate._Y);
					impostor_normal_map_texture.At(X, Y) = impostor_normal_map_texture.At(closest_texel_coordinate._X, closest_texel_coordinate._Y);
					impostor_material_properties_texture.At(X, Y) = impostor_material_properties_texture.At(closest_texel_coordinate._X, closest_texel_coordinate._Y);
				}
			}
		}
	}

	//Write out the impostor texture files.
	{
		//Determine the directory.
		char directory_path[MAXIMUM_FILE_PATH_LENGTH];

		sprintf_s(directory_path, "%s\\ImpostorTextures", GetSourceDirectoryPath(compile_data->_CompilationDomain));

		//Create the directory, if it doesn't exist.
		File::CreateDirectory(directory_path);

		//Write the albedo/thickness texture.
		{
			char file_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(file_buffer, "%s\\%s_Albedo.png", directory_path, compile_data->_Name.Data());

			PNGWriter::Write(impostor_albedo_texture, file_buffer);

			char asset_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(asset_buffer, "%s\\%s_AlbedoThickness.Texture2D", parameters._Output.Data(), compile_data->_Name.Data());

			std::ofstream asset_file{ asset_buffer };

			asset_file << "File1(" << file_buffer << ");" << std::endl;
			asset_file << "ChannelMapping(RED, FILE_1, RED);" << std::endl;
			asset_file << "ChannelMapping(GREEN, FILE_1, GREEN);" << std::endl;
			asset_file << "ChannelMapping(BLUE, FILE_1, BLUE);" << std::endl;
			asset_file << "ChannelMapping(ALPHA, FILE_1, ALPHA);" << std::endl;
			asset_file << "ApplyGammaCorrection();" << std::endl;
			asset_file << "BaseMipLevel(" << parameters._SuperSample << ");" << std::endl;

			asset_file.close();
		}

		//Write the normal map texture.
		{
			char file_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(file_buffer, "%s\\%s_NormalMap.png", directory_path, compile_data->_Name.Data());

			PNGWriter::Write(impostor_normal_map_texture, file_buffer);

			char asset_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(asset_buffer, "%s\\%s_NormalMapDisplacement.Texture2D", parameters._Output.Data(), compile_data->_Name.Data());

			std::ofstream asset_file{ asset_buffer };

			asset_file << "File1(" << file_buffer << ");" << std::endl;
			asset_file << "ChannelMapping(RED, FILE_1, RED);" << std::endl;
			asset_file << "ChannelMapping(GREEN, FILE_1, GREEN);" << std::endl;
			asset_file << "ChannelMapping(BLUE, FILE_1, BLUE);" << std::endl;
			asset_file << "ChannelMapping(ALPHA, FILE_1, ALPHA);" << std::endl;
			asset_file << "BaseMipLevel(" << parameters._SuperSample << ");" << std::endl;

			asset_file.close();
		}

		//Write the material properties texture.
		{
			char file_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(file_buffer, "%s\\%s_MaterialProperties.png", directory_path, compile_data->_Name.Data());

			PNGWriter::Write(impostor_material_properties_texture, file_buffer);

			char asset_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(asset_buffer, "%s\\%s_MaterialProperties.Texture2D", parameters._Output.Data(), compile_data->_Name.Data());

			std::ofstream asset_file{ asset_buffer };

			asset_file << "File1(" << file_buffer << ");" << std::endl;
			asset_file << "ChannelMapping(RED, FILE_1, RED);" << std::endl;
			asset_file << "ChannelMapping(GREEN, FILE_1, GREEN);" << std::endl;
			asset_file << "ChannelMapping(BLUE, FILE_1, BLUE);" << std::endl;
			asset_file << "ChannelMapping(ALPHA, FILE_1, ALPHA);" << std::endl;
			asset_file << "BaseMipLevel(" << parameters._SuperSample << ");" << std::endl;

			asset_file.close();
		}

		//Write the opacity texture.
		{
			char file_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(file_buffer, "%s\\%s_Opacity.png", directory_path, compile_data->_Name.Data());

			PNGWriter::Write(impostor_opacity_texture, file_buffer);

			char asset_buffer[MAXIMUM_FILE_PATH_LENGTH];
			sprintf_s(asset_buffer, "%s\\%s_Opacity.Texture2D", parameters._Output.Data(), compile_data->_Name.Data());

			std::ofstream asset_file{ asset_buffer };

			asset_file << "File1(" << file_buffer << ");" << std::endl;
			asset_file << "ChannelMapping(RED, FILE_1, RED);" << std::endl;
			asset_file << "ChannelMapping(GREEN, FILE_1, GREEN);" << std::endl;
			asset_file << "ChannelMapping(BLUE, FILE_1, BLUE);" << std::endl;
			asset_file << "ChannelMapping(ALPHA, FILE_1, ALPHA);" << std::endl;
			asset_file << "BaseMipLevel(" << parameters._SuperSample << ");" << std::endl;
			asset_file << "MipmapGenerationMode(OPACITY);";

			asset_file.close();
		}
	}

	//Write the material file.
	{
		char asset_buffer[MAXIMUM_FILE_PATH_LENGTH];
		sprintf_s(asset_buffer, "%s\\%s.Material", parameters._Output.Data(), compile_data->_Name.Data());

		std::ofstream asset_file{ asset_buffer };

		asset_file << "Type(MASKED);" << std::endl;
		asset_file << "AlbedoThicknessTexture(" << compile_data->_Name.Data() << "_AlbedoThickness);" << std::endl;
		asset_file << "NormalMapDisplacementTexture(" << compile_data->_Name.Data() << "_NormalMapDisplacement);" << std::endl;
		asset_file << "MaterialPropertiesTexture(" << compile_data->_Name.Data() << "_MaterialProperties);" << std::endl;
		asset_file << "OpacityTexture(" << compile_data->_Name.Data() << "_Opacity);" << std::endl;

		asset_file.close();
	}

	/*
	//Determine the collection directory.
	char collection_directory_path[MAXIMUM_FILE_PATH_LENGTH];

	if (compile_data->_Collection)
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION %s", GetCompiledDirectoryPath(compile_data->_CompilationDomain), compile_data->_Collection.Data());
	}

	else
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION Default", GetCompiledDirectoryPath(compile_data->_CompilationDomain));
	}

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(collection_directory_path);

	//Determine the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "%s\\Materials", collection_directory_path);

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(directory_path);
	
	//Determine the output file path.
	char output_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_file_path, "%s\\%s.ca", directory_path, compile_data->_Name.Data());

	//Open the output file.
	BinaryFile<BinaryFileMode::OUT> output_file{ output_file_path };

	//Write the asset header to the file.
	AssetHeader asset_header{ AssetTypeIdentifier(), CurrentVersion(), HashString(compile_data->_Name.Data()), compile_data->_Name.Data() };
	output_file.Write(&asset_header, sizeof(AssetHeader));

	//Just write the parameters.
	output_file.Write(&parameters, sizeof(MaterialParameters));

	//Close the output file.
	output_file.Close();
	*/
}