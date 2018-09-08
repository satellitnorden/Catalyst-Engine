#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/Collection.h>
#include <Core/General/UpdateContext.h>

//Forward declarations.
class DynamicPhysicalEntity;

class MaximEnemy final
{

public:

	/*
	*	Updates this maxim enemy.
	*/
	void Update(const UpdateContext *const RESTRICT context) NOEXCEPT;

private:

	//The dynamic physical entity.
	DynamicPhysicalEntity *RESTRICT _Entity;

};