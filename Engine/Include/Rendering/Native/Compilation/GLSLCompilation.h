#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File.
#include <File/Utilities/TextParsingUtilities.h>

namespace GLSLCompilation
{

	/*
	*	Inserts a uniform buffer definition into the given output file from the given input file.
	*/
	FORCE_INLINE void InsertUniformBufferDefinition
	(
		std::ofstream &output_file,
		const char *const RESTRICT input_file_path,
		const uint32 binding
	) NOEXCEPT
	{
		//Open the input file.
		std::ifstream input_file{ input_file_path };

		//Go through all the lines.
		bool has_parsed_first_line{ false };
		std::string current_line;
		uint32 current_offset{ 0 };

		while (std::getline(input_file, current_line))
		{
			//Ignore if only whitespace.
			if (TextParsingUtilities::OnlyWhitespace(current_line.data(), current_line.length()))
			{
				continue;
			}

			//If this is the first line, insert the declaration.
			if (!has_parsed_first_line)
			{
				output_file << "layout (std140, set = 0, binding = " << binding << ") uniform " << current_line << std::endl;

				has_parsed_first_line = true;

				continue;
			}

			//If it's opening/closing brackets, just insert them.
			if (current_line[0] == '{' || current_line[0] == '}')
			{
				output_file << current_line << std::endl;

				continue;
			}

			//The rest of the lines should be type/name pairs, so split them up.
			std::string type_string;
			std::string name_string;

			{
				size_t type_begin{ 0 };

				for (size_t i{ 0 }; i < current_line.length(); ++i)
				{
					if (TextParsingUtilities::IsWhitespace(current_line[i]))
					{
						++type_begin;
					}

					else
					{
						break;
					}
				}

				size_t type_end{ type_begin };

				for (size_t i{ type_begin }; i < current_line.length(); ++i)
				{
					if (!TextParsingUtilities::IsWhitespace(current_line[i]))
					{
						++type_end;
					}

					else
					{
						break;
					}
				}

				type_string = current_line.substr(type_begin, type_end - type_begin);
				name_string = current_line.substr(type_end + 1);

				output_file << "\tlayout (offset = " << current_offset << ") " << type_string.data() << " " << name_string.data() << std::endl;
			
				//Update the current offset.
				if (type_string == "mat4")
				{
					current_offset += 64;
				}

				else if (type_string == "vec3")
				{
					current_offset += 16;
				}

				else if (type_string == "vec2")
				{
					current_offset += 8;
				}

				else if (type_string == "float")
				{
					current_offset += 4;
				}

				else
				{
					ASSERT(false, "Unknown type!");
				}
			}
		}
	}

	/*
	*	Inserts a shader function library into the given output file from the given input file.
	*/
	FORCE_INLINE void InsertShaderFunctionLibrary
	(
		std::ofstream &output_file,
		const char *const RESTRICT input_file_path
	) NOEXCEPT
	{
		//Open the input file.
		std::ifstream input_file{ input_file_path };

		//Go through all the lines.
		std::string current_line;

		while (std::getline(input_file, current_line))
		{
			output_file << current_line << std::endl;
		}
	}

}