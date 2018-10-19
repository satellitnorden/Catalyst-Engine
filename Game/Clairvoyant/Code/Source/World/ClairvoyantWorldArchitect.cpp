//Header file.
#include <World/ClairvoyantWorldArchitect.h>

//Clairvoyant.
#include <World/TimeOfDaySystem.h>

//Core.
#include <Core/General/HashString.h>

//Entities.
#include <Entities/ParticleSystemEntity.h>
#include <Entities/InitializationData/ParticleSystemInitializationData.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/PerlinNoiseGenerator.h>

//Managers.
#include <Managers/EnvironmentManager.h>
#include <Managers/RenderingConfigurationManager.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/UpdateSystem.h>
#include <Systems/VegetationSystem.h>

//Terrain.
#include <Terrain/TerrainUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantWorldArchitect);

//Clairvoyant world architects constants.
namespace ClairvoyantWorldArchitectConstants
{
	constexpr float GRASS_VEGETATION_MEDIUM_DETAIL_DISTANCE{ 62.5f };
	constexpr float GRASS_VEGETATION_LOW_DETAIL_DISTANCE{ 125.0f };

	constexpr float SOLID_VEGETATION_MEDIUM_DETAIL_DISTANCE{ 100.0f };
	constexpr float SOLID_VEGETATION_LOW_DETAIL_DISTANCE{ 200.0f };
}

/*
*	Initializes Clairvoyant the world architect.
*/
void ClairvoyantWorldArchitect::Initialize() NOEXCEPT
{
	//Initialize the environment parameters.
	InitializeEnvironmentParameters();

	//Initialize the particles.
	InitializeParticles();

	//Initialize the vegetation.
	InitializeVegetation();

	//Register the Clairvoyant world architect for updates.
	UpdateSystem::Instance->RegisterAsynchronousLogicUpdate(this);
}

/*
*	Updates the Clairvoyant world architect asynchronously during the logic update phase.
*/
bool ClairvoyantWorldArchitect::LogicUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the environment.
	UpdateEnvironment();

	//Return that the Clairvoyant world architect wants to continue to receive updates.
	return true;
}

/*
*	Initializes the environment parameters.
*/
void ClairvoyantWorldArchitect::InitializeEnvironmentParameters() NOEXCEPT
{
	constexpr float MINIMUM_SUN_INTENSITY{ 7.5f };
	constexpr float MAXIMUM_SUN_INTENSITY{ 15.0f };

	constexpr float MINIMUM_WIND_SPEED{ 4.0f };
	constexpr float MAXIMUM_WIND_SPEED{ 6.0f };

	constexpr float MINIMUM_VOLUMETRIC_FOG_DISTANCE{ 100'000.0f };
	constexpr float MAXIMUM_VOLUMETRIC_FOG_DISTANCE{ 200'000.0f };

	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._EnvironmentMaterial = ResourceLoader::GetEnvironmentMaterial(HashString("NightEnvironmentMaterial"));
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._SunColor = Vector3(0.75f, 0.75f, 1.0f);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._SunIntensity = 0.0f;
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._WindSpeed = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_SPEED, MAXIMUM_WIND_SPEED);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._VolumetricFogDistance = CatalystBaseMath::RandomFloatInRange(MINIMUM_VOLUMETRIC_FOG_DISTANCE, MAXIMUM_VOLUMETRIC_FOG_DISTANCE);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._EnvironmentMaterial = ResourceLoader::GetEnvironmentMaterial(HashString("MorningEnvironmentMaterial"));
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._SunColor = Vector3(1.0f, 0.75f, 0.75f);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._SunIntensity = 0.1f;
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._WindSpeed = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_SPEED, MAXIMUM_WIND_SPEED);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._VolumetricFogDistance = CatalystBaseMath::RandomFloatInRange(MINIMUM_VOLUMETRIC_FOG_DISTANCE, MAXIMUM_VOLUMETRIC_FOG_DISTANCE);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._EnvironmentMaterial = RenderingSystem::Instance->GetCommonEnvironmentMaterial(RenderingSystem::CommonEnvironmentMaterial::Day);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._SunColor = Vector3(1.0f, 1.0f, 0.75f);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._SunIntensity = CatalystBaseMath::RandomFloatInRange(MINIMUM_SUN_INTENSITY, MAXIMUM_SUN_INTENSITY);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._WindSpeed = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_SPEED, MAXIMUM_WIND_SPEED);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._VolumetricFogDistance = CatalystBaseMath::RandomFloatInRange(MINIMUM_VOLUMETRIC_FOG_DISTANCE, MAXIMUM_VOLUMETRIC_FOG_DISTANCE);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._EnvironmentMaterial = ResourceLoader::GetEnvironmentMaterial(HashString("EveningEnvironmentMaterial"));
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._SunColor = Vector3(0.75f, 1.0f, 1.0f);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._SunIntensity = 0.1f;
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._WindSpeed = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_SPEED, MAXIMUM_WIND_SPEED);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._VolumetricFogDistance = CatalystBaseMath::RandomFloatInRange(MINIMUM_VOLUMETRIC_FOG_DISTANCE, MAXIMUM_VOLUMETRIC_FOG_DISTANCE);
}

/*
*	Initializes the particles.
*/
void ClairvoyantWorldArchitect::InitializeParticles()
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
		data->_ParticleSystemProperties._MinimumPosition = Vector3(-25.0f, -25.0f, -25.0f);
		data->_ParticleSystemProperties._MaximumPosition = Vector3(25.0f, 25.0f, 25.0f);
		data->_ParticleSystemProperties._MinimumVelocity = Vector3(-0.25f, -0.25f, -0.25f);
		data->_ParticleSystemProperties._MaximumVelocity = Vector3(0.25f, 0.25f, 0.25f);
		data->_Position = AxisAlignedBoundingBox::CalculateCenter(box);

		ParticleSystemEntity *const RESTRICT particles = EntitySystem::Instance->CreateEntity<ParticleSystemEntity>();

		EntitySystem::Instance->RequestInitialization(particles, data, true);

		entities->EmplaceSlow(particles);
	}, 50.0f);
}

/*
*	Initializes the vegetation.
*/
void ClairvoyantWorldArchitect::InitializeVegetation()
{	
	{
		//Add the common fern grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantWorldArchitectConstants::GRASS_VEGETATION_MEDIUM_DETAIL_DISTANCE * 2.0f;
		properties._LowDetailDistance = ClairvoyantWorldArchitectConstants::GRASS_VEGETATION_LOW_DETAIL_DISTANCE * 2.0f;
		properties._CutoffDistance = 500.0f;
		properties._Density = 50;
		properties._Thickness = 1.0f;
		properties._WindModulatorFactor = 0.05f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldArchitect::Instance->GenerateTransformation(false, false, false, 0.0f, 0.25f, 0.035f, 0.07f, Vector3(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		StaticArray<GrassModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("LowDetailCommonFernModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("MediumDetailCommonFernModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("HighDetailCommonFernModel"));

		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("CommonFernGrassMaterial")) };

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);
	}

	{
		//Add the default grass grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantWorldArchitectConstants::GRASS_VEGETATION_MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantWorldArchitectConstants::GRASS_VEGETATION_LOW_DETAIL_DISTANCE;
		properties._Density = 2'500;
		properties._Thickness = 0.25f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldArchitect::Instance->GenerateTransformation(false, false, false, 0.5f, 0.25f, 0.25f, 0.5f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};

		StaticArray<GrassModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;
		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("DefaultGrassMaterial")) };

		properties._CutoffDistance = 62.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);

		properties._CutoffDistance = 125.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);

		properties._CutoffDistance = 250.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);
	}

	{
		//Add the snow grass grass vegetation type.
		GrassVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantWorldArchitectConstants::GRASS_VEGETATION_MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantWorldArchitectConstants::GRASS_VEGETATION_LOW_DETAIL_DISTANCE;
		properties._Density = 500;
		properties._Thickness = 0.25f;
		properties._WindModulatorFactor = 0.15f;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldArchitect::Instance->GenerateTransformation(false, true, true, 0.5f, 0.25f, 0.25f, 0.5f, Vector3(0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f), 0.0f), box, transformation);
		};

		StaticArray<GrassModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;
		GrassVegetationMaterial material{ ResourceLoader::GetGrassVegetationMaterial(HashString("SnowGrassMaterial")) };

		properties._CutoffDistance = 62.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("HighDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);

		properties._CutoffDistance = 125.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("MediumDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);

		properties._CutoffDistance = 250.5f;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetGrassModel(HashString("LowDetailDefaultGrassModel"));

		VegetationSystem::Instance->AddGrassVegetationType(properties, models, material);
	}

	{
		//Add the rock volcanic solid vegetation type.
		SolidVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantWorldArchitectConstants::SOLID_VEGETATION_MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantWorldArchitectConstants::SOLID_VEGETATION_LOW_DETAIL_DISTANCE;
		properties._CutoffDistance = 1'000.0f;
		properties._Density = 25;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldArchitect::Instance->GenerateTransformation(true, true, false, 0.0f, 0.8f, 0.15f, 0.3f, Vector3(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};

		StaticArray<PhysicalModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetPhysicalModel(HashString("LowDetailRockVolcanicModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetPhysicalModel(HashString("MediumDetailRockVolcanicModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetPhysicalModel(HashString("HighDetailRockVolcanicModel"));

		PhysicalMaterial material{ ResourceLoader::GetPhysicalMaterial(HashString("RockVolcanicMaterial")) };

		VegetationSystem::Instance->AddSolidVegetationType(properties, models, material);
	}

	{
		//Add the tree stump solid vegetation type.
		SolidVegetationTypeProperties properties;

		properties._MediumDetailDistance = ClairvoyantWorldArchitectConstants::SOLID_VEGETATION_MEDIUM_DETAIL_DISTANCE;
		properties._LowDetailDistance = ClairvoyantWorldArchitectConstants::SOLID_VEGETATION_LOW_DETAIL_DISTANCE;
		properties._CutoffDistance = 1'000.0f;
		properties._Density = 150;
		properties._PlacementFunction = [](const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation)
		{
			return ClairvoyantWorldArchitect::Instance->GenerateTransformation(false, true, false, 0.0f, 0.9f, 0.035f, 0.07f, Vector3(-90.0f, 0.0f, CatalystBaseMath::RandomFloatInRange(-180.0f, 180.0f)), box, transformation);
		};
		
		StaticArray<PhysicalModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> models;

		models[UNDERLYING(VegetationLevelOfDetail::Low)] = ResourceLoader::GetPhysicalModel(HashString("LowDetailTreeStumpModel"));
		models[UNDERLYING(VegetationLevelOfDetail::Medium)] = ResourceLoader::GetPhysicalModel(HashString("MediumDetailTreeStumpModel"));
		models[UNDERLYING(VegetationLevelOfDetail::High)] = ResourceLoader::GetPhysicalModel(HashString("HighDetailTreeStumpModel"));

		PhysicalMaterial material{ ResourceLoader::GetPhysicalMaterial(HashString("TreeStumpMaterial")) };

		VegetationSystem::Instance->AddSolidVegetationType(properties, models, material);
	}
}

/*
*	Generates a transformation.
*/
bool ClairvoyantWorldArchitect::GenerateTransformation(const bool underwater, const bool snow, const bool onlySnow, const float height, const float dotModulator, const float minimumScale, const float maximumScale, const Vector3 &randomRotation, const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation) NOEXCEPT
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

	if (!snow && terrainHeight > 8'000.0f)
	{
		return false;
	}

	else if (!snow && terrainHeight > 7'000.0f)
	{
		if (CatalystBaseMath::RandomChance((terrainHeight - 7'000.0f) * 0.001f))
		{
			return false;
		}
	}

	if (onlySnow && terrainHeight < 7'000.0f)
	{
		return false;
	}

	if (onlySnow && terrainHeight < 8'000.0f)
	{
		if (CatalystBaseMath::RandomChance(1.0f - (terrainHeight - 7'000.0f) * 0.001f))
		{
			return false;
		}
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

/*
*	Updates the environment.
*/
void ClairvoyantWorldArchitect::UpdateEnvironment() NOEXCEPT
{
	//Get the current time.
	const float currentTime{ TimeOfDaySystem::Instance->GetCurrentTime() };

	//If the current time has exceeded midnight, re-initialize the environment parameters.
	if (currentTime < _CurrentTime)
	{
		InitializeEnvironmentParameters();
	}

	_CurrentTime = currentTime;

	//Determine which environment parameters to blend between.
	if (currentTime < 6.0f)
	{
		BlendEnvironmentParameters(	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)],
									_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)],
									currentTime / 6.0f);
	}

	else if (currentTime < 12.0f)
	{
		BlendEnvironmentParameters(	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)],
									_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)],
									(currentTime - 6.0f) / 6.0f);
	}

	else if (currentTime < 18.0f)
	{
		BlendEnvironmentParameters(	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)],
									_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)],
									(currentTime - 12.0f) / 6.0f);
	}

	else
	{
		BlendEnvironmentParameters(	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)],
									_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)],
									(currentTime - 18.0f) / 6.0f);
	}
}

/*
*	Blends between two environment parameters.
*/
void ClairvoyantWorldArchitect::BlendEnvironmentParameters(const EnvironmentParameters &first, const EnvironmentParameters &second, const float alpha) NOEXCEPT
{
	//Smooth the alpha a bit.
	const float smoothedAlpha{ CatalystBaseMath::SmoothStep<1>(alpha) };

	//Blend the environment materials.
	EnvironmentManager::Instance->SetNightEnvironmentMaterial(first._EnvironmentMaterial);
	EnvironmentManager::Instance->SetDayEnvironmentMaterial(second._EnvironmentMaterial);
	EnvironmentManager::Instance->SetEnvironmentBlend(smoothedAlpha);

	//Blend the sun color.
	TimeOfDaySystem::Instance->GetSun()->SetColor(Vector3::LinearlyInterpolate(first._SunColor, second._SunColor, smoothedAlpha));

	//Blend the sun intensity.
	TimeOfDaySystem::Instance->GetSun()->SetIntensity(CatalystBaseMath::LinearlyInterpolate(first._SunIntensity, second._SunIntensity, smoothedAlpha));

	//Blend the wind speed.
	PhysicsSystem::Instance->SetWindSpeed(CatalystBaseMath::LinearlyInterpolate(first._WindSpeed, second._WindSpeed, smoothedAlpha));

	//Blend the volumetric fog distance.
	RenderingConfigurationManager::Instance->SetVolumetricFogDistance(CatalystBaseMath::LinearlyInterpolate(first._VolumetricFogDistance, second._VolumetricFogDistance, smoothedAlpha));
}