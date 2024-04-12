//Header file.
#include <Core/General/JSON.h>

//File.
#include <File/Utilities/TextParsingUtilities.h>

//Third party.
#include <ThirdParty/json/json.hpp>

//STl.
#include <fstream>

//Defines.
#define IMPLEMENTATION() (_Implementation.Get<nlohmann::json>())

static_assert(JSON::ANY_SIZE >= sizeof(nlohmann::json), "Insufficient size!");

/*
*	Default constructor.
*/
JSON::JSON() NOEXCEPT
{
	//Construct the implementation.
	new (IMPLEMENTATION()) nlohmann::json();
}

/*
*	Constructor taking a file path.
*/
JSON::JSON(const char *const RESTRICT file_path) NOEXCEPT
{
	//Construct the implementation.
	new (IMPLEMENTATION()) nlohmann::json();

	//Open the file.
	std::ifstream file{ file_path };

	/*
	*	Gather the source, adding some custom filtering to remove comments. (:
	*/
	std::string source;

	{
		std::string current_line;

		while (std::getline(file, current_line))
		{
			if (TextParsingUtilities::OnlyComment(current_line.data(), current_line.length()))
			{
				continue;
			}

			source += current_line;
		}
	}

	//Close the file.
	file.close();

	//Now parse into the JSON object.
	*IMPLEMENTATION() = nlohmann::json::parse(source.c_str());
}

/*
*	Subscript operator overload.
*/
JSON JSON::operator[](const char *const RESTRICT key) NOEXCEPT
{
	JSON sub_object;

	*sub_object._Implementation.Get<nlohmann::json>() = (*IMPLEMENTATION())[key];

	return sub_object;
}