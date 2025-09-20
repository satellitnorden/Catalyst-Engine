//Header file.
#include "RenderPipelineGenerator.h"

//Catalyst code generator.
#include "TextParsing.h"

//Constants.
#define ENGINE_RENDERING "..\\..\\..\\..\\Catalyst-Engine\\Engine\\Rendering"
#define GAME_RENDERING "..\\..\\..\\Rendering"

#define SOURCE_FILE_PATH "..\\..\\..\\Code\\Source\\Generated\\RenderPipelineInformation.Generated.cpp"

/*
*	Runs this render pipeline generator.
*/
void RenderPipelineGenerator::Run()
{
	//Create the "CodeGeneration" directory, if it doesn't exist.
	std::filesystem::create_directory("..\\..\\..\\Code\\CodeGeneration");

	//Set up the JSON object.
	nlohmann::json JSON;

	//Read the cache, if it exists.
//#if defined(NDEBUG)
	if (std::filesystem::exists("..\\..\\..\\Code\\CodeGeneration\\RenderPipelineCache.json"))
	{
		std::ifstream input_file{ "..\\..\\..\\Code\\CodeGeneration\\RenderPipelineCache.json" };
		input_file >> JSON;
		input_file.close();
	}
//#endif

	//Gather render pipelines!
	bool new_files_parsed{ false };

	new_files_parsed |= GatherRenderPipelines(ENGINE_RENDERING, JSON);
	new_files_parsed |= GatherRenderPipelines(GAME_RENDERING, JSON);

	//Generate the source file.
	if (new_files_parsed || !std::filesystem::exists(SOURCE_FILE_PATH))
	{
		GenerateSourceFile(JSON);
	}

	//Write the JSON to file.
	std::ofstream file{ "..\\..\\..\\Code\\CodeGeneration\\RenderPipelineCache.json" };
	file << std::setw(4) << JSON;
	file.close();
}

/*
*	Gathers render pipelines in the given directory.
*/
bool RenderPipelineGenerator::GatherRenderPipelines(const char *const directory_path, nlohmann::json &JSON)
{
	//Ignore if the directory doesn't exist.
	if (!std::filesystem::exists(directory_path))
	{
		return false;
	}

	//Remember if new files were parsed.
	bool new_files_parsed{ false };

	//Iterate over all files.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//Gather recursively if this is a directory.
		if (entry.is_directory())
		{
			new_files_parsed |= GatherRenderPipelines(entry.path().string().c_str(), JSON);

			continue;
		}

		//Cache the file path string.
		const std::string file_path{ entry.path().string() };

		//Figure out the extension.
		std::string extension;

		{
			const size_t last_dot_position{ file_path.find_last_of(".") };
			extension = file_path.substr(last_dot_position + 1, std::string::npos);
		}

		//Check if this is a C++ file.
		if (extension != "render_pipeline")
		{
			continue;
		}

		//Cache the entry.
		nlohmann::json &entry{ JSON[file_path] };

		//Calculate the last write time.
		std::time_t last_write_time;

		{
			const std::filesystem::file_time_type _last_write_time{ std::filesystem::last_write_time(file_path) };
			const std::chrono::time_point<std::chrono::system_clock> time_point{ std::chrono::utc_clock::to_sys(std::chrono::file_clock::to_utc(_last_write_time)) };
			last_write_time = std::chrono::system_clock::to_time_t(time_point);
		}

		//Check if this entry should be parse.
		bool parse_entry{ true };

		if (!entry.is_null())
		{
			nlohmann::json &last_write_time_entry{ entry["LastWriteTime"] };

			if (!last_write_time_entry.is_null())
			{
				parse_entry = last_write_time != last_write_time_entry.get<std::time_t>();
			}
		}

		if (!parse_entry)
		{
			continue;
		}

		//Clear the entry before writing to it.
		entry.clear();

		//Figure out the name.
		std::string name;

		{
			const size_t last_slash_position{ file_path.find_last_of("\\") };
			name = file_path.substr(last_slash_position + 1, file_path.length() - last_slash_position - strlen(extension.c_str()) - 2);
		}

		//Add "_RenderPipeline", for now. (:
		name += "_RenderPipeline";

		entry["Name"] = name.c_str();

		//Set the identifier.
		const uint64 identifier{ CatalystHash(name.c_str(), name.length()) };
		entry["Identifier"] = identifier;

		//Write the last write time.
		entry["LastWriteTime"] = last_write_time;

		//Parse the file.
		std::ifstream file{ file_path.c_str() };
		ParseRenderPipeline(file, entry);
		file.close();

		//New files were parsed!
		new_files_parsed = true;
	}

	return new_files_parsed;
}

/*
*	Parses a user interface scene.
*/
void RenderPipelineGenerator::ParseRenderPipeline(std::ifstream &file, nlohmann::json &JSON)
{
	//Iterate through all lines.
	std::string current_line;

	while (std::getline(file, current_line))
	{
		//Ignore comments and whitespace.
		if (OnlyComment(current_line.c_str(), current_line.length())
			|| OnlyWhitespace(current_line.c_str(), current_line.length()))
		{
			continue;
		}

		//Check if this is an output depth buffer declaration.
		{
			const size_t position{ current_line.find("OutputDepthBuffer(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["OutputDepthBuffer"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a render resolution declaration.
		{
			const size_t position{ current_line.find("RenderResolution(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["RenderResolution"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a color load operator declaration.
		{
			const size_t position{ current_line.find("ColorLoadOperator(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["ColorLoadOperator"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a color store operator declaration.
		{
			const size_t position{ current_line.find("ColorStoreOperator(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["ColorStoreOperator"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a depth/stencil load operator declaration.
		{
			const size_t position{ current_line.find("DepthStencilLoadOperator(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["DepthStencilLoadOperator"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a depth/stencil store operator declaration.
		{
			const size_t position{ current_line.find("DepthStencilStoreOperator(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["DepthStencilStoreOperator"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a polygon mode declaration.
		{
			const size_t position{ current_line.find("PolygonMode(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["PolygonMode"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a cull mode declaration.
		{
			const size_t position{ current_line.find("CullMode(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["CullMode"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a blend enable declaration.
		{
			const size_t position{ current_line.find("BlendEnable(") };

			if (position != std::string::npos)
			{
				JSON["BlendEnabled"] = true;

				continue;
			}
		}

		//Check if this is a blend color source factor declaration.
		{
			const size_t position{ current_line.find("BlendColorSourceFactor(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["BlendColorSourceFactor"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a blend color destination factor declaration.
		{
			const size_t position{ current_line.find("BlendColorDestinationFactor(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["BlendColorDestinationFactor"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a blend color color declaration.
		{
			const size_t position{ current_line.find("BlendColorOperator(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["BlendColorOperator"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a blend alpha source factor declaration.
		{
			const size_t position{ current_line.find("BlendAlphaSourceFactor(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["BlendAlphaSourceFactor"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a blend alpha destination factor declaration.
		{
			const size_t position{ current_line.find("BlendAlphaDestinationFactor(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["BlendAlphaDestinationFactor"] = arguments[0].c_str();

				continue;
			}
		}

		//Check if this is a blend alpha color declaration.
		{
			const size_t position{ current_line.find("BlendAlphaOperator(") };

			if (position != std::string::npos)
			{
				//Parse the function arguments.
				std::array<std::string, 1> arguments;

				const uint64 number_of_arguments
				{
					TextParsing::ParseFunctionArguments
					(
						current_line.c_str(),
						current_line.length(),
						arguments.data()
					)
				};

				JSON["BlendAlphaOperator"] = arguments[0].c_str();

				continue;
			}
		}
	}
}

/*
*	Generates the source file.
*/
void RenderPipelineGenerator::GenerateSourceFile(const nlohmann::json &JSON)
{
	//Figure out the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "..\\..\\..\\Code\\Source\\Generated");

	//Create the directories, if it doesn't exist.
	std::filesystem::create_directory(directory_path);

	//Figure out the file path.
	char file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(file_path, "%s\\RenderPipelineInformation.Generated.cpp", directory_path);

	//Open the file.
	std::ofstream file{ file_path };

	//Add header stuff.
	file << "//Header file" << std::endl;
	file << "#include <Rendering/Native/RenderPipelineInformation.h>" << std::endl;
	file << std::endl;

	//Add the "RetrieveGraphicsRenderPipelineInformation()" function.
	file << "void RenderPipelineInformation::RetrieveGraphicsRenderPipelineInformation(const HashString identifier, GraphicsRenderPipelineInformation *const RESTRICT information) NOEXCEPT" << std::endl;
	file << "{" << std::endl;
	file << "\tswitch (identifier)" << std::endl;
	file << "\t{" << std::endl;

	for (const nlohmann::json &entry : JSON)
	{
		file << "\t\tcase " << entry["Identifier"].get<uint64>() << ": /*" << entry["Name"].get<std::string>().c_str() << "*/" << std::endl;
		file << "\t\t{" << std::endl;

		if (entry.contains("OutputDepthBuffer"))
		{
			const std::string output_depth_buffer{ entry["OutputDepthBuffer"].get<std::string>() };
			const uint64 identifier{ CatalystHash(output_depth_buffer.c_str(), output_depth_buffer.length()) };

			file << "\t\t\tinformation->_OutputDepthBuffer = " << identifier << "; /*" << output_depth_buffer.c_str() << "*/" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_OutputDepthBuffer = HashString();" << std::endl;
		}

		if (entry.contains("RenderResolution"))
		{
			const std::string render_resolution{ entry["RenderResolution"].get<std::string>() };
			const uint64 identifier{ CatalystHash(render_resolution.c_str(), render_resolution.length()) };

			file << "\t\t\tinformation->_RenderResolution = " << identifier << "; /*" << render_resolution.c_str() << "*/" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_RenderResolution = HashString();" << std::endl;
		}

		if (entry.contains("ColorLoadOperator"))
		{
			file << "\t\t\tinformation->_ColorLoadOperator = AttachmentLoadOperator::" << entry["ColorLoadOperator"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_ColorLoadOperator = AttachmentLoadOperator::DONT_CARE;" << std::endl;
		}

		if (entry.contains("ColorStoreOperator"))
		{
			file << "\t\t\tinformation->_ColorStoreOperator = AttachmentStoreOperator::" << entry["ColorStoreOperator"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_ColorStoreOperator = AttachmentStoreOperator::DONT_CARE;" << std::endl;
		}

		if (entry.contains("DepthStencilLoadOperator"))
		{
			file << "\t\t\tinformation->_DepthStencilLoadOperator = AttachmentLoadOperator::" << entry["DepthStencilLoadOperator"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_DepthStencilLoadOperator = AttachmentLoadOperator::DONT_CARE;" << std::endl;
		}

		if (entry.contains("DepthStencilStoreOperator"))
		{
			file << "\t\t\tinformation->_DepthStencilStoreOperator = AttachmentStoreOperator::" << entry["DepthStencilStoreOperator"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_DepthStencilStoreOperator = AttachmentStoreOperator::DONT_CARE;" << std::endl;
		}

		if (entry.contains("PolygonMode"))
		{
			file << "\t\t\tinformation->_PolygonMode = PolygonMode::" << entry["PolygonMode"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_PolygonMode = PolygonMode::FILL;" << std::endl;
		}

		if (entry.contains("CullMode"))
		{
			file << "\t\t\tinformation->_CullMode = CullMode::" << entry["CullMode"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_CullMode = CullMode::NONE;" << std::endl;
		}

		if (entry.contains("BlendEnabled"))
		{
			file << "\t\t\tinformation->_BlendEnabled = true;" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_BlendEnabled = false;" << std::endl;
		}

		if (entry.contains("BlendColorSourceFactor"))
		{
			file << "\t\t\tinformation->_BlendColorSourceFactor = BlendFactor::" << entry["BlendColorSourceFactor"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_BlendColorSourceFactor = BlendFactor::ZERO;" << std::endl;
		}

		if (entry.contains("BlendColorDestinationFactor"))
		{
			file << "\t\t\tinformation->_BlendColorDestinationFactor = BlendFactor::" << entry["BlendColorDestinationFactor"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_BlendColorDestinationFactor = BlendFactor::ZERO;" << std::endl;
		}

		if (entry.contains("BlendColorOperator"))
		{
			file << "\t\t\tinformation->_BlendColorOperator = BlendOperator::" << entry["BlendColorOperator"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_BlendColorOperator = BlendOperator::ADD;" << std::endl;
		}

		if (entry.contains("BlendAlphaSourceFactor"))
		{
			file << "\t\t\tinformation->_BlendAlphaSourceFactor = BlendFactor::" << entry["BlendAlphaSourceFactor"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_BlendAlphaSourceFactor = BlendFactor::ZERO;" << std::endl;
		}

		if (entry.contains("BlendAlphaDestinationFactor"))
		{
			file << "\t\t\tinformation->_BlendAlphaDestinationFactor = BlendFactor::" << entry["BlendAlphaDestinationFactor"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_BlendAlphaDestinationFactor = BlendFactor::ZERO;" << std::endl;
		}

		if (entry.contains("BlendAlphaOperator"))
		{
			file << "\t\t\tinformation->_BlendAlphaOperator = BlendOperator::" << entry["BlendAlphaOperator"].get<std::string>().c_str() << ";" << std::endl;
		}

		else
		{
			file << "\t\t\tinformation->_BlendAlphaOperator = BlendOperator::ADD;" << std::endl;
		}

		file << "\t\t\tbreak;" << std::endl;
		file << "\t\t}" << std::endl;
	}

	file << "\t\tdefault:" << std::endl;
	file << "\t\t{" << std::endl;
	file << "\t\t\tASSERT(false, \"Invalid case!\");" << std::endl;
	file << "\t\t\tbreak;" << std::endl;
	file << "\t\t}" << std::endl;

	file << "\t}" << std::endl;
	file << "}";

	//Close the file.
	file.close();
}