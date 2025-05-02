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

static_assert(JSON::Iterator::ITERATOR_ANY_SIZE >= sizeof(nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>), "Insufficient size!");
static_assert(JSON::ANY_SIZE >= sizeof(Implementation), "Insufficient size!");

/*
*	Dereference operator overload.
*/
JSON JSON::Iterator::operator*() NOEXCEPT
{
	JSON sub_object;

	sub_object._Implementation.Get<Implementation>()->_JSON = &_Implementation.Get<nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>>()->operator*();
	sub_object._Implementation.Get<Implementation>()->_IsSubObject = true;

	return sub_object;
}

/*
*	Increment operator overload.
*/
void JSON::Iterator::operator++() NOEXCEPT
{
	_Implementation.Get<nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>>()->operator++();
}

/*
*	Equality operator overload.
*/
NO_DISCARD bool JSON::Iterator::operator==(const Iterator &other) NOEXCEPT
{
	return *_Implementation.Get<nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>>() == *other._Implementation.Get<nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>>();
}

/*
*	Inequality operator overload.
*/
NO_DISCARD bool JSON::Iterator::operator!=(const Iterator &other) NOEXCEPT
{
	return *_Implementation.Get<nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>>() != *other._Implementation.Get<nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>>();
}

/*
*	Returns the name.
*/
NO_DISCARD DynamicString JSON::Iterator::Name() NOEXCEPT
{
	return _Implementation.Get<nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>>()->key().c_str();
}

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
*	Assignment operator taking an int32.
*/
void JSON::operator=(const int32 value) NOEXCEPT
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
*	Begin operator overload.
*/
JSON::Iterator JSON::begin() NOEXCEPT
{
	Iterator iterator;

	*iterator._Implementation.Get<nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>>() = IMPLEMENTATION->_JSON->begin();

	return iterator;
}

/*
*	End operator overload.
*/
JSON::Iterator JSON::end() NOEXCEPT
{
	Iterator iterator;

	*iterator._Implementation.Get<nlohmann::json_abi_v3_11_3::detail::iter_impl<nlohmann::json_abi_v3_11_3::ordered_json>>() = IMPLEMENTATION->_JSON->end();

	return iterator;
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
	*IMPLEMENTATION->_JSON = nlohmann::ordered_json::parse(source.c_str());
}

/*
*	Writes to the given file path.
*/
void JSON::Write(const char *const RESTRICT file_path) NOEXCEPT
{
	//Open the file.
	std::ofstream file{ file_path };

	//Write the JSON to the file.
	file << std::setw(4) << (*IMPLEMENTATION->_JSON);

	//Close the file.
	file.close();
}

/*
*	Returns if this JSON object contains the given key.
*/
NO_DISCARD bool JSON::Contains(const char *const RESTRICT key) NOEXCEPT
{
	return IMPLEMENTATION->_JSON->contains(key);
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

/*
*	Emplaces a new object under this JSON object.
*/
JSON JSON::Emplace(const char *const RESTRICT name) NOEXCEPT
{
	JSON sub_object;

	sub_object._Implementation.Get<Implementation>()->_JSON = &(*IMPLEMENTATION->_JSON).emplace_back(name);
	sub_object._Implementation.Get<Implementation>()->_IsSubObject = true;

	return sub_object;
}

/*
*	Returns the size of this JSON object.
*/
NO_DISCARD uint64 JSON::Size() const NOEXCEPT
{
	return IMPLEMENTATION->_JSON->size();
}

/*
*	Clears this JSON object.
*/
void JSON::Clear() NOEXCEPT
{
	IMPLEMENTATION->_JSON->clear();
}

/*
*	Converts this JSON object to a bool.
*/
NO_DISCARD bool JSON::ToBool() const NOEXCEPT
{
	return IMPLEMENTATION->_JSON->get<bool>();
}

/*
*	Converts this JSON object to a float32.
*/
NO_DISCARD float32 JSON::ToFloat32() const NOEXCEPT
{
	return IMPLEMENTATION->_JSON->get<float32>();
}

/*
*	Converts this JSON object to a float64.
*/
NO_DISCARD float64 JSON::ToFloat64() const NOEXCEPT
{
	return IMPLEMENTATION->_JSON->get<float64>();
}

/*
*	Converts this JSON object to an int8.
*/
NO_DISCARD int8 JSON::ToInt8() const NOEXCEPT
{
	return IMPLEMENTATION->_JSON->get<int8>();
}

/*
*	Converts this JSON object to an int32
*/
NO_DISCARD int32 JSON::ToInt32() const NOEXCEPT
{
	return IMPLEMENTATION->_JSON->get<int32>();
}

/*
*	Converts this JSON object to a string.
*/
NO_DISCARD DynamicString JSON::ToString() const NOEXCEPT
{
	return DynamicString(IMPLEMENTATION->_JSON->get<std::string>().c_str());
}

/*
*	Converts this JSON object to a uint8.
*/
NO_DISCARD uint8 JSON::ToUint8() const NOEXCEPT
{
	return IMPLEMENTATION->_JSON->get<uint8>();
}

/*
*	Converts this JSON object to a uint32.
*/
NO_DISCARD uint32 JSON::ToUint32() const NOEXCEPT
{
	return IMPLEMENTATION->_JSON->get<uint32>();
}

/*
*	Converts this JSON object to a uint64.
*/
NO_DISCARD uint64 JSON::ToUint64() const NOEXCEPT
{
	return IMPLEMENTATION->_JSON->get<uint64>();
}