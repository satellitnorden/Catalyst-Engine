#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/DirectionalLightEntity.h>

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
	*	Releases the Maxim game system.
	*/
	void ReleaseSystem() NOEXCEPT;

private:

	//The speed.
	float _Speed{ 1.0f };

	//The spawn timer.
	float _SpawnTimer{ 0.0f };

	//The spawn time.
	float _SpawnTime{ 1.25f };

	//The camera.
	CameraEntity * RESTRICT camera;

	//The sun.
	DirectionalLightEntity *RESTRICT sun;

	//Container for all enemies.
	DynamicArray<MaximObject *RESTRICT> _Enemies;

};