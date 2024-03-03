#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StreamArchive.h>

class LevelData final
{

public:

	/*
	*	Level entry class definition.
	*/
	class LevelEntry final
	{

	public:

		/*
		*	Component entry class definition.
		*/
		class ComponentEntry final
		{

		public:

			//The component identifier.
			HashString _ComponentIdentifier;

			//The number of editable fields.
			uint64 _NumberOfEditableFields;

			//The field data position.
			uint64 _FieldDataPosition;

		};

		//The component entries.
		DynamicArray<ComponentEntry> _ComponentEntries;

	};

	//The level entries.
	DynamicArray<LevelEntry> _LevelEntries;

	//The stream archive.
	StreamArchive _StreamArchive;

};