#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Content.
#include <Content/Core/AssetPointer.h>
#include <Content/Assets/LevelAsset.h>

//World.
#include <World/Core/WorldTransform.h>
#include <World/Level/Level.h>

//Forward declarations.
class ComponentInitializationData;

class ALIGN(8) LevelSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LevelSystem);

	//Type aliases.
	using SpawnFunction = void(*)
	(
		ComponentInitializationData *const RESTRICT initialization_data,
		const uint64 entity_identifier,
		void *const RESTRICT user_data
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LevelSystem() NOEXCEPT
	{
	
	}

	/*
	*	Spawns a level.
	*/
	void SpawnLevel
	(
		const WorldTransform &world_transform,
		const AssetPointer<LevelAsset> level_asset,
		Level *const RESTRICT level,
		SpawnFunction spawn_function = nullptr,
		void *const RESTRICT spawn_function_user_data = nullptr
	) NOEXCEPT;

	/*
	*	Despawns the given level.
	*/
	void DespawnLevel(Level *const RESTRICT level) NOEXCEPT;

};