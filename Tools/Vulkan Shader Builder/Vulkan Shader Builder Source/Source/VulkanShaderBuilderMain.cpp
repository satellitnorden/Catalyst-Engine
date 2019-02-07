//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/BinaryFile.h>

//STL.
#include <filesystem>

//Preprocessor defines.
#if !defined EXIT_SUCCESS
	#define EXIT_SUCCESS 0
#endif

int main()
{
	std::ofstream masterOutput{ "..\\..\\Include\\Rendering\\ShaderData\\Vulkan\\VulkanShaderData.h" };

	masterOutput << "/*" << std::endl;
	masterOutput << "*\tThis file is auto-generated, do not edit manually." << std::endl;
	masterOutput << "*/" << std::endl;
	masterOutput << std::endl;
	masterOutput << "#pragma once" << std::endl;
	masterOutput << std::endl;
	masterOutput << "//Core." << std::endl;
	masterOutput << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
	masterOutput << std::endl;
	masterOutput << "//Rendering." << std::endl;
	 
	//Iterate over all files in the directory and export them into a C++ header file.
	for (auto iterator : std::experimental::filesystem::directory_iterator{ std::experimental::filesystem::current_path() })
	{
		std::string fileName{ iterator.path().filename().generic_string() };

		if (fileName.find(".spv") != std::string::npos)
		{
			BinaryFile<IOMode::In> file{ fileName.c_str() };

			std::string outputFileName{ iterator.path().stem().string() };
			std::string outputHeaderFileName = "..\\..\\Include\\Rendering\\ShaderData\\Vulkan\\" + outputFileName + ".h";
			std::string outputSourceFileName = "..\\..\\Source\\Rendering\\ShaderData\\Vulkan\\" + outputFileName + ".cpp";
			std::ofstream headerOutput{ outputHeaderFileName.c_str() };
			std::ofstream sourceOutput{ outputSourceFileName.c_str() };

			masterOutput << "#include <Rendering/ShaderData/Vulkan/" << outputFileName << ".h>" << std::endl;

			headerOutput << "/*" << std::endl;
			headerOutput << "*\tThis file is auto-generated, do not edit manually." << std::endl;
			headerOutput << "*/" << std::endl;
			headerOutput << std::endl;
			headerOutput << "#pragma once" << std::endl;
			headerOutput << std::endl;
			headerOutput << "//Core." << std::endl;
			headerOutput << "#include <Core/Essential/CatalystEssential.h>" << std::endl;
			headerOutput << "#include <Core/Containers/DynamicArray.h>" << std::endl;
			headerOutput << std::endl;
			headerOutput << "namespace VulkanShaderData" << std::endl;
			headerOutput << "{" << std::endl;
			headerOutput << "\tvoid Get" << outputFileName.c_str() << "Data(DynamicArray<byte> &data);" << std::endl;
			headerOutput << "}" << std::endl;
			headerOutput.close();

			sourceOutput << "/*" << std::endl;
			sourceOutput << "*\tThis file is auto-generated, do not edit manually." << std::endl;
			sourceOutput << "*/" << std::endl;
			sourceOutput << std::endl;
			sourceOutput << "//Header file." << std::endl;
			sourceOutput << "#include <Rendering/ShaderData/Vulkan/" << outputFileName << ".h>" << std::endl;
			sourceOutput << std::endl;
			sourceOutput << "#pragma optimize(\"\", off)" << std::endl;
			sourceOutput << std::endl;
			sourceOutput << "namespace VulkanShaderData" << std::endl;
			sourceOutput << "{" << std::endl;
			sourceOutput << "\tvoid Get" << outputFileName.c_str() << "Data(DynamicArray<byte> &data)" << std::endl;
			sourceOutput << "\t{" << std::endl;
			sourceOutput << "\t\tdata.UpsizeFast(" << file.Size() << ");" << std::endl;
			sourceOutput << std::endl;
			sourceOutput << "\t\tnew (data.Data()) byte[" << file.Size() << "]" << std::endl;
			sourceOutput << "\t\t{" << std::endl;

			for (uint64 i = 0, size = file.Size(); i < size; ++i)
			{
				byte outputByte;
				file.Read(&outputByte, sizeof(byte));
				sourceOutput << "\t\t\t" << static_cast<uint64>(outputByte) << "," << std::endl;
			}

			file.Close();

			sourceOutput << "\t\t};" << std::endl;
			sourceOutput << "\t}" << std::endl;
			sourceOutput << "}" << std::endl;
			sourceOutput << std::endl;
			sourceOutput << "#pragma optimize(\"\", on)" << std::endl;

			sourceOutput.close();
		}
	}

	masterOutput.close();

	return EXIT_SUCCESS;
}