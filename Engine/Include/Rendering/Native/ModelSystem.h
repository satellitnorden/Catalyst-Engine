#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/UpdateContext.h>

//Entities.
#include <Entities/Types/DynamicModelEntity.h>

//Rendering.
#include <Rendering/Native/HighlightedModel.h>
#include <Rendering/Native/RenderingCore.h>
#include <Rendering/Native/TopLevelAccelerationStructureInstanceData.h>

class ModelSystem final
{

public:

	/*
	*	Initializes the model system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Post-initializes the model system.
	*/
	void PostInitialize() NOEXCEPT;

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
	*	Enables highlight on a model entity.
	*/
	void EnableHighlight(const DynamicModelEntity *const RESTRICT entity, const Vector3<float>& color, const float strength) NOEXCEPT;

	/*
	*	Sets the highlight color on a model entity.
	*/
	void SetHighlightColor(const DynamicModelEntity *const RESTRICT entity, const Vector3<float> &color) NOEXCEPT;

	/*
	*	Sets the highlight strength on a model entity.
	*/
	void SetHighlightStrength(const DynamicModelEntity *const RESTRICT entity, const float32 strength) NOEXCEPT;

	/*
	*	Disables highlight on a model entity.
	*/
	void DisableHighlight(const DynamicModelEntity *const RESTRICT entity) NOEXCEPT;

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

	/*
	*	Updates the model system during the input update phase.
	*/
	void InputUpdate() NOEXCEPT;

};