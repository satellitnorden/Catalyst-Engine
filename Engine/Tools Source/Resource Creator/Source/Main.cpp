//Engine core.
#include <EngineCore.h>

//Resource creator.
#include <PhysicalModelCreator.h>
#include <TerrainMaterialCreator.h>

//Preprocessor defines.
#if !defined(EXIT_SUCCESS)
	#define EXIT_SUCCESS
#endif

int main(const int32 argumentCount, char *RESTRICT arguments[]) NOEXCEPT
{
	//First, determine what type of resource that is supposed to be created.
	if (strcmp(arguments[1], "PhysicalModel") == 0)
	{
		PhysicalModelCreator::CreatePhysicalModel(argumentCount, arguments);
	}

	else if (strcmp(arguments[1], "TerrainMaterial") == 0)
	{
		TerrainMaterialCreator::CreateTerrainMaterial(argumentCount, arguments);
	}

	return EXIT_SUCCESS;
}