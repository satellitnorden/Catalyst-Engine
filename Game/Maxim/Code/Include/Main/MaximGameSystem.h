#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/Updateable.h>
#include <Core/Pointers/UniquePointer.h>

//Entities.
#include <Entities/CameraEntity.h>
#include <Entities/DirectionalLightEntity.h>
#include <Entities/ParticleSystemEntity.h>

//Multithreading.
#include <Multithreading/Spinlock.h>

//Maxim.
#include <Main/MaximCore.h>
#include <Main/MaximObject.h>

class MaximGameSystem final : public Updateable
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
	*	Updates the Maxim game system asynchronously during the opening update phase.
	*/
	bool OpeningUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT final override;

	/*
	*	Updates the Maxim game system asynchronously during the closing update phase.
	*/
	bool ClosingUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT final override;

	/*
	*	Destroys a Maxim object.
	*/
	void DestroyMaximObject(MaximObject *const RESTRICT object) NOEXCEPT;

private:

	//The current color.
	MaximColor _CurrentColor;

	//The next color.
	MaximColor _NextColor;

	//The color timer.
	float _ColorTimer{ 0.0f };

	//The spawn timer.
	float _SpawnTimer{ 0.0f };

	//The spawn time.
	float _SpawnTime{ 1.25f };

	//The camera.
	CameraEntity * RESTRICT _Camera;

	//The sun.
	DirectionalLightEntity *RESTRICT _Sun;

	//Container for the particles.
	StaticArray<ParticleSystemEntity *RESTRICT, 2> _Particles;

	//Container for all object.
	DynamicArray<MaximObject *RESTRICT> _Objects;

	//Lock for the destruction queue.
	Spinlock _DestructionQueueLock;

	//Container for all objects wishing to be destroyed.
	DynamicArray<MaximObject *RESTRICT> _DestructionQueue;

	/*
	*	Updates the color.
	*/
	void UpdateColor(const float deltaTime) NOEXCEPT;

	/*
	*	Returns a random color.
	*/
	MaximColor GetRandomColor() const NOEXCEPT;

	/*
	*	Returns the corresponding color.
	*/
	Vector3 GetColor(const MaximColor color) const NOEXCEPT;

};