//Header file.
#include <World/ClairvoyantWorldArchitect.h>

//Clairvoyant.
#include <World/TimeOfDaySystem.h>

//Core.
#include <Core/General/HashString.h>

//Entities.
#include <Entities/Creation/DynamicPhysicalinitializationData.h>
#include <Entities/Creation/ParticleSystemInitializationData.h>
#include <Entities/Types/DynamicPhysicalEntity.h>
#include <Entities/Types/ParticleSystemEntity.h>

//Math.
#include <Math/CatalystBaseMath.h>

//Managers.
#include <Managers/EnvironmentManager.h>
#include <Managers/RenderingConfigurationManager.h>

//Rendering.
#include <Rendering/Engine/CPUTexture2D.h>

//Resources.
#include <Resources/ResourceLoader.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/EntityPlacementSystem.h>
#include <Systems/LightingSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/UpdateSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantWorldArchitect);

/*
*	Initializes Clairvoyant the world architect.
*/
void ClairvoyantWorldArchitect::Initialize() NOEXCEPT
{
	//Initialize the environment parameters.
	InitializeEnvironmentParameters();

	//Initialize the particles.
	InitializeParticles();

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
	constexpr float MINIMUM_SUN_INTENSITY{ 1.0f };
	constexpr float MAXIMUM_SUN_INTENSITY{ 100.0f };

	constexpr float MINIMUM_WIND_SPEED{ 2.0f };
	constexpr float MAXIMUM_WIND_SPEED{ 20.0f };

	constexpr float MINIMUM_VOLUMETRIC_FOG_DENSITY{ 0.0f };
	constexpr float MAXIMUM_VOLUMETRIC_FOG_DENSITY{ 0.025f };

	constexpr float MINIMUM_DEPTH_OF_FIELD_DISTANCE{ 4'915.2f };
	constexpr float MAXIMUM_DEPTH_OF_FIELD_DISTANCEY{ 49'152.0f };

	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._EnvironmentMaterial = ResourceLoader::GetEnvironmentMaterial(HashString("NightEnvironmentMaterial"));
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._SunColor = Vector3<float>(0.75f, 0.75f, 1.0f);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._SunIntensity = 0.0f;
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._WindSpeed = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_SPEED, MAXIMUM_WIND_SPEED);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._VolumetricFogDensity = CatalystBaseMath::RandomFloatInRange(MINIMUM_VOLUMETRIC_FOG_DENSITY, MAXIMUM_VOLUMETRIC_FOG_DENSITY);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Night)]._DepthOfFieldDistance = CatalystBaseMath::RandomFloatInRange(MINIMUM_DEPTH_OF_FIELD_DISTANCE, MAXIMUM_DEPTH_OF_FIELD_DISTANCEY);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._EnvironmentMaterial = ResourceLoader::GetEnvironmentMaterial(HashString("MorningEnvironmentMaterial"));
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._SunColor = Vector3<float>(1.0f, 0.75f, 0.75f);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._SunIntensity = 0.1f;
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._WindSpeed = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_SPEED, MAXIMUM_WIND_SPEED);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._VolumetricFogDensity = CatalystBaseMath::RandomFloatInRange(MINIMUM_VOLUMETRIC_FOG_DENSITY, MAXIMUM_VOLUMETRIC_FOG_DENSITY);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Morning)]._DepthOfFieldDistance = CatalystBaseMath::RandomFloatInRange(MINIMUM_DEPTH_OF_FIELD_DISTANCE, MAXIMUM_DEPTH_OF_FIELD_DISTANCEY);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._EnvironmentMaterial = RenderingSystem::Instance->GetCommonEnvironmentMaterial(RenderingSystem::CommonEnvironmentMaterial::Day);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._SunColor = Vector3<float>(1.0f, 1.0f, 0.75f);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._SunIntensity = CatalystBaseMath::RandomFloatInRange(MINIMUM_SUN_INTENSITY, MAXIMUM_SUN_INTENSITY);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._WindSpeed = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_SPEED, MAXIMUM_WIND_SPEED);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._VolumetricFogDensity = CatalystBaseMath::RandomFloatInRange(MINIMUM_VOLUMETRIC_FOG_DENSITY, MAXIMUM_VOLUMETRIC_FOG_DENSITY);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Day)]._DepthOfFieldDistance = CatalystBaseMath::RandomFloatInRange(MINIMUM_DEPTH_OF_FIELD_DISTANCE, MAXIMUM_DEPTH_OF_FIELD_DISTANCEY);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._EnvironmentMaterial = ResourceLoader::GetEnvironmentMaterial(HashString("EveningEnvironmentMaterial"));
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._SunColor = Vector3<float>(0.75f, 1.0f, 1.0f);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._SunIntensity = 0.1f;
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._WindSpeed = CatalystBaseMath::RandomFloatInRange(MINIMUM_WIND_SPEED, MAXIMUM_WIND_SPEED);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._VolumetricFogDensity = CatalystBaseMath::RandomFloatInRange(MINIMUM_VOLUMETRIC_FOG_DENSITY, MAXIMUM_VOLUMETRIC_FOG_DENSITY);
	_EnvironmentParameters[UNDERLYING(EnvironmentPhase::Evening)]._DepthOfFieldDistance = CatalystBaseMath::RandomFloatInRange(MINIMUM_DEPTH_OF_FIELD_DISTANCE, MAXIMUM_DEPTH_OF_FIELD_DISTANCEY);
}

/*
*	Initializes the particles.
*/
void ClairvoyantWorldArchitect::InitializeParticles()
{
	//Register a placement function for the dust particles.
	EntityPlacementSystem::Instance->RegisterThreeDimensionalPlacementFunction([](const AxisAlignedBoundingBox &box, DynamicArray<Entity *RESTRICT> *const RESTRICT entities)
	{
		//Create the particles.
		ParticleSystemInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<ParticleSystemInitializationData>() };

		data->_Properties = EntityInitializationData::EntityProperty::None;
		data->_Material = RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle);
		data->_ParticleSystemProperties._Properties = ParticleSystemProperty::AffectedByWind | ParticleSystemProperty::Looping;
		data->_ParticleSystemProperties._FadeTime = 2.5f;
		data->_ParticleSystemProperties._Lifetime = 10.0f;
		data->_ParticleSystemProperties._SpawnFrequency = 0.01f;
		data->_ParticleSystemProperties._MinimumScale = Vector2<float>(0.025f, 0.025f);
		data->_ParticleSystemProperties._MaximumScale = Vector2<float>(0.05f, 0.05f);
		data->_ParticleSystemProperties._MinimumPosition = Vector3<float>(-25.0f, -25.0f, -25.0f);
		data->_ParticleSystemProperties._MaximumPosition = Vector3<float>(25.0f, 25.0f, 25.0f);
		data->_ParticleSystemProperties._MinimumVelocity = Vector3<float>(-0.25f, -0.25f, -0.25f);
		data->_ParticleSystemProperties._MaximumVelocity = Vector3<float>(0.25f, 0.25f, 0.25f);
		data->_Position = AxisAlignedBoundingBox::CalculateCenter(box);

		ParticleSystemEntity *const RESTRICT particles = EntityCreationSystem::Instance->CreateEntity<ParticleSystemEntity>();

		EntityCreationSystem::Instance->RequestInitialization(particles, data, true);

		entities->EmplaceSlow(particles);
	}, 50.0f);
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
	LightingSystem::Instance->GetDirectionalLight()->SetColor(Vector3<float>::LinearlyInterpolate(first._SunColor, second._SunColor, smoothedAlpha));

	//Blend the sun intensity.
	LightingSystem::Instance->GetDirectionalLight()->SetIntensity(CatalystBaseMath::LinearlyInterpolate(first._SunIntensity, second._SunIntensity, smoothedAlpha));

	//Blend the wind speed.
	PhysicsSystem::Instance->SetWindSpeed(CatalystBaseMath::LinearlyInterpolate(first._WindSpeed, second._WindSpeed, smoothedAlpha));

	//Blend the volumetric fog distance.
	RenderingConfigurationManager::Instance->SetVolumetricFogDensity(CatalystBaseMath::LinearlyInterpolate(first._VolumetricFogDensity, second._VolumetricFogDensity, smoothedAlpha));

	//Blend the depth of field distance.
	RenderingConfigurationManager::Instance->SetDepthOfFieldDistanceh(CatalystBaseMath::LinearlyInterpolate(first._DepthOfFieldDistance, second._DepthOfFieldDistance, smoothedAlpha));
}