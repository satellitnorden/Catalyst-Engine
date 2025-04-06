#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Animation.
#include <Animation/Skeleton.h>

//Content.
#include <Content/Core/Asset.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

class AnimatedModelAsset final : public Asset
{

public:

	//The type identifier.
	static HashString TYPE_IDENTIFIER;

	//The model space axis-aligned bounding box.
	AxisAlignedBoundingBox3D _ModelSpaceAxisAlignedBoundingBox;

	//The vertex buffer.
	BufferHandle _VertexBuffer;

	//The index buffer.
	BufferHandle _IndexBuffer;

	//The index count.
	uint32 _IndexCount;

	//The parent transform.
	Matrix4x4 _ParentTransform;

	//The skeleton.
	Skeleton _Skeleton;

};