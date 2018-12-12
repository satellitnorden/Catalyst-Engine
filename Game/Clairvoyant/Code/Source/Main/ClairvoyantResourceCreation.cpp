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
		//Create the barrel material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\BarrelMaterial";
		parameters._ID = "BarrelMaterial";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Props\\Barrel\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Props\\Barrel\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Props\\Barrel\\Roughness.png";
		parameters._MetallicFile = "..\\..\\..\\Resources\\Raw\\Textures\\Props\\Barrel\\Metallic.png";
		parameters._AmbientOcclusionFile = nullptr;
		parameters._VariantFile = nullptr;

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
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
		//Create the barrel model.
		PhysicalModelCreator::PhysicalModelCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\BarrelModel";
		parameters._ID = "BarrelModel";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Props\\Barrel.fbx";

		PhysicalModelCreator::CreatePhysicalModel(parameters);
	}

	{
		//Create the test model.
		PhysicalModelCreator::PhysicalModelCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\TestModel";
		parameters._ID = "TestModel";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Test\\Model.obj";

		PhysicalModelCreator::CreatePhysicalModel(parameters);
	}

	CreateGrassVegetationIntermediateResources();
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
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\BarrelMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\TowerMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\BarrelModel.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\TestModel.cr");

		ResourceCollectionCreator::CreateResourceCollection(parameters);
	}

	CreateGrassVegetationResourceCollection();
	CreateTerrainResourceCollection();
#endif
}

/*
*	Creates the grass vegetation intermediate resources.
*/
void ClairvoyantResourceCreation::CreateGrassVegetationIntermediateResources() NOEXCEPT
{
	{
		//Create the default grass material.
		GrassMaterialCreator::GrassMaterialCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Vegetation\\DefaultGrassVegetationMaterial";
		parameters._ID = "DefaultGrassVegetationMaterial";
		parameters._MaskMipmapLevels = 2;
		parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\DefaultGrass\\Mask.png";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\DefaultGrass\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\DefaultGrass\\NormalMap.png";
		GrassMaterialCreator::CreateGrassMaterial(parameters);
	}

	{
		//Create the low detail default grass vegetation model.
		GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\Vegetation\\LowDetailDefaultGrassVegetationModel";
		parameters._ID = "LowDetailDefaultGrassVegetationModel";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\LowDetailDefaultGrassVegetationModel.fbx";
		parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::Y;
		GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
	}

	{
		//Create the medium detail default grass vegetation model.
		GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\Vegetation\\MediumDetailDefaultGrassVegetationModel";
		parameters._ID = "MediumDetailDefaultGrassVegetationModel";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\MediumDetailDefaultGrassVegetationModel.fbx";
		parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::Y;
		GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
	}

	{
		//Create the high detail default grass vegetation model.
		GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\Vegetation\\HighDetailDefaultGrassVegetationModel";
		parameters._ID = "HighDetailDefaultGrassVegetationModel";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\HighDetailDefaultGrassVegetationModel.fbx";
		parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::Y;
		GrassVegetationModelCreator::CreateGrassVegetationModel(parameters);
	}
}

/*
*	Creates the grass vegetation resource collection.
*/
void ClairvoyantResourceCreation::CreateGrassVegetationResourceCollection() NOEXCEPT
{
	//Create the resource collection.
	ResourceCollectionCreator::ResourceCollectionCreationParameters parameters;

	parameters._Output = "..\\..\\..\\Resources\\Final\\ClairvoyantGrassVegetationResourceCollection";
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Vegetation\\DefaultGrassVegetationMaterial.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\Vegetation\\HighDetailDefaultGrassVegetationModel.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\Vegetation\\MediumDetailDefaultGrassVegetationModel.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\Vegetation\\LowDetailDefaultGrassVegetationModel.cr");

	ResourceCollectionCreator::CreateResourceCollection(parameters);
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
		//Create the terrain cliff snow 2 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Cliff_Snow_2_Material";
		parameters._ID = "Terrain_Cliff_Snow_2_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_2\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_2\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_2\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_2\\AmbientOcclusion.png";
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Cliff_Snow_2\\Displacement.png";

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
		//Create the terrain sand 2 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Sand_2_Material";
		parameters._ID = "Terrain_Sand_2_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_2\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_2\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_2\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_2\\AmbientOcclusion.png";
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_2\\Displacement.png";

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the terrain sand 3 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Sand_3_Material";
		parameters._ID = "Terrain_Sand_3_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_3\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_3\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_3\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_3\\AmbientOcclusion.png";
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Sand_3\\Displacement.png";

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

	parameters._Output = "..\\..\\..\\Resources\\Final\\ClairvoyantTerrainResourceCollection";
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Cliff_Snow_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Cliff_Snow_2_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Grass_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Grass_2_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Sand_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Sand_2_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Sand_3_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Snow_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\Terrain\\Terrain_Test_Material.cr");

	ResourceCollectionCreator::CreateResourceCollection(parameters);
}