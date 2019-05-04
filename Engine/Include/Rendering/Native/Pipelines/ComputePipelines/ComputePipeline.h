#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Pipelines/Core/Pipeline.h>

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
	FORCE_INLINE NO_DISCARD Shader GetShader() const NOEXCEPT
	{
		return _Shader;
	}

protected:

	/*
	*	Sets the shader.
	*/
	FORCE_INLINE void SetShader(const Shader shader) NOEXCEPT
	{
		_Shader = shader;
	}

private:

	//The shader.
	Shader _Shader;

};