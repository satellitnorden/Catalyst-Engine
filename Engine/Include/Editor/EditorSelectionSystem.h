#if defined(CATALYST_EDITOR)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Core/Entity.h>

//Math.
#include <Math/Geometry/Ray.h>

class EditorSelectionSystem final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE EditorSelectionSystem() NOEXCEPT
	{
		Memory::Set(this, 0, sizeof(EditorSelectionSystem));
	}

	/*
	*	Initializes the editor selection system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the editor selection system.
	*/
	void Update() NOEXCEPT;

	/*
	*	Returns the currently selected entity.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD Entity *const RESTRICT GetCurrentlySelectedEntity() const NOEXCEPT
	{
		return _CurrentlySelectedEntity;
	}

	/*
	*	Sets the currently selected entity.
	*/
	void SetCurrentlySelectedEntity(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Sets the currently selected entity override.
	*/
	FORCE_INLINE void SetCurrentlySelectedEntityOverride(Entity *const RESTRICT value) NOEXCEPT
	{
		_CurrentlySelectedEntityOverride = value;
	}

private:

	/*
	*	Dynamic model selection data class definition.
	*/
	class DynamicModelSelectionData final
	{

	public:

		//Denotes if the user is currently selecting a model resource.
		bool _IsSelectingModelResource{ false };

		//Denotes if the user is currently selecting a material resource.
		bool _IsSelectingMaterialResource{ false };

		//The material index currently being selected.
		uint8 _SelectedMaterialIndex{ 0 };

	};

	/*
	*	Static model selection data class definition.
	*/
	class StaticModelSelectionData final
	{

	public:

		//Denotes if the user is currently selecting a model resource.
		bool _IsSelectingModelResource{ false };

		//Denotes if the user is currently selecting a material resource.
		bool _IsSelectingMaterialResource{ false };

		//The material index currently being selected.
		uint8 _SelectedMaterialIndex{ 0 };

	};

	/*
	*	User interface selection data class definition.
	*/
	class UserInterfaceSelectionData final
	{

	public:

		//Denotes if the user is currently selecting the scene.
		bool _IsSelectingScene{ false };

	};

	//The currently selected entity.
	Entity *RESTRICT _CurrentlySelectedEntity{ nullptr };

	//The currently selected entity override.
	Entity *RESTRICT _CurrentlySelectedEntityOverride{ nullptr };

	//Denotes if an entity was recently duplicated.
	bool _DuplicatedEntity{ false };

	union
	{
		//The dynamic model selection data.
		DynamicModelSelectionData _DynamicModelSelectionData;

		//The static model selection data.
		StaticModelSelectionData _StaticModelSelectionData;

		//The user interface selection data.
		UserInterfaceSelectionData _UserInterfaceSelectionData;
	};

	/*
	*	Transforms the currently selected entity.
	*/
	void TransformCurrentlySelectedEntity(const Ray &ray);

	/*
	*	Adds a position editor.
	*/
	NO_DISCARD bool AddPositionEditor(Vector3<float32> *const RESTRICT value) NOEXCEPT;

};
#endif
