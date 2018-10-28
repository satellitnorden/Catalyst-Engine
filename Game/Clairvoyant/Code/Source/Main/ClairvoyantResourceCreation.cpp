//Header file.
#include <Main/ClairvoyantResourceCreation.h>

//Resource creator.
#include <EnvironmentMaterialCreator.h>
#include <GrassMaterialCreator.h>
#include <GrassVegetationModelCreator.h>
#include <PhysicalMaterialCreator.h>
#include <PhysicalModelCreator.h>
#include <ResourceCollectionCreator.h>
#include <OceanMaterialCreator.h>
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
			//Create the night environment material.
			EnvironmentMaterialCreator::EnvironmentMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\NightEnvironmentMaterial";
			parameters._ID = "NightEnvironmentMaterial";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Night.png";
			parameters._DiffuseResolution = 1'024;
			parameters._DiffuseIrradianceResolution = 2;

			//EnvironmentMaterialCreator::CreateEnvironmentMaterial(parameters);
		}

		{
			//Create the morning environment material.
			EnvironmentMaterialCreator::EnvironmentMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\MorningEnvironmentMaterial";
			parameters._ID = "MorningEnvironmentMaterial";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Morning.hdr";
			parameters._DiffuseResolution = 1'024;
			parameters._DiffuseIrradianceResolution = 2;

			//EnvironmentMaterialCreator::CreateEnvironmentMaterial(parameters);
		}

		{
			//Create the evening environment material.
			EnvironmentMaterialCreator::EnvironmentMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\EveningEnvironmentMaterial";
			parameters._ID = "EveningEnvironmentMaterial";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Evening.hdr";
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

			OceanMaterialCreator::CreateOceanMaterial(parameters);
		}

		{
			//Create the common amaryllis grass vegetation material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\AmaryllisGrassVegetationMaterial";
			parameters._ID = "AmaryllisGrassVegetationMaterial";
			parameters._MaskMipmapLevels = 9;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Amaryllis\\Mask.png";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Amaryllis\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Amaryllis\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the common fern grass material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\CommonFernGrassMaterial";
			parameters._ID = "CommonFernGrassMaterial";
			parameters._MaskMipmapLevels = 9;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\CommonFern\\Mask.png";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\CommonFern\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\CommonFern\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the default grass material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultGrassMaterial";
			parameters._ID = "DefaultGrassMaterial";
			parameters._MaskMipmapLevels = 2;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\DefaultGrass\\Mask.png";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\DefaultGrass\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\DefaultGrass\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the snow grass material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\SandGrassMaterial";
			parameters._ID = "SandGrassMaterial";
			parameters._MaskMipmapLevels = 2;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\SandGrass\\Mask.png";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\SandGrass\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\SandGrass\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the snow grass material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\SnowGrassMaterial";
			parameters._ID = "SnowGrassMaterial";
			parameters._MaskMipmapLevels = 2;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\SnowGrass\\Mask.png";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\SnowGrass\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\SnowGrass\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the amaryllis grass vegetation model.
			GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\AmaryllisGrassVegetationModel";
			parameters._ID = "AmaryllisGrassVegetationModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\AmaryllisGrassVegetationModel.fbx";
			parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::X;

			GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
		}

		{
			//Create the low detail common fern grass vegetation model.
			GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailCommonFernModel";
			parameters._ID = "LowDetailCommonFernModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\LowDetailCommonFernModel.fbx";
			parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::X;

			GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
		}

		{
			//Create the medium detail common fern grass vegetation model.
			GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailCommonFernModel";
			parameters._ID = "MediumDetailCommonFernModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\MediumDetailCommonFernModel.fbx";
			parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::X;

			GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
		}

		{
			//Create the high detail common fern grass vegetation model.
			GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailCommonFernModel";
			parameters._ID = "HighDetailCommonFernModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\HighDetailCommonFernModel.fbx";
			parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::X;

			GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
		}

		{
			//Create the low detail default grass vegetation model.
			GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailDefaultGrassModel";
			parameters._ID = "LowDetailDefaultGrassModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\LowDetailDefaultGrassModel.fbx";
			parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::Y;

			GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
		}

		{
			//Create the medium detail default grass vegetation model.
			GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailDefaultGrassModel";
			parameters._ID = "MediumDetailDefaultGrassModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\MediumDetailDefaultGrassModel.fbx";
			parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::Y;

			GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
		}

		{
			//Create the high detail default grass vegetation model.
			GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailDefaultGrassModel";
			parameters._ID = "HighDetailDefaultGrassModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\HighDetailDefaultGrassModel.fbx";
			parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::Y;

			GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
		}

		{
			//Create the test material.
			PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\TestMaterial";
			parameters._ID = "TestMaterial";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Test\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Test\\NormalMap.png";
			parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Test\\Roughness.png";
			parameters._MetallicFile = "..\\..\\..\\Resources\\Raw\\Textures\\Test\\Metallic.png";
			parameters._AmbientOcclusionFile = nullptr;
			parameters._ThicknessFile = nullptr;

			PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
		}

		{
			//Create the rock volcanic physical material.
			PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\RockVolcanicMaterial";
			parameters._ID = "RockVolcanicMaterial";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\RockVolcanic\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\RockVolcanic\\NormalMap.png";
			parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\RockVolcanic\\Roughness.png";
			parameters._MetallicFile = nullptr;
			parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\RockVolcanic\\AmbientOcclusion.png";
			parameters._ThicknessFile = nullptr;

			PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
		}

		{
			//Create the tree stump physical material.
			PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\TreeStumpMaterial";
			parameters._ID = "TreeStumpMaterial";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\TreeStump\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\TreeStump\\NormalMap.png";
			parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\TreeStump\\Roughness.png";
			parameters._MetallicFile = nullptr;
			parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\TreeStump\\AmbientOcclusion.png";
			parameters._ThicknessFile = nullptr;

			PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
		}

		{
			//Create the test model.
			PhysicalModelCreator::PhysicalModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\TestModel";
			parameters._ID = "TestModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Test\\Model.obj";

			PhysicalModelCreator::CreatePhysicalModel(parameters);
		}

		{
			//Create the low detail tree stump model.
			PhysicalModelCreator::PhysicalModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailTreeStumpModel";
			parameters._ID = "LowDetailTreeStumpModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\LowDetailTreeStumpModel.fbx";

			PhysicalModelCreator::CreatePhysicalModel(parameters);
		}

		{
			//Create the medium detail tree stump model.
			PhysicalModelCreator::PhysicalModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailTreeStumpModel";
			parameters._ID = "MediumDetailTreeStumpModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\MediumDetailTreeStumpModel.fbx";

			PhysicalModelCreator::CreatePhysicalModel(parameters);
		}

		{
			//Create the high detail tree stump model.
			PhysicalModelCreator::PhysicalModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailTreeStumpModel";
			parameters._ID = "HighDetailTreeStumpModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\HighDetailTreeStumpModel.fbx";

			PhysicalModelCreator::CreatePhysicalModel(parameters);
		}

		{
			//Create the low detail rock volcanic model.
			PhysicalModelCreator::PhysicalModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailRockVolcanicModel";
			parameters._ID = "LowDetailRockVolcanicModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\LowDetailRockVolcanicModel.fbx";

			PhysicalModelCreator::CreatePhysicalModel(parameters);
		}

		{
			//Create the medium detail rock volcanic model.
			PhysicalModelCreator::PhysicalModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailRockVolcanicModel";
			parameters._ID = "MediumDetailRockVolcanicModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\MediumDetailRockVolcanicModel.fbx";

			PhysicalModelCreator::CreatePhysicalModel(parameters);
		}

		{
			//Create the high detail rock volcanic model.
			PhysicalModelCreator::PhysicalModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailRockVolcanicModel";
			parameters._ID = "HighDetailRockVolcanicModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\HighDetailRockVolcanicModel.fbx";

			PhysicalModelCreator::CreatePhysicalModel(parameters);
		}

		{
			//Create the terrain material.
			TerrainMaterialCreator::TerrainMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultTerrainMaterial";
			parameters._ID = "DefaultTerrainMaterial";
			parameters._MipmapLevels = 9;

			parameters._Layers[0]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1Albedo.png";
			parameters._Layers[0]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1NormalMap.png";
			parameters._Layers[0]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1Roughness.png";
			parameters._Layers[0]._Metallic = nullptr;
			parameters._Layers[0]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1AmbientOcclusion.png";
			parameters._Layers[0]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass1Displacement.png";

			parameters._Layers[1]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2Albedo.png";
			parameters._Layers[1]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2NormalMap.png";
			parameters._Layers[1]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2Roughness.png";
			parameters._Layers[1]._Metallic = nullptr;
			parameters._Layers[1]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2AmbientOcclusion.png";
			parameters._Layers[1]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass2Displacement.png";

			parameters._Layers[2]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Albedo.png";
			parameters._Layers[2]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1NormalMap.png";
			parameters._Layers[2]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Roughness.png";
			parameters._Layers[2]._Metallic = nullptr;
			parameters._Layers[2]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1AmbientOcclusion.png";
			parameters._Layers[2]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand1Displacement.png";

			parameters._Layers[3]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1Albedo.png";
			parameters._Layers[3]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1NormalMap.png";
			parameters._Layers[3]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1Roughness.png";
			parameters._Layers[3]._Metallic = nullptr;
			parameters._Layers[3]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1AmbientOcclusion.png";
			parameters._Layers[3]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow1Displacement.png";

			parameters._Layers[4]._Albedo = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1Albedo.png";
			parameters._Layers[4]._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1NormalMap.png";
			parameters._Layers[4]._Roughness = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1Roughness.png";
			parameters._Layers[4]._Metallic = nullptr;
			parameters._Layers[4]._AmbientOcclusion = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1AmbientOcclusion.png";
			parameters._Layers[4]._Displacement = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Stone1Displacement.png";

			TerrainMaterialCreator::CreateTerrainMaterial(parameters);
		}

		{
			//Create the resource collection.
			ResourceCollectionCreator::ResourceCollectionCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Final\\ClairvoyantResourceCollection";
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\NightEnvironmentMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\MorningEnvironmentMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\EveningEnvironmentMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultOceanMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\AmaryllisGrassVegetationMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\CommonFernGrassMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultGrassMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\SandGrassMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\SnowGrassMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\AmaryllisGrassVegetationModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailCommonFernModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailCommonFernModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailCommonFernModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailDefaultGrassModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailDefaultGrassModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailDefaultGrassModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\TestMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\RockVolcanicMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\TreeStumpMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\TestModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailRockVolcanicModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailRockVolcanicModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailRockVolcanicModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailTreeStumpModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailTreeStumpModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailTreeStumpModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultTerrainMaterial.cr");

			ResourceCollectionCreator::CreateResourceCollection(parameters);
		}
	}

}