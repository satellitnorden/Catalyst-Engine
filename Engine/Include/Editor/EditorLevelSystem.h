#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>
#include <Core/General/StaticString.h>

//Entities.
#include <Entities/Core/Entity.h>

//Math.
#include <Math/General/Vector.h>

//Third party.
#include <ThirdParty/json/json.hpp>

//Forward declarations.
class Asset;

class EditorLevelSystem final
{

public:

	/*
	*	Post initializes the editor level system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Creates an entity.
	*/
	void CreateEntity() NOEXCEPT;

	/*
	*	Saves the current level.
	*/
	void SaveLevel() NOEXCEPT;

	/*
	*	Saves the current level as...
	*/
	void SaveLevelAs() NOEXCEPT;

	/*
	*	Loads a level
	*/
	void LoadLevel() NOEXCEPT;

	/*
	*	Starts the game.
	*/
	void StartGame() NOEXCEPT;

	/*
	*	Ends the game.
	*/
	void EndGame() NOEXCEPT;

private:

	//Enumeration covering all gizmo modes.
	enum class GizmoMode : uint8
	{
		TRANSLATE,
		ROTATE,
		SCALE
	};

	/*
	*	Entity editor data class definition.
	*/
	class EntityEditorData final
	{

	public:

		//The name.
		StaticString<64> _Name;

		//The identifier.
		uint64 _Identifier;

		//The rotation. Used in place of the quaternion rotation in WorldTransformComponent.
		EulerAngles _Rotation{ 0.0f, 0.0f, 0.0f };

		//The links.
		DynamicArray<uint64> _Links;

	};

	//The level file path.
	DynamicString _LevelFilePath;

	//The entities.
	DynamicArray<Entity *RESTRICT> _Entities;

	//The entity editor data.
	DynamicArray<EntityEditorData> _EntityEditorData;

	//The name counter.
	uint32 _NameCounter{ 0 };

	//The current entity identifier.
	uint64 _CurrentEntityIdentifier{ 0 };

	//The selected entity index.
	uint64 _SelectedEntityIndex{ UINT64_MAXIMUM };

	//The current Gizmo mode.
	GizmoMode _CurrentGizmoMode{ GizmoMode::TRANSLATE };

	//The currently played level.
	nlohmann::json _CurrentlyPlayedLevel;

	/*
	*	Saves a level internally.
	*/
	void SaveLevelInternal(nlohmann::json &JSON) NOEXCEPT;

	/*
	*	Loads a level internally.
	*/
	void LoadLevelInternal(const nlohmann::json &JSON) NOEXCEPT;

	/*
	*	Generates an entity name.
	*/
	void GenerateEntityName(char *const RESTRICT buffer, const uint64 buffer_size) NOEXCEPT;

	/*
	*	Generates an entity identifier.
	*/
	void GenerateEntityIdentifier(uint64 *const RESTRICT identifier) NOEXCEPT;

	/*
	*	The top right window update.
	*/
	NO_DISCARD bool TopRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

	/*
	*	The bottom right window update.
	*/
	NO_DISCARD bool BottomRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

	/*
	*	Loads an entity.
	*/
	void LoadEntity(const char *const RESTRICT file_path, Entity *RESTRICT *const RESTRICT entity, EntityEditorData *const RESTRICT editor_data) NOEXCEPT;

	/*
	*	Saves an entity.
	*/
	void SaveEntity(const char *const RESTRICT file_path, Entity *const RESTRICT entity, const EntityEditorData &entity_editor_data) NOEXCEPT;

	/*
	*	Duplicates an entry.
	*/
	void DuplicateEntity(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Returns the index for the given entity identifier.
	*/
	NO_DISCARD uint64 EntityIdentifierToIndex(const uint64 identifier) NOEXCEPT;

	/*
	*	Returns if the first entity is linked to the second entity.
	*/
	NO_DISCARD bool AreEntitiesLinked(const uint64 entity_index_1, const uint64 entity_index_2) NOEXCEPT;

};

#endif
