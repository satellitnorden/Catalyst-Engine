#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix4.h>

class StaticPhysicalRenderComponent final
{

public:

	//Denotes whether or not this static physical model is in the view frustum.
	bool _IsInViewFrustum;

	//The model matrix.
	Matrix4 _ModelMatrix;

	//The render data table.
	RenderDataTableHandle _RenderDataTable;

	//The buffer.
	ConstantBufferHandle _Buffer;

	//The offset for the indices in the buffer.
	uint64 _IndexOffset{ 0 };

	//The index count.
	uint32 _IndexCount{ 0 };

};