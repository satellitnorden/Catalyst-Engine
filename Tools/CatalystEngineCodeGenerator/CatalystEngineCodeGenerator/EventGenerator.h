#pragma once

//Catalyst code generator.
#include "CatalystCodeGeneratorCore.h"
#include "json.hpp"

class EventGenerator final
{

public:

	/*
	*	Runs this event generator.
	*/
	void Run();

private:

	/*
	*	Event reference class definition.
	*/
	class EventReference final
	{

	public:

		//The file.
		std::string _File;

		//The singleton name.
		std::string _SingletonName;

	};

	/*
	*	Event class definition.
	*/
	class Event final
	{

	public:

		//The name.
		std::string _Name;

		//The argument types.
		std::vector<std::string> _ArgumentTypes;

		//The argument names.
		std::vector<std::string> _ArgumentNames;

		//The references.
		std::vector<EventReference> _References;

		/*
		*	Equality operator overload.
		*/
		bool operator==(const Event &other)
		{
			return _Name == other._Name
					/*&& _ArgumentTypes == other._ArgumentTypes*/ //Because for example a reference operator might be slightly differently placed, only compare the names for now...
					&& _ArgumentNames == other._ArgumentNames;
		}

	};

	/*
	*	Gathers events in the given directory.
	*/
	bool GatherEvents(const char *const directory_path, nlohmann::json *const JSON);

	/*
	*	Parses an event.
	*/
	void ParseEvent(std::ifstream &file, std::string &current_line, nlohmann::json *const JSON);

	/*
	*	Gathers unique events.
	*/
	void GatherUniqueEvents(const nlohmann::json &JSON, std::vector<Event> *const unique_events);

	/*
	*	Generates the header file.
	*/
	void GenerateHeaderFile(const nlohmann::json &JSON);

	/*
	*	Generates the source file.
	*/
	void GenerateSourceFile(const nlohmann::json &JSON);

};