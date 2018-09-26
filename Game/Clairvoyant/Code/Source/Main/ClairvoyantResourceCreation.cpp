//Header file.
#include <Main/ClairvoyantResourceCreation.h>

//Resource creator.
#include <ResourceCollectionCreator.h>
#include <TerrainMaterialCreator.h>

namespace ClairvoyantResourceCreation
{

	/*
	*	Creates resources for the Clairvoyant project.
	*/
	void CreateResources() NOEXCEPT
	{
#if !defined(CATALYST_FINAL)
		if (true)
#else
		if (true)
#endif
		{
			return;
		}

		{
			//Create the terrain material.
			TerrainMaterialCreator::TerrainMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultTerrainMaterial";
			parameters._ID = "DefaultTerrainMaterial";
			parameters._MipmapLevels = 9;

			parameters._Layers[0]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Albedo.png";
			parameters._Layers[0]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1NormalMap.png";
			parameters._Layers[0]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Roughness.png";
			parameters._Layers[0]._Metallic = nullptr;
			parameters._Layers[0]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1AmbientOcclusion.png";
			parameters._Layers[0]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Displacement.png";

			parameters._Layers[1]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Albedo.png";
			parameters._Layers[1]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1NormalMap.png";
			parameters._Layers[1]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Roughness.png";
			parameters._Layers[1]._Metallic = nullptr;
			parameters._Layers[1]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1AmbientOcclusion.png";
			parameters._Layers[1]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Displacement.png";

			parameters._Layers[2]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Albedo.png";
			parameters._Layers[2]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1NormalMap.png";
			parameters._Layers[2]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Roughness.png";
			parameters._Layers[2]._Metallic = nullptr;
			parameters._Layers[2]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1AmbientOcclusion.png";
			parameters._Layers[2]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Displacement.png";

			parameters._Layers[3]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Albedo.png";
			parameters._Layers[3]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1NormalMap.png";
			parameters._Layers[3]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Roughness.png";
			parameters._Layers[3]._Metallic = nullptr;
			parameters._Layers[3]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1AmbientOcclusion.png";
			parameters._Layers[3]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Displacement.png";

			parameters._Layers[4]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Albedo.png";
			parameters._Layers[4]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1NormalMap.png";
			parameters._Layers[4]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Roughness.png";
			parameters._Layers[4]._Metallic = nullptr;
			parameters._Layers[4]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1AmbientOcclusion.png";
			parameters._Layers[4]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Displacement.png";

			TerrainMaterialCreator::CreateTerrainMaterial(parameters);
		}

		{
			//Create the resource collection.
			ResourceCollectionCreator::ResourceCollectionCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Final\\ClairvoyantResourceCollection";
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultTerrainMaterial.cr");

			ResourceCollectionCreator::CreateResourceCollection(parameters);
		}
	}

}