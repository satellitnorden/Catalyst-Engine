#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//STD.
#include <fstream>

/*
*	This namespace contains a number of utility functions for working with Reaper (the DAW).
*/
namespace ReaperUtilities
{

	/*
	*	Generates regions parameters class definition.
	*/
	class GenerateRegionsParameters final
	{

	public:

		/*
		*	Region class definition.
		*/
		class Region final
		{

		public:

			//The name.
			DynamicString _Name;

			//The start bar.
			uint32 _StartBar;

			//The number of bars.
			uint32 _NumberOfBars;

		};

		//The output file path.
		const char *RESTRICT _OutputFilePath;

		//The regions.
		DynamicArray<Region> _Regions;

	};

	/*
	*	Generates a regions file.
	*/
	FORCE_INLINE void GenerateRegions(const GenerateRegionsParameters &parameters) NOEXCEPT
	{
		//Open the file.
		std::ofstream file{ parameters._OutputFilePath };

		//Insert the header.
		file << "#,Name,Start,End,Length" << std::endl;

		//Add all regions.
		uint32 region_counter{ 1 };

		for (const GenerateRegionsParameters::Region &region : parameters._Regions)
		{
			file << "R" << region_counter++ << "," << region._Name.Data() << "," << region._StartBar << ".0.00," << (region._StartBar + region._NumberOfBars) << ".0.00," << region._NumberOfBars << ".0.00" << std::endl;
		}

		//Close the file.
		file.close();
	}

}