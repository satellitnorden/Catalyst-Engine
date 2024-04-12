#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

namespace TextParsingUtilities
{

	/*
	*	Returns if the given character is whitespace.
	*/
	FORCE_INLINE NO_DISCARD bool IsWhitespace(const char character) NOEXCEPT
	{
		//Treat both spaces and tabs as whitespace.
		return character == ' ' || character == '\t';
	}

	/*
	*	Returns if a string only has whitespace.
	*/
	FORCE_INLINE NO_DISCARD bool OnlyWhitespace(const char *const RESTRICT string, const uint64 string_length) NOEXCEPT
	{
		for (uint64 i{ 0 }; i < string_length; ++i)
		{
			if (!IsWhitespace(string[i]))
			{
				return false;
			}
		}

		return true;
	}

	/*
	*	Returns if a string only has comments.
	*/
	FORCE_INLINE NO_DISCARD bool OnlyComment(const char *const RESTRICT string, const uint64 string_length) NOEXCEPT
	{
		bool seen_first_slash{ false };

		for (uint64 i{ 0 }; i < string_length; ++i)
		{
			//Ignore whitespace.
			if (IsWhitespace(string[i]))
			{
				continue;
			}

			//Is this a slash.
			const bool is_slash{ string[i] == '/' };

			if (is_slash && !seen_first_slash)
			{
				seen_first_slash = true;
			}

			else if (is_slash && seen_first_slash)
			{
				return true;
			}

			else
			{
				return false;
			}
		}

		return false;
	}

	/*
	*	Parses function arguments in the given output array.
	*	For example, if the string is "Function(Argument_1, Argument_2);",
	*	the given output array would be filled with the strings "Argument_1" and "Argument_2".
	*	Returns the number of arguments parsed
	*/
	FORCE_INLINE uint64 ParseFunctionArguments
	(
		const char *const RESTRICT string,
		const uint64 string_length,
		DynamicString *const RESTRICT output_arguments,
		uint64 *const RESTRICT end_position = nullptr
	) NOEXCEPT
	{
		char buffer[128];
		uint64 current_buffer_index{ 0 };
		uint64 current_argument_index{ 0 };
		bool currently_parsing_argument{ false };
		uint32 parantheses_depth{ 0 };
		bool is_in_comment{ false };

		for (uint64 i{ 0 }; i < string_length; ++i)
		{
			//Update if is currently in a comment.
			if (i > 1 && string[i - 1] == '/' && string[i] == '*')
			{
				is_in_comment = true;
			}

			else if (i > 1 && string[i - 1] == '*' && string[i] == '/')
			{
				is_in_comment = false;
			}

			if (is_in_comment)
			{
				continue;
			}

			//Check if we should stop parsing the argument.
			if (currently_parsing_argument && (string[i] == ',' || string[i] == ')') && parantheses_depth == 1)
			{
				//Write the argument.
				buffer[current_buffer_index] = '\0';
				output_arguments[current_argument_index] = buffer;

				//Reset values.
				current_buffer_index = 0;
				++current_argument_index;
				currently_parsing_argument = false;
			}

			if (string[i] == ')')
			{
				--parantheses_depth;

				if (end_position && parantheses_depth == 0)
				{
					*end_position = i;
				}
			}

			//Add to the buffer if we're currently parsing an argument.
			if (currently_parsing_argument && !IsWhitespace(string[i]) && string[i] != '/')
			{
				buffer[current_buffer_index++] = string[i];
			}

			//See if we should start parsing the first argument.
			if (string[i] == '(')
			{
				if (!currently_parsing_argument)
				{
					currently_parsing_argument = true;
					parantheses_depth = 1;
				}
				
				else
				{
					++parantheses_depth;
				}
			}

			if (string[i] == ',' && parantheses_depth == 1)
			{
				currently_parsing_argument = true;
			}
		}

		return current_argument_index;
	}

	/*
	*	Parses space separated arguments in the given output array.
	*	For example, if the string is "ARGUMENT_1 ARGUMENT_2",
	*	the given output array would be filled with the strings "ARGUMENT_1" and "ARGUMENT_2".
	*/
	FORCE_INLINE NO_DISCARD uint64 ParseSpaceSeparatedArguments
	(
		const char *const RESTRICT string,
		const uint64 string_length,
		DynamicString *const RESTRICT output_arguments
	) NOEXCEPT
	{
		char buffer[128];
		uint64 current_buffer_index{ 0 };
		uint64 current_argument_index{ 0 };
		bool currently_parsing_argument{ false };

		for (uint64 i{ 0 }; i < string_length; ++i)
		{
			//See if we should start parsing the argument.
			if (!currently_parsing_argument && !IsWhitespace(string[i]))
			{
				currently_parsing_argument = true;
			}

			//Check if we should stop parsing the argument.
			if (currently_parsing_argument && IsWhitespace(string[i]))
			{
				//Write the argument.
				buffer[current_buffer_index] = '\0';
				output_arguments[current_argument_index] = buffer;

				//Reset values.
				current_buffer_index = 0;
				++current_argument_index;
				currently_parsing_argument = false;
			}

			//Add to the buffer if we're currently parsing an argument.
			if (currently_parsing_argument)
			{
				buffer[current_buffer_index++] = string[i];
			}
		}

		//Write the last argument.
		buffer[current_buffer_index] = '\0';
		output_arguments[current_argument_index] = buffer;

		return current_argument_index + 1;
	}

}