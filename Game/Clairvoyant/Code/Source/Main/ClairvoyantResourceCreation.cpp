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

//Preprocessor defines.
#define CREATE_INTERMEDIATE_RESOURCES false
#define CREATE_RESOURCE_COLLECTION false

/*
*	Creates resources for the Clairvoyant project.
*/
void ClairvoyantResourceCreation::CreateResources() NOEXCEPT
{
#if CREATE_INTERMEDIATE_RESOURCES
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
		//Create the seaweed grass vegetation material.
		GrassMaterialCreator::GrassMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\SeaweedGrassVegetationMaterial";
		parameters._ID = "SeaweedGrassVegetationMaterial";
		parameters._MaskMipmapLevels = 2;
		parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Seaweed\\Mask.png";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Seaweed\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Seaweed\\NormalMap.png";

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
		//Create the tower material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\TowerMaterial";
		parameters._ID = "TowerMaterial";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\General\\Tower\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\General\\Tower\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\General\\Tower\\Roughness.png";
		parameters._MetallicFile = "..\\..\\..\\Resources\\Raw\\Textures\\General\\Tower\\Metallic.png";
		parameters._AmbientOcclusionFile = nullptr;
		parameters._VariantFile = nullptr;

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
		parameters._VariantFile = nullptr;

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
		parameters._VariantFile = nullptr;

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

	CreateTerrainIntermediateResources();
#endif

#if CREATE_RESOURCE_COLLECTION
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
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\SeaweedGrassVegetationMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\SnowGrassMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\AmaryllisGrassVegetationModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailCommonFernModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailCommonFernModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailCommonFernModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailDefaultGrassModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailDefaultGrassModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailDefaultGrassModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\TowerMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\RockVolcanicMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\TreeStumpMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\TestModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailRockVolcanicModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailRockVolcanicModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailRockVolcanicModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\LowDetailTreeStumpModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\MediumDetailTreeStumpModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\HighDetailTreeStumpModel.cr");

		ResourceCollectionCreator::CreateResourceCollection(parameters);
	}

	CreateTerrainResourceCollection();
#endif
}
/*
*	Creates the terrain intermediate resources.
*/
void ClairvoyantResourceCreation::CreateTerrainIntermediateResources() NOEXCEPT
{
	{
		//Create the terrain cliff snow 1 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Cliff_Snow_1_Material";
		parameters._ID = "Terrain_Cliff_Snow_1_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_1\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_1\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_1\\AmbientOcclusion.png";
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_1\\Displacement.png";

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the terrain grass 1 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Grass_1_Material";
		parameters._ID = "Terrain_Grass_1_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_1\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_1\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_1\\AmbientOcclusion.png";
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_1\\Displacement.png";

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the terrain grass 2 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Grass_2_Material";
		parameters._ID = "Terrain_Grass_2_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_2\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_2\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_2\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_2\\AmbientOcclusion.png";
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Grass_2\\Displacement.png";

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the terrain sand 1 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Sand_1_Material";
		parameters._ID = "Terrain_Sand_1_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_1\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_1\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_1\\AmbientOcclusion.png";
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_1\\Displacement.png";

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the terrain snow 1 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Snow_1_Material";
		parameters._ID = "Terrain_Snow_1_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow_1\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow_1\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow_1\\AmbientOcclusion.png";
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow_1\\Displacement.png";

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the terrain test material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Test_Material";
		parameters._ID = "TerrainTestMaterial";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Test\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Test\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Test\\Roughness.png";
		parameters._MetallicFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Test\\Metallic.png";
		parameters._AmbientOcclusionFile = nullptr;
		parameters._VariantFile = nullptr;

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}
}

/*
*	Creates the terrain resource collection.
*/
void ClairvoyantResourceCreation::CreateTerrainResourceCollection() NOEXCEPT
{
	//Create the terrain resource collection.
	ResourceCollectionCreator::ResourceCollectionCreationParameters parameters;

	parameters._Output = "..\\..\\..\\Resources\\Final\\TerrainResourceCollection";
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Cliff_Snow_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Grass_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Grass_2_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Sand_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Snow_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Test_Material.cr");

	ResourceCollectionCreator::CreateResourceCollection(parameters);
}