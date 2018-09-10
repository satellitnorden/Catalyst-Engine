#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/Pointers/UniquePointer.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/DirectionalLightEntity.h>
#include <Entities/ParticleSystemEntity.h>

//Maxim.
#include <Main/MaximObject.h>

class MaximGameSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MaximGameSystem);

	/*
	*	Default constructor.
	*/
	MaximGameSystem() NOEXCEPT { }

	/*
	*	Initializes the Maxim game system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the Maxim game system synchronously.
	*/
	void UpdateSystemSynchronous(const float deltaTime) NOEXCEPT;

	/*
	*	Destroys a Maxim object.
	*/
	void DestroyMaximObject(MaximObject *const RESTRICT object) NOEXCEPT;

private:

	//The speed.
	float _Speed{ 1.0f };

	//The spawn timer.
	float _SpawnTimer{ 0.0f };

	//The spawn time.
	float _SpawnTime{ 5.0f };

	//The camera.
	CameraEntity * RESTRICT _Camera;

	//The sun.
	DirectionalLightEntity *RESTRICT _Sun;

	//Container for the particles.
	StaticArray<ParticleSystemEntity *RESTRICT, 2> _Particles;

	//Container for all enemies.
	DynamicArray<MaximObject> _Enemies;

};