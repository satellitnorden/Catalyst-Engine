//STL.
#include <iostream>

//Asset creators.
#include "TerrainMaterialCreator.h"

//Preprocessor defines.
#define EXIT_SUCCESS 0

int main(const int argumentCount, char *arguments[]) noexcept
{
	//First, determine what type of asset that is supposed to be created.
	if (strcmp(arguments[1], "TerrainMaterial") == 0)
	{
		TerrainMaterialCreator::CreateTerrainMaterial(argumentCount, arguments);
	}

	return EXIT_SUCCESS;
}