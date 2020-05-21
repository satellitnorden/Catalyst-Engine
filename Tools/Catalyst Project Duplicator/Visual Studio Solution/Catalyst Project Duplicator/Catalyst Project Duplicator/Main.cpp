#include <filesystem>
#include <fstream>
#include <iostream>

/*
*	Processes a file.
*/
void ProcessFile(const std::string& file, const std::string& original_name, const std::string& new_name)
{
	//Nofity the user that the file is processing.
	std::cout << "Processing file; " << file << std::endl;

	//Read the input file.
	std::ifstream input_file{ file };
	std::string file_string;

	input_file.seekg(0, std::ios::end);   
	file_string.reserve(input_file.tellg());
	input_file.seekg(0, std::ios::beg);

	file_string.assign(	std::istreambuf_iterator<char>(input_file),
						std::istreambuf_iterator<char>());

	//Close the input file.
	input_file.close();

	//Replace all occurences.
	size_t position{ file_string.find(original_name) };

	while (position != std::string::npos)
	{
		file_string.replace(position, original_name.length(), new_name);

		position = file_string.find(original_name);
	}

	//Write to the output file.
	std::ofstream output_file{ file };
	output_file << file_string;
	output_file.close();
}

/*
*	Processes a directory.
*/
void ProcessDirectory(const std::string& directory, const std::string& original_name, const std::string& new_name)
{
	//Iterate over all files in the directory and change every occuerence from the original name to the new name.
	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		if (entry.is_directory())
		{
			ProcessDirectory(entry.path().generic_u8string(), original_name, new_name);
		}
		
		else
		{
			ProcessFile(entry.path().generic_u8string(), original_name, new_name);
		}
	}
}

int main(int number_of_arguments, char* arguments[])
{
	//Retrieve the directory.
	const std::string directory{ arguments[1] };

	//Retrieve the original name.
	const std::string original_name{ arguments[2] };

	//Retrieve the new name.
	const std::string new_name{ arguments[3] };

	//Process the directory.
	ProcessDirectory(directory, original_name, new_name);

	return 0;
}