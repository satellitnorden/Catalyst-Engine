#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Creation/DynamicPhysicalInitializationData.h>
#include <Entities/Creation/ParticleSystemInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

namespace EntityCreationUtilities
{

	/*
	*	Creates and returns dynamic physical initialization data.
	*/
	FORCE_INLINE RESTRICTED static NO_DISCARD DynamicPhysicalInitializationData *const RESTRICT CreateDynamicPhysicalInitializationData(const EntityInitializationData::EntityProperty properties,
																																		const PhysicalFlag flags,
																																		const PhysicalModel &model,
																																		const PhysicalMaterial &material,
																																		const Vector3<float> &position,
																																		const Vector3<float> &rotation,
																																		const Vector3<float> &scale,
																																		const Vector3<float> outlineColor) NOEXCEPT
	{
		DynamicPhysicalInitializationData *const RESTRICT data{ EntityCreationSystem::Instance->CreateInitializationData<DynamicPhysicalInitializationData>() };

		data->_Properties = properties;
		data->_Flags = flags;
		data->_Model = model;
		data->_Material = material;
		data->_Position = position;
		data->_Rotation = rotation;
		data->_Scale = scale;
		data->_OutlineColor = outlineColor;

		return data;
	}

}