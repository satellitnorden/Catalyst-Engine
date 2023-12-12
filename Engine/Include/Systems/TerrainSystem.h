#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Components.
#include <Components/Transient/TerrainComponent.h>

class ALIGN(8) TerrainSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TerrainSystem);

	/*
	*	Default constructor.
	*/
	TerrainSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the terrain system.
	*/
	void Initialize() NOEXCEPT;

private:

	/*
	*	Updates the terrain system during the PRE update phase.
	*/
	void PreUpdate() NOEXCEPT;

	/*
	*	Checks combination of a node.
	*/
	void CheckCombination(TerrainComponent *const RESTRICT component, const Vector3<float32>& camera_position, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Checks subdivisions of a node.
	*/
	void CheckSubdivision(TerrainComponent *const RESTRICT component, const Vector3<float32>& camera_position, TerrainQuadTreeNode* const RESTRICT node) NOEXCEPT;

	/*
	*	Calculates borders for the given node.
	*/
	void CalculateBorders(TerrainComponent *const RESTRICT component, TerrainQuadTreeNode *const RESTRICT node) NOEXCEPT;

};