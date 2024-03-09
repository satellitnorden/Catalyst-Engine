#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Content.
#include <Content/Core/Asset.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

//Physics
#include <Physics/Native/PhysicsCore.h>

//Rendering.
#include <Rendering/Native/Mesh.h>

class ModelAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The model space axis-aligned bounding box.
	AxisAlignedBoundingBox3D _ModelSpaceAxisAlignedBoundingBox;

	//Container for all the meshes.
	DynamicArray<Mesh> _Meshes;

	//The collision model.
	CollisionModelHandle _CollisionModel;

};