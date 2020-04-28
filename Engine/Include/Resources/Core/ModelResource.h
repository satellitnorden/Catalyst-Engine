#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Native/Mesh.h>

//Resources.
#include <Resources/Core/Resource.h>

class ModelResource final : public Resource
{

public:

	//The model space axis-aligned bounding box.
	AxisAlignedBoundingBox3 _ModelSpaceAxisAlignedBoundingBox;

	//Container for all the meshes.
	DynamicArray<Mesh> _Meshes;

};