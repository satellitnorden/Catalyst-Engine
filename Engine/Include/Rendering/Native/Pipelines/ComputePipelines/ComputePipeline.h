#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>
#include <Resources/Core/ShaderResource.h>

class ComputePipeline : public Pipeline
{

public:

	/*
	*	Default constructor.
	*/
	ComputePipeline() NOEXCEPT;

	/*
	*	Returns the shader.
	*/
	FORCE_INLINE NO_DISCARD ShaderHandle GetShader() const NOEXCEPT
	{
		return _Shader;
	}

protected:

	/*
	*	Resets this compute pipeline.
	*/
	void ResetComputePipeline() NOEXCEPT;

	/*
	*	Sets the shader.
	*/
	FORCE_INLINE void SetShader(const ShaderHandle shader) NOEXCEPT
	{
		_Shader = shader;
	}

private:

	//The shader.
	ShaderHandle _Shader;

};