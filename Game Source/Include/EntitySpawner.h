#pragma once

//Engine core.
#include <EngineCore.h>

class EntitySpawner final
{

public:

	/*
	*	Default constructor.
	*/
	EntitySpawner() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~EntitySpawner() CATALYST_NOEXCEPT;

	/*
	*	Updates this entity spawner.
	*/
	void Update(const float deltaTime) CATALYST_NOEXCEPT;

private:

	//Keeps track of when to spawn entities.
	float spawnTimer{ 0.0f };

};