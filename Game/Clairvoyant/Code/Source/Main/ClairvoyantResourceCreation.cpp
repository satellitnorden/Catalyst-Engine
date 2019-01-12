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

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\NightEnvironmentMaterial";
		parameters._ID = "NightEnvironmentMaterial";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Night.png";
		parameters._DiffuseResolution = 1'024;
		parameters._DiffuseIrradianceResolution = 2;

		//EnvironmentMaterialCreator::CreateEnvironmentMaterial(parameters);
	}

	{
		//Create the morning environment material.
		EnvironmentMaterialCreator::EnvironmentMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\MorningEnvironmentMaterial";
		parameters._ID = "MorningEnvironmentMaterial";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Morning.hdr";
		parameters._DiffuseResolution = 1'024;
		parameters._DiffuseIrradianceResolution = 2;

		//EnvironmentMaterialCreator::CreateEnvironmentMaterial(parameters);
	}

	{
		//Create the evening environment material.
		EnvironmentMaterialCreator::EnvironmentMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\EveningEnvironmentMaterial";
		parameters._ID = "EveningEnvironmentMaterial";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Evening.hdr";
		parameters._DiffuseResolution = 1'024;
		parameters._DiffuseIrradianceResolution = 2;

		//EnvironmentMaterialCreator::CreateEnvironmentMaterial(parameters);
	}

	{
		//Create the ocean material.
		OceanMaterialCreator::OceanMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\DefaultOceanMaterial";
		parameters._ID = "DefaultOceanMaterial";
		parameters._MipmapLevels = 9;
		parameters._Normal = "..\\..\\..\\Resources\\Raw\\Textures\\Ocean\\Normal.jpg";
		parameters._Foam = "..\\..\\..\\Resources\\Raw\\Textures\\Ocean\\Foam.jpg";

		//OceanMaterialCreator::CreateOceanMaterial(parameters);
	}

	//CreateGrassVegetationIntermediateResources();
	//CreateSolidVegetationIntermediateResources();
	//CreateLocationIntermediateResources();
	CreateTerrainIntermediateResources();
#endif

#if CREATE_RESOURCE_COLLECTION
	{
		//Create the resource collection.
		ResourceCollectionCreator::ResourceCollectionCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Final\\ClairvoyantResourceCollection";
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\NightEnvironmentMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\MorningEnvironmentMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\EveningEnvironmentMaterial.cr");
		parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\DefaultOceanMaterial.cr");

		//ResourceCollectionCreator::CreateResourceCollection(parameters);
	}

	//CreateGrassVegetationResourceCollection();
	//CreateSolidVegetationResourceCollection();
	//CreateLocationResourceCollection();
	CreateTerrainResourceCollection();
#endif
}

/*
*	Creates the grass vegetation intermediate resources.
*/
void ClairvoyantResourceCreation::CreateGrassVegetationIntermediateResources() NOEXCEPT
{
	{
		//Create the flower 1 material.
		GrassMaterialCreator::GrassMaterialCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_1_Material";
		parameters._ID = "GrassVegetation_Flower_1_Material";
		parameters._MaskMipmapLevels = 3;
		parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_1\\Mask.png";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_1\\NormalMap.png";
		GrassMaterialCreator::CreateGrassMaterial(parameters);
	}

	{
		//Create the flower 2 material.
		GrassMaterialCreator::GrassMaterialCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_2_Material";
		parameters._ID = "GrassVegetation_Flower_2_Material";
		parameters._MaskMipmapLevels = 3;
		parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_2\\Mask.png";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_2\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_2\\NormalMap.png";
		GrassMaterialCreator::CreateGrassMaterial(parameters);
	}

	{
		//Create the flower 3 material.
		GrassMaterialCreator::GrassMaterialCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_3_Material";
		parameters._ID = "GrassVegetation_Flower_3_Material";
		parameters._MaskMipmapLevels = 3;
		parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_3\\Mask.png";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_3\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_3\\NormalMap.png";
		GrassMaterialCreator::CreateGrassMaterial(parameters);
	}

	{
		//Create the flower 4 material.
		GrassMaterialCreator::GrassMaterialCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_4_Material";
		parameters._ID = "GrassVegetation_Flower_4_Material";
		parameters._MaskMipmapLevels = 3;
		parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_4\\Mask.png";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_4\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_4\\NormalMap.png";
		GrassMaterialCreator::CreateGrassMaterial(parameters);
	}

	{
		//Create the flower 5 material.
		GrassMaterialCreator::GrassMaterialCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_5_Material";
		parameters._ID = "GrassVegetation_Flower_5_Material";
		parameters._MaskMipmapLevels = 3;
		parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_5\\Mask.png";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_5\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Flower_5\\NormalMap.png";
		GrassMaterialCreator::CreateGrassMaterial(parameters);
	}

	{
		//Create the grass 1 material.
		GrassMaterialCreator::GrassMaterialCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Grass_1_Material";
		parameters._ID = "GrassVegetation_Grass_1_Material";
		parameters._MaskMipmapLevels = 3;
		parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Grass_1\\Mask.png";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Grass_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\GrassVegetation\\Grass_1\\NormalMap.png";
		GrassMaterialCreator::CreateGrassMaterial(parameters);
	}

	{
		//Create the default grass vegetation model.
		GrassVegetationModelCreator::GrassVegetationModelCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Default_Model";
		parameters._ID = "GrassVegetation_Default_Model";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\GrassVegetation\\GrassVegetation_Default_Model.fbx";
		parameters._UpAxis = GrassVegetationModelCreator::GrassVegetationModelCreationParameters::Axis::Z;
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
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_2_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_3_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_4_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Flower_5_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Grass_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\GrassVegetation_Default_Model.cr");

	ResourceCollectionCreator::CreateResourceCollection(parameters);
}

/*
*	Creates the solid vegetation intermediate resources.
*/
void ClairvoyantResourceCreation::CreateSolidVegetationIntermediateResources() NOEXCEPT
{
	{
		//Create the rock 1 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\SolidVegetation_Rock_1_Material";
		parameters._ID = "SolidVegetation_Rock_1_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Rock_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Rock_1\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Rock_1\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Rock_1\\AmbientOcclusion.png";
		parameters._VariantFile = nullptr;
		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the tree 1 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;
		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\SolidVegetation_Tree_1_Material";
		parameters._ID = "SolidVegetation_Tree_1_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Tree_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Tree_1\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Tree_1\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Tree_1\\AmbientOcclusion.png";
		parameters._VariantFile = nullptr;
		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the rock 1 model.
		PhysicalModelCreator::PhysicalModelCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\SolidVegetation_Rock_1_Model";
		parameters._ID = "SolidVegetation_Rock_1_Model";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Rock_1\\Model.fbx";

		PhysicalModelCreator::CreatePhysicalModel(parameters);
	}

	{
		//Create the tree 1 model.
		PhysicalModelCreator::PhysicalModelCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\SolidVegetation_Tree_1_Model";
		parameters._ID = "SolidVegetation_Tree_1_Model";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\SolidVegetation\\Tree_1\\Model.fbx";

		PhysicalModelCreator::CreatePhysicalModel(parameters);
	}
}

/*
*	Creates the solid vegetation resource collection.
*/
void ClairvoyantResourceCreation::CreateSolidVegetationResourceCollection() NOEXCEPT
{
	//Create the resource collection.
	ResourceCollectionCreator::ResourceCollectionCreationParameters parameters;

	parameters._Output = "..\\..\\..\\Resources\\Final\\ClairvoyantSolidVegetationResourceCollection";
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\SolidVegetation_Rock_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\SolidVegetation_Tree_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\SolidVegetation_Rock_1_Model.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\SolidVegetation_Tree_1_Model.cr");

	ResourceCollectionCreator::CreateResourceCollection(parameters);
}

/*
*	Creates the location intermediate resources.
*/
void ClairvoyantResourceCreation::CreateLocationIntermediateResources() NOEXCEPT
{
	{
		//Create the barrel material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\BarrelMaterial";
		parameters._ID = "BarrelMaterial";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Location\\Barrel\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Location\\Barrel\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Location\\Barrel\\Roughness.png";
		parameters._MetallicFile = "..\\..\\..\\Resources\\Raw\\Textures\\Location\\Barrel\\Metallic.png";
		parameters._AmbientOcclusionFile = nullptr;
		parameters._VariantFile = nullptr;

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the box material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\BoxMaterial";
		parameters._ID = "BoxMaterial";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Location\\Box\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Location\\Box\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Location\\Box\\Roughness.png";
		parameters._MetallicFile = "..\\..\\..\\Resources\\Raw\\Textures\\Location\\Box\\Metallic.png";
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Location\\Box\\AmbientOcclusion.png";
		parameters._VariantFile = nullptr;

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the barrel model.
		PhysicalModelCreator::PhysicalModelCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\BarrelModel";
		parameters._ID = "BarrelModel";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Location\\Barrel.fbx";

		PhysicalModelCreator::CreatePhysicalModel(parameters);
	}

	{
		//Create the box model.
		PhysicalModelCreator::PhysicalModelCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\BoxModel";
		parameters._ID = "BoxModel";
		parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Location\\Box.fbx";

		PhysicalModelCreator::CreatePhysicalModel(parameters);
	}
}

/*
*	Creates the location resource collection.
*/
void ClairvoyantResourceCreation::CreateLocationResourceCollection() NOEXCEPT
{
	//Create the resource collection.
	ResourceCollectionCreator::ResourceCollectionCreationParameters parameters;

	parameters._Output = "..\\..\\..\\Resources\\Final\\ClairvoyantLocationResourceCollection";
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\BarrelMaterial.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\BoxMaterial.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\BarrelModel.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\BoxModel.cr");

	ResourceCollectionCreator::CreateResourceCollection(parameters);
}

/*
*	Creates the terrain intermediate resources.
*/
void ClairvoyantResourceCreation::CreateTerrainIntermediateResources() NOEXCEPT
{
	{
		//Create the terrain grass 1 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Terrain_Grass_1_Material";
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
		//Create the terrain rock 1 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Terrain_Rock_1_Material";
		parameters._ID = "Terrain_Rock_1_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Rock_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Rock_1\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Rock_1\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Rock_1\\AmbientOcclusion.png";
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Rock_1\\Displacement.png";

		PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
	}

	{
		//Create the terrain sand 1 material.
		PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Terrain_Sand_1_Material";
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

		parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Terrain_Snow_1_Material";
		parameters._ID = "Terrain_Snow_1_Material";
		parameters._MipmapLevels = 9;
		parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow_1\\Albedo.png";
		parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow_1\\NormalMap.png";
		parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow_1\\Roughness.png";
		parameters._MetallicFile = nullptr;
		parameters._AmbientOcclusionFile = nullptr;
		parameters._VariantFile = "..\\..\\..\\Resources\\Raw\\Textures\\Terrain\\Snow_1\\Displacement.png";

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
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Terrain_Grass_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Terrain_Rock_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Terrain_Sand_1_Material.cr");
	parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Terrain_Snow_1_Material.cr");

	ResourceCollectionCreator::CreateResourceCollection(parameters);
}