#pragma once

//Engine core.
#include <EngineCore.h>

class EntitySpawner final
{

public:

	/*
	*	Default constructor.
	*/
	EntitySpawner() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~EntitySpawner() NOEXCEPT;

	/*
	*	Updates this entity spawner.
	*/
	void Update(const float deltaTime) NOEXCEPT;

private:

	//Keeps track of when to spawn entities.
	float spawnTimer{ 0.0f };

};