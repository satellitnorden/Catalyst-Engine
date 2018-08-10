//Core.
#include <Core/EngineCore.h>

//Clairvoyant.
#include <Main/ClairvoyantGameSystem.h>

//Systems.
#include <Systems/EngineSystem.h>

//Preprocessor defines.
#define EXIT_SUCCESS 0

#if !defined(CATALYST_FINAL) && defined(CATALYST_WINDOWS)
//Forward declarations.
void CreateResources() NOEXCEPT;
#endif

MAIN
{
#if !defined(CATALYST_FINAL) && defined(CATALYST_WINDOWS)
	//Create resources, if necessary.
	if (false)
	{
		CreateResources();
	}
#endif

	//The delta timer.
	DeltaTimer deltaTimer;

	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem(	CatalystProjectInformation(CatalystProjectGeneralInformation("Clairvoyant"),
												CatalystProjectMultithreadingInformation(8),
												CatalystProjectRenderingInformation(),
												CatalystProjectSoundInformation(256)));

	//Initialize the game system.
	ClairvoyantGameSystem::Instance->InitializeSystem();

	//Main game loop.
	bool shouldTerminate{ false };

	while (!shouldTerminate)
	{
		//Calculate the delta time.
		const float deltaTime{ deltaTimer.Update() };

		//Update the game system.
		ClairvoyantGameSystem::Instance->UpdateSystemSynchronous(deltaTime);

		//Update the engine system.
		shouldTerminate = EngineSystem::Instance->UpdateSystemSynchronous(deltaTime);
	}

	//Release the game system.
	ClairvoyantGameSystem::Instance->ReleaseSystem();

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();

	MAIN_RETURN_SUCCESS;
}

#if !defined(CATALYST_FINAL) && defined(CATALYST_WINDOWS)
//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Systems.
#include <Systems/TaskSystem.h>

//Resource creator.
#include <EnvironmentMaterialCreator.h>
#include <ParticleMaterialCreator.h>
#include <PhysicalMaterialCreator.h>
#include <PhysicalModelCreator.h>
#include <ResourceCollectionCreator.h>
#include <SoundBankCreator.h>
#include <TerrainMaterialCreator.h>
#include <VegetationMaterialCreator.h>
#include <WaterMaterialCreator.h>

//Enumeration covering all resource tasks.
enum class Resource : uint16
{
	//Environment materials.
	DayEnvironmentMaterial,
	NightEnvironmentMaterial,

	//Particle materials.
	Fog1Material,
	ParticleMaterial,

	//Physical materials.
	Grass1Material,
	Stone1Material,
	Stone2Material,
	Tree1Material,

	//Physical models.
	Stone1Model,
	Stone2Model,
	Tree1Model,

	//Sound banks.
	MasterSoundBank,

	//Terrain materials.
	GrassTerrainMaterial,

	//Vegetation materials.
	DefaulVegetationMaterial,

	//Water materials.
	DefaultWaterMaterial,

	NumberOfResources
};

void CreateResources() NOEXCEPT
{
	//The tasks.
	StaticArray<Task, INDEX(Resource::NumberOfResources)> tasks
	{
		/*
		*	Environment materials.
		*/

		//Day environment material.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/DayEnvironmentMaterial",
				"DayEnvironmentMaterial",
				"../../../Game Resources/Raw/Textures/Environments/Day.hdr",
				"1024",
				"16"
			};

			EnvironmentMaterialCreator::CreateEnvironmentMaterial(arguments);
		}, nullptr),

		//Night environment material.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/NightEnvironmentMaterial",
				"NightEnvironmentMaterial",
				"../../../Game Resources/Raw/Textures/Environments/Night.hdr",
				"1024",
				"16"
			};

			EnvironmentMaterialCreator::CreateEnvironmentMaterial(arguments);
		}, nullptr),

		/*
		*	Particle materials.
		*/

		//Fog 1 particle material.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/Fog1Material",
				"Fog1Material",
				"5",
				"../../../Game Resources/Raw/Textures/Particles/Fog1.png"
			};

			ParticleMaterialCreator::CreateParticleMaterial(arguments);
		}, nullptr),

		//Particle material.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/ParticleMaterial",
				"ParticleMaterial",
				"4",
				"../../../Game Resources/Raw/Textures/Particles/Particle.png"
			};

			ParticleMaterialCreator::CreateParticleMaterial(arguments);
		}, nullptr),

		/*
		*	Physical materials.
		*/

		//Grass 1 physical material.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/Grass1Material",
				"Grass1Material",
				"9",
				"../../../Game Resources/Raw/Textures/General/Grass1Albedo.png",
				"../../../Game Resources/Raw/Textures/General/Grass1Normal.png",
				"../../../Game Resources/Raw/Textures/General/Grass1Roughness.png",
				"Default",
				"../../../Game Resources/Raw/Textures/General/Grass1AmbientOcclusion.png",
				"Default"
			};

			PhysicalMaterialCreator::CreatePhysicalMaterial(arguments);
		}, nullptr),

		//Stone 1 physical material.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/Stone1Material",
				"Stone1Material",
				"5",
				"../../../Game Resources/Raw/Textures/StoneAlbedo.png",
				"../../../Game Resources/Raw/Textures/StoneNormalMap.png",
				"../../../Game Resources/Raw/Textures/StoneRoughness.png",
				"Default",
				"../../../Game Resources/Raw/Textures/General/Grass1AmbientOcclusion.png",
				"Default"
			};

			PhysicalMaterialCreator::CreatePhysicalMaterial(arguments);
		}, nullptr),

		//Stone 2 physical material.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/Stone2Material",
				"Stone2Material",
				"9",
				"../../../Game Resources/Raw/Textures/ModelTextures/Stone2/Albedo.png",
				"../../../Game Resources/Raw/Textures/ModelTextures/Stone2/Normal.png",
				"../../../Game Resources/Raw/Textures/ModelTextures/Stone2/Roughness.png",
				"Default",
				"Default",
				"Default"
			};

			PhysicalMaterialCreator::CreatePhysicalMaterial(arguments);
		}, nullptr),

		//Tree 1 physical material.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/Tree1Material",
				"Tree1Material",
				"9",
				"../../../Game Resources/Raw/Textures/ModelTextures/Tree1/Albedo.png",
				"../../../Game Resources/Raw/Textures/ModelTextures/Tree1/Normal.png",
				"../../../Game Resources/Raw/Textures/ModelTextures/Tree1/Roughness.png",
				"Default",
				"../../../Game Resources/Raw/Textures/ModelTextures/Tree1/AmbientOcclusion.png",
				"Default"
			};

			PhysicalMaterialCreator::CreatePhysicalMaterial(arguments);
		}, nullptr),

		/*
		*	Physical models.
		*/

		//Stone 1 model.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Models/Stone1Model",
				"Stone1Model",
				"../../../Game Resources/Raw/Models/Stone1.obj"
			};

			PhysicalModelCreator::CreatePhysicalModel(arguments);
		}, nullptr),

		//Stone 2 model.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Models/Stone2Model",
				"Stone2Model",
				"../../../Game Resources/Raw/Models/Stone2.fbx"
			};

			PhysicalModelCreator::CreatePhysicalModel(arguments);
		}, nullptr),

		//Tree 1 model.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Models/Tree1Model",
				"Tree1Model",
				"../../../Game Resources/Raw/Models/Tree1.fbx"
			};

			PhysicalModelCreator::CreatePhysicalModel(arguments);
		}, nullptr),

		/*
		*	Sounds banks.
		*/

		//Master sound bank.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/SoundBanks/MasterSoundBank",
				"MasterSoundBank",
				"../../../Game Resources/Raw/SoundBanks/MasterBank.bank"
			};

			SoundBankCreator::CreateSoundBank(arguments);
		}, nullptr),

		/*
		*	Terrain materials.
		*/

		//Grass terrain material.
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/GrassTerrainMaterial",
				"GrassTerrainMaterial",
				"7",

				//Sand layer.
				"../../../Game Resources/Raw/Textures/Sand1Albedo.png",
				"../../../Game Resources/Raw/Textures/Sand1NormalMap.png",
				"../../../Game Resources/Raw/Textures/Sand1Roughness.png",
				"Default",
				"../../../Game Resources/Raw/Textures/Sand1AmbientOcclusion.png",
				"../../../Game Resources/Raw/Textures/Sand1Displacement.png",

				//Grass layer 1.
				"../../../Game Resources/Raw/Textures/Grass1Albedo.png",
				"../../../Game Resources/Raw/Textures/Grass1NormalMap.png",
				"../../../Game Resources/Raw/Textures/Grass1Roughness.png",
				"Default",
				"../../../Game Resources/Raw/Textures/Grass1AmbientOcclusion.png",
				"../../../Game Resources/Raw/Textures/Grass1Displacement.png",

				//Grass layer 2.
				"../../../Game Resources/Raw/Textures/Terrain/Grass2Albedo.png",
				"../../../Game Resources/Raw/Textures/Terrain/Grass2Normal.png",
				"../../../Game Resources/Raw/Textures/Terrain/Grass2Roughness.png",
				"Default",
				"../../../Game Resources/Raw/Textures/Terrain/Grass2AmbientOcclusion.png",
				"../../../Game Resources/Raw/Textures/Terrain/Grass2Displacement.png",

				//Stone layer.
				"../../../Game Resources/Raw/Textures/Stone1Albedo.png",
				"../../../Game Resources/Raw/Textures/Stone1NormalMap.png",
				"../../../Game Resources/Raw/Textures/Stone1Roughness.png",
				"Default",
				"../../../Game Resources/Raw/Textures/Stone1AmbientOcclusion.png",
				"../../../Game Resources/Raw/Textures/Stone1Displacement.png",

				//Snow layer
				"../../../Game Resources/Raw/Textures/Snow3Albedo.png",
				"../../../Game Resources/Raw/Textures/Snow3NormalMap.png",
				"../../../Game Resources/Raw/Textures/Snow3Roughness.png",
				"Default",
				"../../../Game Resources/Raw/Textures/Snow3AmbientOcclusion.png",
				"../../../Game Resources/Raw/Textures/Snow3Displacement.png",
			};

			TerrainMaterialCreator::CreateTerrainMaterial(arguments);
		}, nullptr),

		/*
		*	Vegetation materials.
		*/

		//Default vegetation material
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/DefaultVegetationMaterial",
				"DefaultVegetationMaterial",
				"3",
				"10",
				"../../../Game Resources/Raw/Textures/Vegetation/Mask.png",
				"../../../Game Resources/Raw/Textures/Vegetation/Albedo.png",
				"../../../Game Resources/Raw/Textures/Vegetation/NormalMap.png",
				"../../../Game Resources/Raw/Textures/Vegetation/Roughness.png",
				"../../../Game Resources/Raw/Textures/Vegetation/AmbientOcclusion.png",
				"../../../Game Resources/Raw/Textures/Vegetation/Thinness.png"
			};

			VegetationMaterialCreator::CreateVegetationMaterial(arguments);
		}, nullptr),

		/*
		*	Water materials.
		*/

		//Default water material
		Task([](void *const RESTRICT)
		{
			const char *const RESTRICT arguments[]
			{
				"../../../Game Resources/Intermediate/Materials/DefaultWaterMaterial",
				"DefaultWaterMaterial",
				"5",
				"../../../Game Resources/Raw/Textures/WaterNormal.png"
			};

			WaterMaterialCreator::CreateWaterMaterial(arguments);
		}, nullptr),
	};

	//Initialize the task system.
	TaskSystem::Instance->InitializeSystem(CatalystProjectMultithreadingInformation(INDEX(Resource::NumberOfResources)));

	//Execute all resource tasks.
	for (Task &task : tasks)
	{
		TaskSystem::Instance->ExecuteTask(&task);
	}

	//Wait for all tasks to finish.
	uint16 counter{ 0 };

	for (Task &task : tasks)
	{
		task.WaitFor();

		++counter;

		PRINT_TO_CONSOLE(counter << " out of " << INDEX(Resource::NumberOfResources) << " resource tasks are done.");
	}

	//Create the resource collection.
	const char *const RESTRICT arguments[]
	{
		"../../../Game Resources/Final/ClairvoyantResourceCollection",

		"../../../Game Resources/Intermediate/Materials/DayEnvironmentMaterial.cr",
		"../../../Game Resources/Intermediate/Materials/NightEnvironmentMaterial.cr",
		"../../../Game Resources/Intermediate/Materials/Fog1Material.cr",
		"../../../Game Resources/Intermediate/Materials/ParticleMaterial.cr",
		"../../../Game Resources/Intermediate/Materials/Grass1Material.cr",
		"../../../Game Resources/Intermediate/Materials/Stone1Material.cr",
		"../../../Game Resources/Intermediate/Materials/Stone2Material.cr",
		"../../../Game Resources/Intermediate/Materials/Tree1Material.cr",
		"../../../Game Resources/Intermediate/Models/Stone1Model.cr",
		"../../../Game Resources/Intermediate/Models/Stone2Model.cr",
		"../../../Game Resources/Intermediate/Models/Stone2Model.cr",
		"../../../Game Resources/Intermediate/Models/Tree1Model.cr",
		"../../../Game Resources/Intermediate/SoundBanks/MasterSoundBank.cr",
		"../../../Game Resources/Intermediate/Materials/GrassTerrainMaterial.cr",
		"../../../Game Resources/Intermediate/Materials/DefaultVegetationMaterial.cr",
		"../../../Game Resources/Intermediate/Materials/DefaultWaterMaterial.cr",
	};

	ResourceCollectionCreator::CreateResourceCollection(sizeof(arguments) / sizeof(arguments[0]), arguments);

	//Release the task system.
	TaskSystem::Instance->ReleaseSystem();
}
#endif