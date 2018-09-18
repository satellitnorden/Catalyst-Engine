//Header file.
#include <Main/MaximGameSystem.h>

//Entities.
#include <Entities/InitializationData/ParticleSystemInitializationData.h>

//Managers.
#include <Managers/RenderingConfigurationManager.h>

//Math.
#include <Math/CatalystBaseMath.h>
#include <Math/CatalystVectorMath.h>

//Multithreading.
#include <Multithreading/ScopedLock.h>

//Systems.
#include <Systems/DebugRenderingSystem.h>
#include <Systems/EngineSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/PhysicsSystem.h>
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
		ParticleSystemInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<ParticleSystemInitializationData>() };

		data->_Properties = 0;
		data->_Material = RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle);
		data->_ParticleSystemProperties._Properties = static_cast<uint32>(ParticleSystemProperties::ParticleSystemProperty::AffectedByWind) | static_cast<uint32>(ParticleSystemProperties::ParticleSystemProperty::Looping);
		data->_ParticleSystemProperties._FadeTime = 10.0f;
		data->_ParticleSystemProperties._Lifetime = 60.0f;
		data->_ParticleSystemProperties._SpawnFrequency = 0.01f;
		data->_ParticleSystemProperties._MinimumScale = Vector2(0.01f, 0.01f);
		data->_ParticleSystemProperties._MaximumScale = Vector2(0.02f, 0.02f);
		data->_ParticleSystemProperties._MinimumPosition = Vector3(-5.0f, -12.5f, -5.0f);
		data->_ParticleSystemProperties._MaximumPosition = Vector3(5.0f, 12.5f, 5.0f);
		data->_ParticleSystemProperties._MinimumVelocity = Vector3(-0.1f, -0.1f, -0.1f);
		data->_ParticleSystemProperties._MaximumVelocity = Vector3(0.1f, 0.1f, 0.1f);
		data->_Position = positions[i];

		_Particles[i] = EntitySystem::Instance->CreateEntity<ParticleSystemEntity>();

		EntitySystem::Instance->InitializeEntity(_Particles[i], data);
	}

	//Create the sun.
	_Sun = EntitySystem::Instance->CreateEntity<DirectionalLightEntity>();
	_Sun->SetIntensity(1.0f);
	_Sun->Rotate(Vector3(-22.5f, 180.0f, 0.0f));
	_Sun->SetColor(GetColor(_CurrentColor));

	//Register the Maxim game system for updates.
	UpdateSystem::Instance->RegisterAsynchronousOpeningUpdate(this);
	UpdateSystem::Instance->RegisterAsynchronousPhysicsUpdate(this);
	UpdateSystem::Instance->RegisterSynchronousClosingUpdate(this);
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
	_SpawnTime = 2.5f / MaximObject::_Speed;

	//Update the spawn timer.
	_SpawnTimer += context->_DeltaTime;

	//Spawn new objects, if necessary.
	while (_SpawnTimer >= _SpawnTime)
	{
		MaximObject *const RESTRICT newObject{ new (MemoryUtilities::ThreadSafePoolAllocate<sizeof(MaximObject)>()) MaximObject };

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
*	Updates the Maxim game system asynchronously during the physics update phase.
*/
bool MaximGameSystem::PhysicsUpdateAsynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update whch Maxim object is selected.
	UpdateSelected();

	//Update the position of the selected Maxim object.
	UpdateSelectedPosition();

	//Return true.
	return true;
}

/*
*	Updates the Maxim game system synchronously during the closing update phase.
*/
bool MaximGameSystem::ClosingUpdateSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Destroy objects.
	for (MaximObject *const RESTRICT object : _DestructionQueue)
	{
		//Terminate the entity.
		EntitySystem::Instance->TerminateEntity(object->_Entity);

		//Destroy the entity.
		EntitySystem::Instance->DestroyEntity(object->_Entity);

		//Remove this Maxim object from the internal list.
		_Objects.Erase(object);

		//De-allocate the object.
		MemoryUtilities::ThreadSafePoolDeAllocate<sizeof(MaximObject)>(object);
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

/*
*	Updates which Maxim object is selected
*/
void MaximGameSystem::UpdateSelected() NOEXCEPT
{
	//Do a ray cast from the camera and towards the mouse/touch and see if any Maxim objects were clicked/touched.
#if defined(CATALYST_WINDOWS)
	const MouseState *const RESTRICT state{ InputSystem::Instance->GetMouseState() };
	const ButtonState buttonState{ state->_Left };
#elif defined(CATALYST_ANDROID)
	const TouchState *const RESTRICT state{ InputSystem::Instance->GetTouchState() };
	const ButtonState buttonState{ state->_ButtonState };
#endif

	if (buttonState == ButtonState::Pressed)
	{
		Ray ray;
		ray._Origin = RenderingSystem::Instance->GetActiveCamera()->GetPosition();
		ray._Direction = RenderingSystem::Instance->GetWorldDirectionFromScreenCoordinate(Vector2(state->_CurrentX, state->_CurrentY));
		ray._Distance = FLOAT_MAXIMUM;

		RayCastResult result;

		PhysicsSystem::Instance->CastRay(ray, &result);

		if (result._HasHit)
		{
			//Find the maxim object with the hit entity.
			for (MaximObject *const RESTRICT object : _Objects)
			{
				if (object->_Entity == result._HitEntity)
				{
					_SelectedObject = object;
					object->_IsSelected = true;
					object->UpdateSelectedPosition();

					return;
				}
			}
		}
	}
}

/*
*	Updates the position of the selected Maxim object.
*/
void MaximGameSystem::UpdateSelectedPosition() NOEXCEPT
{
	if (_SelectedObject && _SelectedObject->_Entity->_Initialized)
	{
#if defined(CATALYST_WINDOWS)
		const MouseState *const RESTRICT state{ InputSystem::Instance->GetMouseState() };
		const ButtonState buttonState{ state->_Left };
#elif defined(CATALYST_ANDROID)
		const TouchState *const RESTRICT state{ InputSystem::Instance->GetTouchState() };
		const ButtonState buttonState{ state->_ButtonState };
#endif
		if (buttonState == ButtonState::Pressed || buttonState == ButtonState::PressedHold)
		{
			const Vector3 newPosition{ CatalystVectorMath::LinePlaneIntersection(	Vector3(0.0f, 0.0f, 0.0f),
																					RenderingSystem::Instance->GetActiveCamera()->GetPosition(),
																					Vector3::BACKWARD,
																					RenderingSystem::Instance->GetWorldDirectionFromScreenCoordinate(Vector2(state->_CurrentX, state->_CurrentY))) };

			_SelectedObject->_Entity->SetPosition(newPosition);

			//Check collision against otheer Maxim objects.
			for (MaximObject *const RESTRICT object : _Objects)
			{
				if (object == _SelectedObject)
				{
					continue;
				}

				if (!object->_Entity->_Initialized)
				{
					continue;
				}

				if (object->_IsDestroyed)
				{
					continue;
				}

				if (CatalystVectorMath::BoxIntersection(	*_SelectedObject->_Entity->GetWorldSpaceAxisAlignedBoundingBox(),
															*object->_Entity->GetWorldSpaceAxisAlignedBoundingBox()))
				{
					object->_IsDestroyed = true;

					ParticleSystemInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<ParticleSystemInitializationData>() };

					data->_Properties = static_cast<uint8>(EntityInitializationData::EntityProperty::AutomaticDestruction) | static_cast<uint8>(EntityInitializationData::EntityProperty::AutomaticTermination);
					data->_Material = RenderingSystem::Instance->GetCommonParticleMaterial(RenderingSystem::CommonParticleMaterial::WhiteCircle);
					data->_ParticleSystemProperties._Properties = static_cast<uint32>(ParticleSystemProperties::ParticleSystemProperty::AffectedByWind);
					data->_ParticleSystemProperties._FadeTime = 0.5f;
					data->_ParticleSystemProperties._Lifetime = 1.0f;
					data->_ParticleSystemProperties._SpawnFrequency = 0.001f;
					data->_ParticleSystemProperties._MinimumScale = Vector2(0.015f, 0.015f);
					data->_ParticleSystemProperties._MaximumScale = Vector2(0.03f, 0.03f);
					data->_ParticleSystemProperties._MinimumPosition = Vector3(-0.1f, -0.1f, -0.1f);
					data->_ParticleSystemProperties._MaximumPosition = Vector3(0.1f, 0.1f, 0.1f);
					data->_ParticleSystemProperties._MinimumVelocity = Vector3::Normalize(Vector3(1.0f, 1.0f, 0.0f)) * -10.0f;
					data->_ParticleSystemProperties._MaximumVelocity = Vector3::Normalize(Vector3(1.0f, 1.0f, 1.0f)) * 10.0f;
					data->_Position = object->_Entity->GetPosition();

					ParticleSystemEntity *const RESTRICT particles{ EntitySystem::Instance->CreateEntity<ParticleSystemEntity>() };

					EntitySystem::Instance->InitializeEntity(particles, data);
				}
			}
		}

		else
		{
			_SelectedObject->_IsSelected = false;
			_SelectedObject = nullptr;
		}
	}
}