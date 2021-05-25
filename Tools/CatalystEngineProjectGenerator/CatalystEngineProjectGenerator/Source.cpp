//Standard library.
#include <filesystem>
#include <fstream>

int main(int argc, char *argv[])
{
	//Retrieve the project name.
	const char *const project_name{ argv[1] };

	//Read the template CMakeLists.txt file and output a new file.
	std::ifstream cmake_lists_input_file{ "C:\\Github\\Catalyst-Engine\\Templates\\CMakeLists.txt" };
	std::ofstream cmake_lists_output_file{ "CMakeLists.txt" };

	std::string cmake_lists_line;

	while (std::getline(cmake_lists_input_file, cmake_lists_line))
	{
		const size_t project_name_position{ cmake_lists_line.find("[PROJECT_NAME]") };

		if (project_name_position != std::string::npos)
		{
			cmake_lists_line.replace(project_name_position, strlen("[PROJECT_NAME]"), project_name);
		}

		cmake_lists_output_file << cmake_lists_line << std::endl;
	}

	//Close the files.
	cmake_lists_input_file.close();
	cmake_lists_output_file.close();

	//Generate the Win64 project.
	std::filesystem::create_directories("Win64\\Win64");

	//Run CMake.
	system("cmake -B Win64\\Win64");

	//Remove the CMakeLists.txt file.
	//std::filesystem::remove("CMakeLists.txt");

	return 0;
}