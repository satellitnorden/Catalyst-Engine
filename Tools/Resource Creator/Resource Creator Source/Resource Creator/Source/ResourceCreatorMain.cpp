//Engine core.
#include <Engine Core/EngineCore.h>

//Resource creator.
#include <EnvironmentMaterialCreator.h>
#include <PhysicalMaterialCreator.h>
#include <PhysicalModelCreator.h>
#include <ResourceCollectionCreator.h>
#include <TerrainMaterialCreator.h>
#include <WaterMaterialCreator.h>

//Preprocessor defines.
#if !defined(EXIT_SUCCESS)
	#define EXIT_SUCCESS
#endif

int main(const int32 argumentCount, char *RESTRICT arguments[]) NOEXCEPT
{
	//First, determine what type of resource that is supposed to be created.
	if (strcmp(arguments[1], "EnvironmentMaterial") == 0)
	{
		EnvironmentMaterialCreator::CreateEnvironmentMaterial(argumentCount, arguments);
	}

	else if (strcmp(arguments[1], "PhysicalMaterial") == 0)
	{
		PhysicalMaterialCreator::CreatePhysicalMaterial(argumentCount, arguments);
	}

	else if (strcmp(arguments[1], "PhysicalModel") == 0)
	{
		PhysicalModelCreator::CreatePhysicalModel(argumentCount, arguments);
	}

	else if (strcmp(arguments[1], "TerrainMaterial") == 0)
	{
		TerrainMaterialCreator::CreateTerrainMaterial(argumentCount, arguments);
	}

	else if (strcmp(arguments[1], "WaterMaterial") == 0)
	{
		WaterMaterialCreator::CreateWaterMaterial(argumentCount, arguments);
	}

	else if (strcmp(arguments[1], "ResourceCollection") == 0)
	{
		ResourceCollectionCreator::CreateResourceCollection(argumentCount, arguments);
	}

	return EXIT_SUCCESS;
}