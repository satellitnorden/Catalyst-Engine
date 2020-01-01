#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Entities.
#include <Entities/Types/ModelEntity.h>

//Rendering.
#include <Rendering/Native/HighlightedModel.h>
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>

class ModelSystem final
{

public:

	/*
	*	Post-initializes the model system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Updates the model system during the pre update phase.
	*/
	void PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the top level acceleration structure instances.
	*/
	FORCE_INLINE const DynamicArray<TopLevelAccelerationStructureInstanceData>& GetTopLevelAccelerationStructureInstances() NOEXCEPT
	{
		return _TopLevelAccelerationStructureInstances;
	}

	/*
	*	Returns the top level acceleration structure.
	*/
	FORCE_INLINE AccelerationStructureHandle GetTopLevelAccelerationStructure() NOEXCEPT
	{
		return _TopLevelAccelerationStructure;
	}

	/*
	*	Returns the model data render data table layout.
	*/
	FORCE_INLINE RenderDataTableLayoutHandle GetModelDataRenderDataTableLayout() const NOEXCEPT
	{
		return _ModelDataRenderDataTableLayout;
	}

	/*
	*	Returns the current model data render data table.
	*/
	RenderDataTableHandle GetCurrentModelDataRenderDataTable() const NOEXCEPT;

	/*
	*	Disables highlight on a model entity.
	*/
	void DisableHighlight(const ModelEntity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Enables highlight on a model entity.
	*/
	void EnableHighlight(const ModelEntity* const RESTRICT entity, const Vector3<float>& color, const float strength) NOEXCEPT;

	/*
	*	Returns the highlighted models.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const DynamicArray<HighlightedModel>* const RESTRICT GetHighlightedModels() const NOEXCEPT
	{
		return &_HighlightedModels;
	}

private:

	//The top level acceleration structure instances.
	DynamicArray<TopLevelAccelerationStructureInstanceData> _TopLevelAccelerationStructureInstances;

	//The top level acceleration structure.
	AccelerationStructureHandle _TopLevelAccelerationStructure{ EMPTY_HANDLE };

	//The model data render data table layout.
	RenderDataTableLayoutHandle _ModelDataRenderDataTableLayout;

	//The model data render data tables.
	DynamicArray<RenderDataTableHandle> _ModelDataRenderDataTables;

	//The highlighted models.
	DynamicArray<HighlightedModel> _HighlightedModels;

	/*
	*	Creates the render data table layout.
	*/
	void CreateRenderDataTableLayout() NOEXCEPT;

	/*
	*	Creates the render data tables.
	*/
	void CreateRenderDataTables() NOEXCEPT;

};