#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/DynamicString.h>

//Math.
#include <Math/General/Vector.h>

//World.
#include <World/Level/Level.h>

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

private:

	//Enumeration covering all gizmo modes.
	enum class GizmoMode : uint8
	{
		TRANSLATE,
		ROTATE,
		SCALE
	};

	//The level file path.
	DynamicString _LevelFilePath;

	//The level.
	Level _Level;

	//The name counter.
	uint32 _NameCounter{ 0 };

	//The selected level entry index.
	uint64 _SelectedLevelEntryIndex{ UINT64_MAXIMUM };

	//The current Gizmo mode.
	GizmoMode _CurrentGizmoMode{ GizmoMode::TRANSLATE };

	/*
	*	Saves a level internally.
	*/
	void SaveLevelInternal(const DynamicString &file_path) NOEXCEPT;

	/*
	*	Loads a level internally.
	*/
	void LoadLevelInternal(const DynamicString &file_path) NOEXCEPT;

	/*
	*	Generates an entity name.
	*/
	void GenerateEntityName(char *const RESTRICT buffer, const uint64 buffer_size) NOEXCEPT;

	/*
	*	The top right window update.
	*/
	NO_DISCARD bool TopRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

	/*
	*	The bottom right window update.
	*/
	NO_DISCARD bool BottomRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

	/*
	*	Duplicates an entry.
	*/
	void DuplicateEntity(Entity *const RESTRICT entity) NOEXCEPT;

};

#endif
