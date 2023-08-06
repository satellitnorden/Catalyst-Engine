#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/General/Vector.h>

class EditorEntitySystem final
{

public:

	/*
	*	Updates the editor entity system.
	*/
	void Update() NOEXCEPT;

	/*
	*	Returns whether or not the user is currently creating an entity.
	*/
	FORCE_INLINE NO_DISCARD bool IsCurrentlyCreatingAnEntity() const NOEXCEPT
	{
		return _IsCurrentlyCreatingEntity;
	}
	
private:

	//Denotes whether or not the user is currently creating an entity.
	bool _IsCurrentlyCreatingEntity{ false };
	
	//The entity type currently being created.
	EntityType _EntityTypeBeingCreated;

	/*
	*	Creates a new entity at the given position.
	*/
	void CreateEntity(const Vector3<float32> &position);

};
#endif