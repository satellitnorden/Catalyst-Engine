//Header file.
#include <Main/MaximGameSystem.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/CatalystVectorMath.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Systems.
#include <Systems/EngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/UpdateSystem.h>

//Singleton definition.
DEFINE_SINGLETON(MaximGameSystem);

DynamicPhysicalEntity *RESTRICT spinner;

/*
*	Initializes the Maxim game system.
*/
void MaximGameSystem::InitializeSystem() NOEXCEPT
{
	//Randomize the current color.
	_CurrentColor = GetRandomColor();

	//Randomize the next color.
	_NextColor = GetRandomColor();

	//Create the camera.
	_Camera = EntitySystem::Instance->CreateEntity<CameraEntity>();

	//Move the camera into position.
	_Camera->Move(Vector3(0.0f, 0.0f, 10.0f));

	//Set it as the active camera.
	RenderingSystem::Instance->SetActiveCamera(_Camera);

	//Create the particles.
	StaticArray<Vector3, 2> positions
	{
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f, 25.0f, 0.0f)
	};

	for (uint64 i = 0, size = _Particles.Size(); i < size; ++i)
	{
		_Particles[i] = EntitySystem::Instance->CreateEntity<ParticleSystemEntity>();
		_Particles[i]->Initialize(	RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle),
								ParticleSystemProperties(	10.0f,
															60.0f,
															0.1f,
															Vector2(0.01f, 0.01f),
															Vector2(0.02f, 0.02f),
															Vector3(-5.0f, -12.5f, -5.0f),
															Vector3(5.0f, 12.5f, 5.0f),
															Vector3(-0.1f, -0.1f, -0.1f),
															Vector3(0.1f, 0.1f, 0.1f)),
															positions[i]);
	}

	//Create the sun.
	_Sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	_Sun->SetIntensity(1.0f);
	_Sun->Rotate(Vector3(-22.5f, 180.0f, 0.0f));
	_Sun->SetColor(GetColor(_CurrentColor));

	//Register the Maxim game system for updates.
	UpdateSystem::Instance->RegisterAsynchronousOpeningUpdate(this);
	UpdateSystem::Instance->RegisterAsynchronousClosingUpdate(this);
}

/*
*	Updates the Maxim game system asynchronously during the opening update phase.
*/
bool MaximGameSystem::OpeningUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the color.
	UpdateColor(context->_DeltaTime);

	//Update the speed.
	MaximObject::_Speed += context->_DeltaTime * 0.1f;

	//Update the spawn time.
	_SpawnTime = 1.25f / MaximObject::_Speed;

	//Update the spawn timer.
	_SpawnTimer += context->_DeltaTime;

	//Spawn new objects, if necessary.
	while (_SpawnTimer >= _SpawnTime)
	{
		MaximObject *const RESTRICT newObject{ new MaximObject };

		_Objects.EmplaceSlow(newObject);

		MaximColor newObjectColor{ GetRandomColor() };

		newObject->Initialize(newObjectColor, GetColor(newObjectColor), MaximObject::_Speed);

		UpdateSystem::Instance->RegisterAsynchronousLogicUpdate(newObject);

		_SpawnTimer -= _SpawnTime;
	}

	//Update the positions of the particles.
	for (ParticleSystemEntity *const RESTRICT particles : _Particles)
	{
		particles->Move(Vector3(0.0f, -MaximObject::_Speed * context->_DeltaTime, 0.0f));

		while (particles->GetPosition()._Y <= -25.0f)
		{
			particles->Move(Vector3(0.0f, 50.0f, 0.0f));
		}
	}

	//Return true.
	return true;
}

/*
*	Updates the Maxim game system asynchronously during the closing update phase.
*/
bool MaximGameSystem::ClosingUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Destroy objects.
	for (MaximObject *const RESTRICT object : _DestructionQueue)
	{
		//Terminate the entity.
		EntitySystem::Instance->RequesTermination(object->GetEntity(), false);

		//Destroy the entity.
		EntitySystem::Instance->RequestDestruction(object->GetEntity(), false);

		//Remove this Maxim object from the internal list.
		_Objects.Erase(object);

		//Delete the object.
		delete object;
	}

	_DestructionQueue.ClearFast();

	//Return true.
	return true;
}

/*
*	Destroys a Maxim object.
*/
void MaximGameSystem::DestroyMaximObject(MaximObject *const RESTRICT object) NOEXCEPT
{
	//Lock the destruction queue.
	ScopedLock<Spinlock> scopedLock{ _DestructionQueueLock };

	//Add this object to the destruction queue.
	_DestructionQueue.EmplaceSlow(object);
}

/*
*	Updates the color.
*/
void MaximGameSystem::UpdateColor(const float deltaTime) NOEXCEPT
{
	_ColorTimer += deltaTime;

	if (_ColorTimer >= 5.0f)
	{
		if (_ColorTimer >= 10.0f)
		{
			_ColorTimer -= 10.0f;
			_CurrentColor = _NextColor;
			_NextColor = GetRandomColor();
		}

		else
		{
			_Sun->SetColor(Vector3::LinearlyInterpolate(GetColor(_CurrentColor), GetColor(_NextColor), (_ColorTimer - 5.0f) / 5.0f));
		}
	}
}

/*
*	Returns a random color.
*/
MaximColor MaximGameSystem::GetRandomColor() const NOEXCEPT
{
	return static_cast<MaximColor>(CatalystBaseMath::RandomIntegerInRange<uint16>(0, static_cast<uint16>(MaximColor::NumberOfMaximColors)));
}

/*
*	Returns the corresponding color.
*/
Vector3 MaximGameSystem::GetColor(const MaximColor color) const NOEXCEPT
{
	switch (color)
	{
		case MaximColor::Lime: return Vector3(0.25f, 1.0f, 0.0f);
		case MaximColor::Purple: return Vector3(0.25f, 0.0f, 1.0f);
		case MaximColor::Red: return Vector3(1.0f, 0.0f, 0.0f);
		case MaximColor::Teal: return Vector3(0.0f, 1.0f, 1.0f);

		default: return GetColor(MaximColor::Teal);
	}
}