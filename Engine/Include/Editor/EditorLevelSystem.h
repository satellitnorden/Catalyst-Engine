#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/General/Vector.h>

//World.
#include <World/Level/LevelEntry.h>

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

private:

	//The name counter.
	uint32 _NameCounter{ 0 };

	//The level entries.
	DynamicArray<LevelEntry> _LevelEntries;

	/*
	*	The top right window update.
	*/
	NO_DISCARD bool TopRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

	/*
	*	The bottom right window update.
	*/
	NO_DISCARD bool BottomRightWindowUpdate(const Vector2<float32> minimum, const Vector2<float32> maximum) NOEXCEPT;

};

#endif
