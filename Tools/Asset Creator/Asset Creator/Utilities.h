#pragma once

//STL.
#include <fstream>

namespace Utilities
{

	/*
	*	Given a file
	*/
	void GenerateMipmaps(const unsigned int numberOfMipmapLevels, const unsigned char *data, const size_t dataLength, std::ofstream &outputFile) noexcept
	{
		for (unsigned int i = 0; i < numberOfMipmapLevels; ++i)
		{
			outputFile.write(reinterpret_cast<const char*>(data), dataLength >> i);
		}
	}
}