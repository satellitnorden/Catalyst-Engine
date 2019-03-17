//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/BinaryFile.h>
#include <Core/Utilities/MemoryUtilities.h>

//STL.
#include <filesystem>

//Preprocessor defines.
#if !defined EXIT_SUCCESS
	#define EXIT_SUCCESS 0
#endif

int main()
{
	BinaryFile<IOMode::Out> output{ "CatalystShaderCollection.csc" };

	//Iterate over all files in the directory and write them to the output file.
	for (auto iterator : std::experimental::filesystem::directory_iterator{ std::experimental::filesystem::current_path() })
	{
		std::string fileName{ iterator.path().filename().generic_string() };

		if (fileName.find(".spv") != std::string::npos)
		{
			BinaryFile<IOMode::In> shader{ fileName.c_str() };

			//Write the shader size.
			const uint64 shaderSize{ shader.Size() };
			output.Write(&shaderSize, sizeof(uint64));

			//Write the shader data.
			void *const RESTRICT shaderData{ MemoryUtilities::AllocateMemory(shaderSize) };
			shader.Read(shaderData, shaderSize);

			output.Write(shaderData, shaderSize);

			MemoryUtilities::FreeMemory(shaderData);

			shader.Close();
		}
	}

	output.Close();

	return EXIT_SUCCESS;
}