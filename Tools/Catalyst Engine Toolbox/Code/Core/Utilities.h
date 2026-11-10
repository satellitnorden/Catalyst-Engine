#pragma once

//Core.
#include <Core/Core.h>

//STD.
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>

//Prints a buffer. (:
#define PRINT_BUFFER(BUFFER, FORMAT, ...)													\
do																							\
{																							\
	const int32 characters_written{snprintf(BUFFER, sizeof(BUFFER), FORMAT, __VA_ARGS__) };	\
	(void)characters_written;																\
} while (false)


namespace Utilities
{

	/*
	*	Changes backward slashes to forward slashes.
	*/
	static std::string BackwardToForwardSlashes(const std::string &string)
	{
		std::string copy{ string };
		std::replace_if(copy.begin(), copy.end(), [](char character) { return character == '\\'; }, '/');
		return copy;
	}

	/*
	*	Changes forward slashes to backward slashes.
	*/
	static std::string ForwardToBackwardSlashes(const std::string& string)
	{
		std::string copy{ string };
		std::replace_if(copy.begin(), copy.end(), [](char character) { return character == '/'; }, '\\');
		return copy;
	}

	/*
	*	Removes spaces.
	*/
	static std::string RemoveSpaces(const std::string &string)
	{
		std::string copy{ string };
		copy.erase(std::remove_if(copy.begin(), copy.end(), [](unsigned char x) { return std::isspace(x); }), copy.end());
		return copy;
	}

	/*
	*	Strips the extension.
	*/
	static std::string StripExtension(const std::string &string)
	{
		const size_t last_dot_position{ string.find_last_of('.') };

		if (last_dot_position != std::string::npos)
		{
			return string.substr(0, last_dot_position);
		}

		else
		{
			return string;
		}
	}

	/*
	*	Converts the given string to upper case.
	*/
	static std::string ToUpperCase(const std::string &string)
	{
		std::string copy{ string };

		for (char &character : copy)
		{
			character = std::toupper(character);
		}

		return copy;
	}

}