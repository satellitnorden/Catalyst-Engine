//Header file.
#include <Core/General/JSON.h>

//File.
#include <File/Utilities/TextParsingUtilities.h>

//Third party.
#include <ThirdParty/json/json.hpp>

//STl.
#include <fstream>

//Defines.
#define IMPLEMENTATION (_Implementation.Get<Implementation>())

/*
*	Implementation class definition.
*/
class Implementation final
{

public:

	//The storage.
	StaticArray<byte, sizeof(nlohmann::ordered_json)> _Storage;

	//The JSON object.
	nlohmann::ordered_json *RESTRICT _JSON;

	//Denotes whether or not this object is a sub object.
	bool _IsSubObject;

};

static_assert(JSON::ANY_SIZE >= sizeof(Implementation), "Insufficient size!");

/*
*	Default constructor.
*/
JSON::JSON() NOEXCEPT
{
	//Construct the implementation.
	IMPLEMENTATION->_JSON = new (IMPLEMENTATION->_Storage.Data()) nlohmann::ordered_json();
	IMPLEMENTATION->_IsSubObject = false;
}

/*
*	Default destructor.
*/
JSON::~JSON() NOEXCEPT
{
	if (!IMPLEMENTATION->_IsSubObject)
	{
		IMPLEMENTATION->_JSON->~basic_json();
	}
}

/*
*	Assignment operator taking a C string.
*/
void JSON::operator=(const char *const RESTRICT value) NOEXCEPT
{
	(*IMPLEMENTATION->_JSON) = value;
}

/*
*	Assignment operator taking a bool.
*/
void JSON::operator=(const bool value) NOEXCEPT
{
	(*IMPLEMENTATION->_JSON) = value;
}

/*
*	Assignment operator taking a float64.
*/
void JSON::operator=(const float64 value) NOEXCEPT
{
	(*IMPLEMENTATION->_JSON) = value;
}

/*
*	Assignment operator taking a uint64.
*/
void JSON::operator=(const uint64 value) NOEXCEPT
{
	(*IMPLEMENTATION->_JSON) = value;
}

/*
*	Subscript operator overload.
*/
JSON JSON::operator[](const char *const RESTRICT key) NOEXCEPT
{
	JSON sub_object;

	sub_object._Implementation.Get<Implementation>()->_JSON = &(*IMPLEMENTATION->_JSON)[key];
	sub_object._Implementation.Get<Implementation>()->_IsSubObject = true;

	return sub_object;
}

/*
*	Reads from the given file path.
*/
void JSON::Read(const char *const RESTRICT file_path) NOEXCEPT
{
	//Open the file.
	std::ifstream file{ file_path };

	/*
	*	Gather the source, adding some custom filtering. (:
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
	IMPLEMENTATION->_JSON->parse(source.c_str());
}

/*
*	Writes to the given file path.
*/
void JSON::Write(const char *const RESTRICT file_path) NOEXCEPT
{
	//OPen the file.
	std::ofstream file{ file_path };

	//Write the JSON to the file.
	file << std::setw(4) << (*IMPLEMENTATION->_JSON);

	//Close the file.
	file.close();
}

/*
*	Emplaces a new object under this JSON object.
*/
JSON JSON::Emplace() NOEXCEPT
{
	JSON sub_object;

	sub_object._Implementation.Get<Implementation>()->_JSON = &(*IMPLEMENTATION->_JSON).emplace_back();
	sub_object._Implementation.Get<Implementation>()->_IsSubObject = true;

	return sub_object;
}