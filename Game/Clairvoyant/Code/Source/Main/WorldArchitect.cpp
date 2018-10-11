//Header file.
#include <Main/WorldArchitect.h>

//Core.
#include <Core/General/HashString.h>

//Entities.
#include <Entities/ParticleSystemEntity.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoiseGenerator.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

//Terrain.
#include <Terrain/TerrainUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(WorldArchitect);

/*
*	Initializes the world architect.
*/
void WorldArchitect::Initialize() NOEXCEPT
{
	//Initialize the particles.
	InitializeParticles();

	//Initialize the vegetation.
	InitializeVegetation();
}

/*
*	Initializes the particles.
*/
void WorldArchitect::InitializeParticles()
{
	//Register a procedural placement function for the dust particles.
	EntitySystem::Instance->RegisterProceduralPlacementFunction([](const AxisAlignedBoundingBox &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities)
	{
		//Create the particles.
		ParticleSystemInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<ParticleSystemInitializationData>() };

		data->_Properties = EntityInitializationData::EntityProperty::None;
		data->_Material = RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle);
		data->_ParticleSystemProperties._Properties = static_cast<uint32>(ParticleSystemProperties::ParticleSystemProperty::AffectedByWind) | static_cast<uint32>(ParticleSystemProperties::ParticleSystemProperty::Looping);
		data->_ParticleSystemProperties._FadeTime = 2.5f;
		data->_ParticleSystemProperties._Lifetime = 10.0f;
		data->_ParticleSystemProperties._SpawnFrequency = 0.01f;
		data->_ParticleSystemProperties._MinimumScale = Vector2(0.025f, 0.025f);
		data->_ParticleSystemProperties._MaximumScale = Vector2(0.05f, 0.05f);
		data->_ParticleSystemProperties._MinimumPosition = Vector3(-50.0f, -50.0f, -50.0f);
		data->_ParticleSystemProperties._MaximumPosition = Vector3(50.0f, 50.0f, 50.0f);
		data->_ParticleSystemProperties._MinimumVelocity = Vector3(-0.25f, -0.25f, -0.25f);
		data->_ParticleSystemProperties._MaximumVelocity = Vector3(0.25f, 0.25f, 0.25f);
		data->_Position = AxisAlignedBoundingBox::CalculateCenter(box);

		ParticleSystemEntity *const RESTRICT particles = EntitySystem::Instance->CreateEntity<ParticleSystemEntity>();

		EntitySystem::Instance->RequestInitialization(particles, data, true);

		entities->EmplaceSlow(particles);
	}, 100.0f);
}

/*
*	Initializes the vegetation.
*/
void WorldArchitect::InitializeVegetation()
{
	{
		//Add the default grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._CutoffDistance = 50.0f;
		properties._Density = 10'000;
		properties._Thickness = 0.1f;
		properties._WindModulatorFactor = 0.2f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return WorldArchitect::Instance->GenerateTransformation(true, 0.5f, 0.25f, 0.3f, 0.6f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};
		GrassModel model{ ResourceLoader::GetGrassModel(HashString("DefaultGrassModel")) };
		GrassMaterial material{ ResourceLoader::GetGrassMaterial(HashString("DefaultGrassMaterial")) };

		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);

		properties._CutoffDistance = 100.0f;
		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);

		properties._CutoffDistance = 150.0f;
		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);

		properties._CutoffDistance = 200.0f;
		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);

		properties._CutoffDistance = 250.0f;
		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);
	}

	{
		//Add the fern vegetation type.
		GrassVegetationTypeProperties properties;

		properties._CutoffDistance = 125.0f;
		properties._Density = 100;
		properties._Thickness = 1.0f;
		properties._WindModulatorFactor = 0.1f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return WorldArchitect::Instance->GenerateTransformation(false, 0.0f, 0.25f, 0.035f, 0.07f, Vector3(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};
		GrassModel model{ ResourceLoader::GetGrassModel(HashString("FernGrassModel")) };
		GrassMaterial material{ ResourceLoader::GetGrassMaterial(HashString("FernGrassMaterial")) };

		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);

		properties._CutoffDistance = 250.0f;
		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);
	}

	{
		//Add the plum grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._CutoffDistance = 125.0f;
		properties._Density = 5'000;
		properties._Thickness = 0.1f;
		properties._WindModulatorFactor = 0.1f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return WorldArchitect::Instance->GenerateTransformation(true, 0.5f, 0.25f, 0.25f, 0.5f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};
		GrassModel model{ ResourceLoader::GetGrassModel(HashString("DefaultGrassModel")) };
		GrassMaterial material{ ResourceLoader::GetGrassMaterial(HashString("PlumGrassMaterial")) };

		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);

		properties._CutoffDistance = 250.0f;
		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);
	}

	{
		//Add the thistle grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._CutoffDistance = 125.0f;
		properties._Density = 5'000;
		properties._Thickness = 0.1f;
		properties._WindModulatorFactor = 0.3f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return WorldArchitect::Instance->GenerateTransformation(true, 0.5f, 0.25f, 0.35f, 0.7f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};
		GrassModel model{ ResourceLoader::GetGrassModel(HashString("DefaultGrassModel")) };
		GrassMaterial material{ ResourceLoader::GetGrassMaterial(HashString("ThistleGrassMaterial")) };

		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);

		properties._CutoffDistance = 250.0f;
		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);
	}

	{
		//Add the weed grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._CutoffDistance = 125.0f;
		properties._Density = 5'000;
		properties._Thickness = 0.1f;
		properties._WindModulatorFactor = 0.2f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return WorldArchitect::Instance->GenerateTransformation(true, 0.5f, 0.25f, 0.25f, 0.5f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};
		GrassModel model{ ResourceLoader::GetGrassModel(HashString("DefaultGrassModel")) };
		GrassMaterial material{ ResourceLoader::GetGrassMaterial(HashString("WeedGrassMaterial")) };

		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);

		properties._CutoffDistance = 250.0f;
		VegetationSystem::Instance->AddGrassVegetationType(properties, model, material);
	}
}

/*
*	Generates a transformation.
*/
bool WorldArchitect::GenerateTransformation(const bool underwater, const float height, const float dotModulator, const float minimumScale, const float maximumScale, const Vector3 &randomRotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT
{
	Vector3 position(CatalystBaseMath::RandomFloatInRange(box._Minimum._X, box._Maximum._X), 0.0f, CatalystBaseMath::RandomFloatInRange(box._Minimum._Z, box._Maximum._Z));
	
	float terrainHeight;

	if (!TerrainSystem::Instance->GetTerrainHeightAtPosition(position, &terrainHeight))
	{
		return false;
	}

	if (!underwater && terrainHeight < 0.0f)
	{
		return false;
	}

	position._Y = terrainHeight;

	Vector3 terrainNormal;

	if (!TerrainSystem::Instance->GetTerrainNormalAtPosition(position, &terrainNormal))
	{
		return false;
	}

	if (!CatalystBaseMath::RandomChance(CatalystBaseMath::Maximum<float>(Vector3::DotProduct(terrainNormal, Vector3::UP), 0.0f) - dotModulator))
	{
		return false;
	}

	position += terrainNormal * height;

	const float scale{ CatalystBaseMath::RandomFloatInRange(minimumScale, maximumScale) };

	transformation->SetTranslation(position);
	transformation->SetScale(Vector3(scale, scale, scale));

	Matrix4 rotationMatrix{ Matrix4::Orientation(terrainNormal, Vector3::UP) };
	rotationMatrix.Rotate(randomRotation);

	*transformation = *transformation * rotationMatrix;

	return true;
}