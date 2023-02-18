#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//World.
#include <World/Core/WorldPosition.h>

class ParticleSystemEntity : public Entity
{

public:

	//Enumeration covering all properties.
	enum class Property : uint8
	{
		/*
		*	No properties.
		*/
		NONE = BIT(0),

		/*
		*	Particle system entities with the looping property is looping, and will not terminate until explicitly terminated.
		*/
		LOOPING = BIT(1)
	};

	/*
	*	Default constructor.
	*/
	ParticleSystemEntity() NOEXCEPT;

	/*
	*	Returns the preprocessing parameters.
	*/
	FORCE_INLINE void GetPreprocessingParameters(EntityPreprocessingParameters* const RESTRICT parameters) NOEXCEPT
	{

	}

	/*
	*	Preprocesses this entity.
	*/
	FORCE_INLINE void Preprocess(EntityInitializationData* const RESTRICT data) NOEXCEPT
	{

	}

	/*
	*	Initializes this entity.
	*/
	void Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT;

	/*
	*	Terminates this entity.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns whether or not this entity should automatically terminate.
	*/
	bool ShouldAutomaticallyTerminate() const NOEXCEPT;

	/*
	*	Returns the initialization data required to duplicate this entity.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT GetDuplicationInitializationData() const NOEXCEPT
	{
		ASSERT(false, "This entity type does not have this function implemented!");

		return nullptr;
	}

	/*
	*	Returns whether or not this particle system entity is looping.
	*/
	NO_DISCARD bool IsLooping() const NOEXCEPT;

	/*
	*	Returns the world position.
	*/
	NO_DISCARD const WorldPosition &GetWorldPosition() const NOEXCEPT;

	/*
	*	Sets the world position.
	*/
	void SetWorldPosition(const WorldPosition &value) NOEXCEPT;

	/*
	*	Sets the minimum velocity.
	*/
	void SetMinimumVelocity(const Vector3<float32> &velocity) NOEXCEPT;

	/*
	*	Sets the maximum velocity.
	*/
	void SetMaximumVelocity(const Vector3<float32> &velocity) NOEXCEPT;

	/*
	*	Sets the spawn frequency.
	*/
	void SetSpawnFrequency(const float spawn_frequency) NOEXCEPT;

};

ENUMERATION_BIT_OPERATIONS(ParticleSystemEntity::Property);