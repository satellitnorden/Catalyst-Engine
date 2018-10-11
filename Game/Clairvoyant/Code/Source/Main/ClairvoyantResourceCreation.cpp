//Header file.
#include <Main/ClairvoyantResourceCreation.h>

//Resource creator.
#include <EnvironmentMaterialCreator.h>
#include <GrassMaterialCreator.h>
#include <GrassModelCreator.h>
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
			//Create the day environment material.
			EnvironmentMaterialCreator::EnvironmentMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\DayEnvironmentMaterial";
			parameters._ID = "DayEnvironmentMaterial";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Day.hdr";
			parameters._DiffuseResolution = 1'024;
			parameters._DiffuseIrradianceResolution = 2;

			EnvironmentMaterialCreator::CreateEnvironmentMaterial(parameters);
		}

		{
			//Create the night environment material.
			EnvironmentMaterialCreator::EnvironmentMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\NightEnvironmentMaterial";
			parameters._ID = "NightEnvironmentMaterial";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Textures\\Environment\\Night.hdr";
			parameters._DiffuseResolution = 1'024;
			parameters._DiffuseIrradianceResolution = 2;

			EnvironmentMaterialCreator::CreateEnvironmentMaterial(parameters);
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
			//Create the default grass material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultGrassMaterial";
			parameters._ID = "DefaultGrassMaterial";
			parameters._MaskMipmapLevels = 1;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\DefaultGrass\\Mask.png";
			parameters._MipmapLevels = 8;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\DefaultGrass\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\DefaultGrass\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the fern grass material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\FernGrassMaterial";
			parameters._ID = "FernGrassMaterial";
			parameters._MaskMipmapLevels = 8;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Fern\\Mask.png";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Fern\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Fern\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the plum grass material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\PlumGrassMaterial";
			parameters._ID = "PlumGrassMaterial";
			parameters._MaskMipmapLevels = 1;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Plum\\Mask.png";
			parameters._MipmapLevels = 8;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Plum\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Plum\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the thistle grass material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\ThistleGrassMaterial";
			parameters._ID = "ThistleGrassMaterial";
			parameters._MaskMipmapLevels = 1;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Thistle\\Mask.png";
			parameters._MipmapLevels = 8;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Thistle\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Thistle\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the weed grass material.
			GrassMaterialCreator::GrassMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\WeedGrassMaterial";
			parameters._ID = "WeedGrassMaterial";
			parameters._MaskMipmapLevels = 1;
			parameters._MaskFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Weed\\Mask.png";
			parameters._MipmapLevels = 8;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Weed\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Weed\\NormalMap.png";

			GrassMaterialCreator::CreateGrassMaterial(parameters);
		}

		{
			//Create the fern grass model.
			GrassModelCreator::GrassModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\FernGrassModel";
			parameters._ID = "FernGrassModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\FernGrassModel.fbx";
			parameters._UpAxis = GrassModelCreator::GrassModelCreationParameters::Axis::Z;

			GrassModelCreator::CreateGrassModel(parameters);
		}

		{
			//Create the grass grass model.
			GrassModelCreator::GrassModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\DefaultGrassModel";
			parameters._ID = "DefaultGrassModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\DefaultGrassModel.fbx";
			parameters._UpAxis = GrassModelCreator::GrassModelCreationParameters::Axis::Y;

			GrassModelCreator::CreateGrassModel(parameters);
		}

		{
			//Create the rock physical material.
			PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\RockPhysicalMaterial";
			parameters._ID = "RockPhysicalMaterial";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Rock\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Rock\\NormalMap.png";
			parameters._RoughnessFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Rock\\Roughness.png";
			parameters._MetallicFile = nullptr;
			parameters._AmbientOcclusionFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Rock\\AmbientOcclusion.png";
			parameters._ThicknessFile = nullptr;

			PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
		}

		{
			//Create the tree physical material.
			PhysicalMaterialCreator::PhysicalMaterialCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Materials\\TreePhysicalMaterial";
			parameters._ID = "TreePhysicalMaterial";
			parameters._MipmapLevels = 9;
			parameters._AlbedoFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Tree\\Albedo.png";
			parameters._NormalMapFile = "..\\..\\..\\Resources\\Raw\\Textures\\Vegetation\\Tree\\NormalMap.png";
			parameters._RoughnessFile = nullptr;
			parameters._MetallicFile = nullptr;
			parameters._AmbientOcclusionFile = nullptr;
			parameters._ThicknessFile = nullptr;

			PhysicalMaterialCreator::CreatePhysicalMaterial(parameters);
		}

		{
			//Create the rock physical model.
			PhysicalModelCreator::PhysicalModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\RockPhysicalModel";
			parameters._ID = "RockPhysicalModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\RockPhysicalModel.fbx";

			PhysicalModelCreator::CreatePhysicalModel(parameters);
		}

		{
			//Create the tree physical model.
			PhysicalModelCreator::PhysicalModelCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Intermediate\\Models\\TreePhysicalModel";
			parameters._ID = "TreePhysicalModel";
			parameters._File = "..\\..\\..\\Resources\\Raw\\Models\\Vegetation\\TreePhysicalModel.fbx";

			PhysicalModelCreator::CreatePhysicalModel(parameters);
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

			TerrainMaterialCreator::CreateTerrainMaterial(parameters);
		}

		{
			//Create the resource collection.
			ResourceCollectionCreator::ResourceCollectionCreationParameters parameters;

			parameters._Output = "..\\..\\..\\Resources\\Final\\ClairvoyantResourceCollection";
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DayEnvironmentMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\NightEnvironmentMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultOceanMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultGrassMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\FernGrassMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\PlumGrassMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\ThistleGrassMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\WeedGrassMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\FernGrassModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\DefaultGrassModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\RockPhysicalMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\TreePhysicalMaterial.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\RockPhysicalModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Models\\TreePhysicalModel.cr");
			parameters._Resources.EmplaceSlow("..\\..\\..\\Resources\\Intermediate\\Materials\\DefaultTerrainMaterial.cr");

			ResourceCollectionCreator::CreateResourceCollection(parameters);
		}
	}

}