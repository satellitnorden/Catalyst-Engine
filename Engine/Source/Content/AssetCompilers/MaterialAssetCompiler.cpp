//Header file.
#include <Content/AssetCompilers/MaterialAssetCompiler.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Readers/JPGReader.h>
#include <File/Readers/PNGReader.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/ContentSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//STL.
#include <fstream>

//Singleton definition.
DEFINE_SINGLETON(MaterialAssetCompiler);

/*
*	Material parameters class definition.
*/
class MaterialParameters final
{

public:

	/*
	*	Component class definition.
	*/
	class Component final
	{

	public:

		//Enumeration covering all types.
		enum class Type : uint8
		{
			COLOR,
			TEXTURE
		};

		//The type.
		Type _Type;

		union
		{
			//The color.
			Color _Color;

			//The texture
			HashString _Texture;
		};

		/*
		*	Default constructor.
		*/
		FORCE_INLINE Component() NOEXCEPT
		{

		}

	};

	//The type.
	MaterialAsset::Type _Type;

	/*
	*	The albedo/thickness component.
	*	Expects the albedo to be in the RGB channels, and the thickness to be in the A channel.
	*/
	Component _AlbedoThicknessComponent;

	/*
	*	The normal map/displacement component.
	*	Expects the (tangent space) normal to be in the RGB channels, and the displacement in the A channel.
	*/
	Component _NormalMapDisplacementComponent;

	/*
	*	The material properties component.
	*	Expects the roughness to be in the R channel, the metallic in the G channel, the ambient occlusion in the B channel and the emissive in the A channel.
	*/
	Component _MaterialPropertiesComponent;

	/*
	*	The opacity component.
	*	Expects the opacity to be in the R channel.
	*/
	Component _OpacityComponent;

	//The emissive multiplier.
	float32 _EmissiveMultiplier;

	//Denotes whether or not this material is double-sided.
	bool _DoubleSided;

};

/*
*	Default constructor.
*/
MaterialAssetCompiler::MaterialAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString MaterialAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return MaterialAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 MaterialAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void MaterialAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
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
		MaterialAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
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
NO_DISCARD Asset *const RESTRICT MaterialAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	MaterialAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) MaterialAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		MaterialAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
	};
	task->_Arguments = load_data;
	task->_ExecutableOnSameThread = true;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	load_context._Tasks->Emplace(task);

	//Return the new asset!
	return new_asset;
}

/*
*	Runs after load.
*/
void MaterialAssetCompiler::PostLoad() NOEXCEPT
{
	while (PostLinkData *const RESTRICT post_link_data{ _PostLinkQueue.Pop() })
	{
		*post_link_data->_Asset = ContentSystem::Instance->GetAsset<Texture2DAsset>(post_link_data->_Identifier);
	}
}

/*
*	Compiles internally.
*/
void MaterialAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("MaterialAssetCompiler::CompileInternal");

	//Set up the parameters.
	MaterialParameters parameters;

	//Set some reasonable defaults.
	parameters._Type = MaterialAsset::Type::OPAQUE;
	parameters._AlbedoThicknessComponent._Type = MaterialParameters::Component::Type::COLOR;
	parameters._AlbedoThicknessComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 0.5f, 1.0f));
	parameters._NormalMapDisplacementComponent._Type = MaterialParameters::Component::Type::COLOR;
	parameters._NormalMapDisplacementComponent._Color = Color(Vector4<float32>(0.5f, 0.5f, 1.0f, 0.5f));
	parameters._MaterialPropertiesComponent._Type = MaterialParameters::Component::Type::COLOR;
	parameters._MaterialPropertiesComponent._Color = Color(Vector4<float32>(1.0f, 0.0f, 1.0f, 0.0f));
	parameters._OpacityComponent._Type = MaterialParameters::Component::Type::COLOR;
	parameters._OpacityComponent._Color = Color(Vector4<float32>(1.0f, 1.0f, 1.0f, 1.0f));
	parameters._EmissiveMultiplier = 0.0f;
	parameters._DoubleSided = false;

	//Open the input file.
	std::ifstream input_file{ compile_data->_FilePath.Data() };

	//Iterate over the lines and fill in the parameters.
	{
		StaticArray<DynamicString, 8> arguments;
		std::string current_line;

		while (std::getline(input_file, current_line))
		{
			//Is this a type declaration?
			{
				const size_t position{ current_line.find("Type(") };

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

					ASSERT(number_of_arguments == 1, "Type() needs one argument!");

					if (arguments[0] == "MASKED")
					{
						parameters._Type = MaterialAsset::Type::MASKED;
					}

					else if (arguments[0] == "OPAQUE")
					{
						parameters._Type = MaterialAsset::Type::OPAQUE;
					}

					else if (arguments[0] == "TRANSLUCENT")
					{
						parameters._Type = MaterialAsset::Type::TRANSLUCENT;
					}

					else
					{
						ASSERT(false, "Unknown argument %s", arguments[0].Data());
					}

					continue;
				}
			}

			//Is this an albedo/thickness color declaration?
			{
				const size_t position{ current_line.find("AlbedoThicknessColor(") };

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

					ASSERT(number_of_arguments == 4, "AlbedoThicknessColor() needs one argument!");

					parameters._AlbedoThicknessComponent._Type = MaterialParameters::Component::Type::COLOR;
					parameters._AlbedoThicknessComponent._Color = Color
					(
						Vector4<float32>
						(
							std::stof(arguments[0].Data()),
							std::stof(arguments[1].Data()),
							std::stof(arguments[2].Data()),
							std::stof(arguments[3].Data())
						)
					);
					parameters._AlbedoThicknessComponent._Color.ApplyGammaCorrection();

					continue;
				}
			}

			//Is this an albedo/thickness texture declaration?
			{
				const size_t position{ current_line.find("AlbedoThicknessTexture(") };

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

					ASSERT(number_of_arguments == 1, "AlbedoThicknessTexture() needs one argument!");

					parameters._AlbedoThicknessComponent._Type = MaterialParameters::Component::Type::TEXTURE;
					parameters._AlbedoThicknessComponent._Texture = HashString(arguments[0].Data());

					continue;
				}
			}

			//Is this an albedo/thickness average color of declaration?
			{
				const size_t position{ current_line.find("AlbedoThicknessAverageColorOf(") };

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

					ASSERT(number_of_arguments == 2, "AlbedoThicknessAverageColorOf() needs two arguments!");

					parameters._AlbedoThicknessComponent._Type = MaterialParameters::Component::Type::COLOR;

					Vector3<float32> average_color{ 0.0f, 0.0f, 0.0f };

					{
						Texture2D<Vector4<float32>> texture;

						const File::Extension file_extension{ File::GetExtension(arguments[0].Data()) };

						switch (file_extension)
						{
							case File::Extension::JPG:
							{
								JPGReader::Read(arguments[0].Data(), &texture);

								break;
							}

							case File::Extension::PNG:
							{
								PNGReader::Read(arguments[0].Data(), &texture);

								break;
							}

							default:
							{
								ASSERT(false, "Invalid case!");

								break;
							}
						}

						for (uint32 Y{ 0 }; Y < texture.GetHeight(); ++Y)
						{
							for (uint32 X{ 0 }; X < texture.GetWidth(); ++X)
							{
								const Vector4<float32> &texel{ texture.At(X, Y) };

								average_color._R += texel._R;
								average_color._G += texel._G;
								average_color._B += texel._B;
							}
						}

						average_color /= static_cast<float32>(texture.GetWidth() * texture.GetHeight());

					}

					const float32 thickness{ std::stof(arguments[1].Data()) };

					parameters._AlbedoThicknessComponent._Color = Color(Vector4<float32>(average_color, thickness));
					parameters._AlbedoThicknessComponent._Color.ApplyGammaCorrection();

					continue;
				}
			}

			//Is this a normal map/displacement texture declaration?
			{
				const size_t position{ current_line.find("NormalMapDisplacementTexture(") };

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

					ASSERT(number_of_arguments == 1, "NormalMapDisplacementTexture() needs one argument!");

					parameters._NormalMapDisplacementComponent._Type = MaterialParameters::Component::Type::TEXTURE;
					parameters._NormalMapDisplacementComponent._Texture = HashString(arguments[0].Data());

					continue;
				}
			}

			//Is this a material properties color declaration?
			{
				const size_t position{ current_line.find("MaterialPropertiesColor(") };

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

					ASSERT(number_of_arguments == 4, "AlbedoThicknessColor() needs one argument!");

					parameters._MaterialPropertiesComponent._Type = MaterialParameters::Component::Type::COLOR;
					parameters._MaterialPropertiesComponent._Color = Color
					(
						Vector4<float32>
						(
							std::stof(arguments[0].Data()),
							std::stof(arguments[1].Data()),
							std::stof(arguments[2].Data()),
							std::stof(arguments[3].Data())
						)
					);

					continue;
				}
			}

			//Is this a material properties texture declaration?
			{
				const size_t position{ current_line.find("MaterialPropertiesTexture(") };

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

					ASSERT(number_of_arguments == 1, "MaterialPropertiesTexture() needs one argument!");

					parameters._MaterialPropertiesComponent._Type = MaterialParameters::Component::Type::TEXTURE;
					parameters._MaterialPropertiesComponent._Texture = HashString(arguments[0].Data());

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

					ASSERT(number_of_arguments == 1, "OpacityTexture() needs one argument!");

					parameters._OpacityComponent._Type = MaterialParameters::Component::Type::TEXTURE;
					parameters._OpacityComponent._Texture = HashString(arguments[0].Data());

					continue;
				}
			}

			//Is this an emissive multiplier declaration?
			{
				const size_t position{ current_line.find("EmissiveMultiplier(") };

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

					ASSERT(number_of_arguments == 1, "EmissiveMultiplier() needs one argument!");

					parameters._EmissiveMultiplier = std::stof(arguments[0].Data());

					continue;
				}
			}

			//Is this a double sided declaration?
			{
				const size_t position{ current_line.find("DoubleSided()") };

				if (position != std::string::npos)
				{
					parameters._DoubleSided = true;

					continue;
				}
			}

			//Couldn't figure out what this line is?
			ASSERT(false, "Unknown line %s", current_line.c_str());
		}
	}

	//Close the input file.
	input_file.close();

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
	BinaryOutputFile output_file{ output_file_path };

	//Write the asset header to the file.
	AssetHeader asset_header{ AssetTypeIdentifier(), CurrentVersion(), HashString(compile_data->_Name.Data()), compile_data->_Name.Data() };
	output_file.Write(&asset_header, sizeof(AssetHeader));

	//Just write the parameters.
	output_file.Write(&parameters, sizeof(MaterialParameters));

	//Close the output file.
	output_file.Close();
}

/*
*	Loads internally.
*/
void MaterialAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("MaterialAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the parameters.
	MaterialParameters parameters;
	load_data->_StreamArchive->Read(&parameters, sizeof(MaterialParameters), &stream_archive_position);

	//Apply all parameters.
	load_data->_Asset->_Index = RenderingSystem::Instance->GetMaterialSystem()->RegisterMaterial(load_data->_Asset);
	load_data->_Asset->_Type = parameters._Type;
	load_data->_Asset->_AlbedoThicknessComponent._Type = static_cast<MaterialAsset::Component::Type>(parameters._AlbedoThicknessComponent._Type);

	if (parameters._AlbedoThicknessComponent._Type == MaterialParameters::Component::Type::COLOR)
	{
		load_data->_Asset->_AlbedoThicknessComponent._Color = parameters._AlbedoThicknessComponent._Color;
	}

	else
	{
		PostLinkData post_link_data;

		post_link_data._Asset = &load_data->_Asset->_AlbedoThicknessComponent._Texture;
		post_link_data._Identifier = parameters._AlbedoThicknessComponent._Texture;

		_PostLinkQueue.Push(post_link_data);
	}

	load_data->_Asset->_NormalMapDisplacementComponent._Type = static_cast<MaterialAsset::Component::Type>(parameters._NormalMapDisplacementComponent._Type);

	if (parameters._NormalMapDisplacementComponent._Type == MaterialParameters::Component::Type::COLOR)
	{
		load_data->_Asset->_NormalMapDisplacementComponent._Color = parameters._NormalMapDisplacementComponent._Color;
	}

	else
	{
		PostLinkData post_link_data;

		post_link_data._Asset = &load_data->_Asset->_NormalMapDisplacementComponent._Texture;
		post_link_data._Identifier = parameters._NormalMapDisplacementComponent._Texture;

		_PostLinkQueue.Push(post_link_data);
	}

	load_data->_Asset->_MaterialPropertiesComponent._Type = static_cast<MaterialAsset::Component::Type>(parameters._MaterialPropertiesComponent._Type);

	if (parameters._MaterialPropertiesComponent._Type == MaterialParameters::Component::Type::COLOR)
	{
		load_data->_Asset->_MaterialPropertiesComponent._Color = parameters._MaterialPropertiesComponent._Color;
	}

	else
	{
		PostLinkData post_link_data;

		post_link_data._Asset = &load_data->_Asset->_MaterialPropertiesComponent._Texture;
		post_link_data._Identifier = parameters._MaterialPropertiesComponent._Texture;

		_PostLinkQueue.Push(post_link_data);
	}

	load_data->_Asset->_OpacityComponent._Type = static_cast<MaterialAsset::Component::Type>(parameters._OpacityComponent._Type);

	if (parameters._OpacityComponent._Type == MaterialParameters::Component::Type::COLOR)
	{
		load_data->_Asset->_OpacityComponent._Color = parameters._OpacityComponent._Color;
	}

	else
	{
		PostLinkData post_link_data;

		post_link_data._Asset = &load_data->_Asset->_OpacityComponent._Texture;
		post_link_data._Identifier = parameters._OpacityComponent._Texture;

		_PostLinkQueue.Push(post_link_data);
	}

	load_data->_Asset->_EmissiveMultiplier = parameters._EmissiveMultiplier;
	load_data->_Asset->_DoubleSided = parameters._DoubleSided;
}