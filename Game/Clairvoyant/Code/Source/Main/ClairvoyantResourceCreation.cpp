//Header file.
#include <Main/ClairvoyantResourceCreation.h>

//Resource creator.
#include <EnvironmentMaterialCreator.h>
#include <PhysicalModelCreator.h>
#include <ResourceCollectionCreator.h>
#include <OceanMaterialCreator.h>
#include <TerrainMaterialCreator.h>
#include <VegetationMaterialCreator.h>
#include <VegetationModelCreator.h>

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
			//Create the day environment material.
			EnvironmentMaterialCreator::EnvironmentMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\DayEnvironmentMaterial";
			parameters._ID = "DayEnvironmentMaterial";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Day.hdr";
			parameters._DiffuseResolution = 1'024;
			parameters._DiffuseIrradianceResolution = 2;

			//EnvironmentMaterialCreator::CreateEnvironmentMaterial(parameters);
		}

		{
			//Create the night environment material.
			EnvironmentMaterialCreator::EnvironmentMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\NightEnvironmentMaterial";
			parameters._ID = "NightEnvironmentMaterial";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Night.hdr";
			parameters._DiffuseResolution = 1'024;
			parameters._DiffuseIrradianceResolution = 2;

			//EnvironmentMaterialCreator::CreateEnvironmentMaterial(parameters);
		}

		{
			//Create the ocean material.
			OceanMaterialCreator::OceanMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultOceanMaterial";
			parameters._ID = "DefaultOceanMaterial";
			parameters._MipmapLevels = 9;
			parameters._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Ocean\\Normal.jpg";
			parameters._Foam = "..\\..\\..\\Resources\\Raw\\Textures\\Ocean\\Foam.jpg";

			//OceanMaterialCreator::CreateOceanMaterial(parameters);
		}

		{
			//Create the grass vegetation material.
			VegetationMaterialCreator::VegetationMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\GrassVegetationMaterial";
			parameters._ID = "GrassVegetationMaterial";
			parameters._MaskMipmapLevels = 8;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Grass\\Mask.png";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Grass\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Grass\\NormalMap.png";

			VegetationMaterialCreator::CreateVegetationMaterial(parameters);
		}

		{
			//Create the grass vegetation model.
			VegetationModelCreator::VegetationModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\GrassVegetationModel";
			parameters._ID = "GrassVegetationModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\GrassVegetationModel.fbx";
			parameters._UpAxis = VegetationModelCreator::VegetationModelCreationParameters::Axis::X;

			//VegetationModelCreator::CreateVegetationModel(parameters);
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

			parameters._Layers[1]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1Albedo.png";
			parameters._Layers[1]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1NormalMap.png";
			parameters._Layers[1]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1Roughness.png";
			parameters._Layers[1]._Metallic = nullptr;
			parameters._Layers[1]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1AmbientOcclusion.png";
			parameters._Layers[1]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1Displacement.png";

			parameters._Layers[2]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2Albedo.png";
			parameters._Layers[2]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2NormalMap.png";
			parameters._Layers[2]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2Roughness.png";
			parameters._Layers[2]._Metallic = nullptr;
			parameters._Layers[2]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2AmbientOcclusion.png";
			parameters._Layers[2]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2Displacement.png";

			parameters._Layers[3]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1Albedo.png";
			parameters._Layers[3]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1NormalMap.png";
			parameters._Layers[3]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1Roughness.png";
			parameters._Layers[3]._Metallic = nullptr;
			parameters._Layers[3]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1AmbientOcclusion.png";
			parameters._Layers[3]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1Displacement.png";

			parameters._Layers[4]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1Albedo.png";
			parameters._Layers[4]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1NormalMap.png";
			parameters._Layers[4]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1Roughness.png";
			parameters._Layers[4]._Metallic = nullptr;
			parameters._Layers[4]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1AmbientOcclusion.png";
			parameters._Layers[4]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1Displacement.png";

			//TerrainMaterialCreator::CreateTerrainMaterial(parameters);
		}

		{
			//Create the resource collection.
			ResourceCollectionCreator::ResourceCollectionCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Final\\ClairvoyantResourceCollection";
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DayEnvironmentMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\NightEnvironmentMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultOceanMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\GrassVegetationMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\GrassVegetationModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultTerrainMaterial.cr");

			ResourceCollectionCreator::CreateResourceCollection(parameters);
		}
	}

}