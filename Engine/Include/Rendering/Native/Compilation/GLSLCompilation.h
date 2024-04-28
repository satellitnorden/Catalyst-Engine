#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/StringUtilities.h>

//File.
#include <File/Utilities/TextParsingUtilities.h>

//STL.
#include <fstream>
#include <string>

namespace GLSLCompilation
{

	/*
	*	Returns the byte offset for the given type.
	*/
	FORCE_INLINE NO_DISCARD uint64 GetByteOffsetForType(const char *const RESTRICT type_string) NOEXCEPT
	{
		if (StringUtilities::IsEqual(type_string, "mat4"))
		{
			return 64;
		}

		if (StringUtilities::IsEqual(type_string, "mat4x3"))
		{
			return 48;
		}

		if (StringUtilities::IsEqual(type_string, "mat3x4"))
		{
			return 48;
		}

		if (StringUtilities::IsEqual(type_string, "vec4"))
		{
			return 16;
		}

		if (StringUtilities::IsEqual(type_string, "vec3"))
		{
			return 16;
		}

		if (StringUtilities::IsEqual(type_string, "vec2"))
		{
			return 8;
		}

		if (StringUtilities::IsEqual(type_string, "float"))
		{
			return 4;
		}

		if (StringUtilities::IsEqual(type_string, "int"))
		{
			return 4;
		}

		if (StringUtilities::IsEqual(type_string, "uint"))
		{
			return 4;
		}

		ASSERT(false, "Unknown type!");

		return 0;
	}

	/*
	*	Returns the location offset for the given type.
	*/
	FORCE_INLINE NO_DISCARD uint32 GetLocationOffsetForType(const char *const RESTRICT type_string) NOEXCEPT
	{
		if (StringUtilities::IsEqual(type_string, "mat4"))
		{
			return 4;
		}

		if (StringUtilities::IsEqual(type_string, "mat3"))
		{
			return 3;
		}

		if (StringUtilities::IsEqual(type_string, "vec4"))
		{
			return 1;
		}

		if (StringUtilities::IsEqual(type_string, "vec3"))
		{
			return 1;
		}

		if (StringUtilities::IsEqual(type_string, "vec2"))
		{
			return 1;
		}

		if (StringUtilities::IsEqual(type_string, "float"))
		{
			return 1;
		}

		if (StringUtilities::IsEqual(type_string, "uint"))
		{
			return 1;
		}

		ASSERT(false, "Unknown type!");

		return 0;
	}

	/*
	*	Inserts extensions into the given file.
	*/
	FORCE_INLINE void InsertExtensions(std::ofstream &output_file) NOEXCEPT
	{
		output_file << "#extension GL_ARB_separate_shader_objects : require" << std::endl;
		output_file << "#extension GL_EXT_nonuniform_qualifier : require" << std::endl;
	}

	/*
	*	Inserts ray tracing extensions into the given file.
	*/
	FORCE_INLINE void InsertRayTracingExtensions(std::ofstream &output_file) NOEXCEPT
	{
		output_file << "#extension GL_NV_ray_tracing : require" << std::endl;
	}

	/*
	*	Insert the contents of a file into the given file.
	*/
	FORCE_INLINE void InsertFromFile
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

		output_file << std::endl;

		//Close the input file.
		input_file.close();
	}

	/*
	*	Inserts a buffer definition into the given output file from the given input file.
	*	Can be either a uniform buffer or a storage buffer.
	*/
	FORCE_INLINE void InsertBufferDefinition
	(
		std::ofstream &output_file,
		const char *const RESTRICT input_file_path,
		const uint32 binding
	) NOEXCEPT
	{
		//Enumeration covering all buffer types.
		enum class BufferType : uint8
		{
			UNKNOWN,
			UNIFORM,
			STORAGE
		};

		//Open the input file.
		std::ifstream input_file{ input_file_path };

		//Go through all the lines.
		BufferType buffer_type{ BufferType::UNKNOWN };
		std::string current_line;
		uint64 current_offset{ 0 };

		while (std::getline(input_file, current_line))
		{
			//Ignore if only whitespace.
			if (TextParsingUtilities::OnlyWhitespace(current_line.data(), current_line.length()))
			{
				continue;
			}

			//Is this a buffer declaration?
			if (buffer_type == BufferType::UNKNOWN)
			{
				{
					const size_t position{ current_line.find("UniformBuffer(") };

					if (position != std::string::npos)
					{
						buffer_type = BufferType::UNIFORM;

						DynamicString name;

						TextParsingUtilities::ParseFunctionArguments(current_line.data(), current_line.length(), &name);

						output_file << "layout (std140, set = 1, binding = " << binding << ") uniform " << name.Data() << std::endl;

						continue;
					}
				}

				const size_t position{ current_line.find("StorageBuffer(") };

				if (position != std::string::npos)
				{
					buffer_type = BufferType::STORAGE;

					DynamicString name;

					TextParsingUtilities::ParseFunctionArguments(current_line.data(), current_line.length(), &name);

					output_file << "layout (std430, set = 1, binding = " << binding << ") buffer " << name.Data() << std::endl;

					continue;
				}
			}

			//Are we currently parsing a buffer.
			if (buffer_type != BufferType::UNKNOWN)
			{
				//If this is the opening bracket, just insert it.
				if (current_line[0] == '{')
				{
					output_file << current_line << std::endl;

					continue;
				}

				//If this is the closing bracket, insert it and stop parsing the buffer.
				if (current_line[0] == '}')
				{
					output_file << current_line << std::endl;
					buffer_type = BufferType::UNKNOWN;

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

					//Figure out if this is an array.
					uint64 array_count{ 0 };

					{
						const size_t array_start_position{ current_line.find("[") };

						if (array_start_position != std::string::npos)
						{
							const size_t array_end_position{ current_line.find("]") };

							const size_t array_position_length{ array_end_position - array_start_position };

							if (array_position_length > 1)
							{
								const std::string array_count_string{ current_line.substr(array_start_position + 1, array_position_length - 1) };

								array_count = std::stoull(array_count_string.c_str());
							}
						}
					}

					//Update the current offset. Assume 16 bytes for each array element if this is an array.
					if (array_count > 0)
					{
						current_offset += 16 * array_count;
					}

					else
					{
						if (buffer_type == BufferType::STORAGE)
						{
							//Offsets are always 16 for storage buffers.
							current_offset += 16;
						}

						else
						{
							current_offset += GetByteOffsetForType(type_string.data());
						}
					}
				}
			}

			//Otherwise, just insert the line.
			else
			{
				output_file << current_line << std::endl;
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