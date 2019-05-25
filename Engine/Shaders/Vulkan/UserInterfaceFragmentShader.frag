//Version declaration.
#version 460

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Constants.
#define USER_INTERFACE_ELEMENT_TYPE_IMAGE (0)
#define USER_INTERFACE_ELEMENT_TYPE_TEXT (1)

//Layout specification.
layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 16) int type;
    layout (offset = 20) int textureIndex;
};

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

void main()
{
	switch (type)
	{
		case USER_INTERFACE_ELEMENT_TYPE_IMAGE:
		{
			//Write the fragment.
			   fragment = texture(globalTextures[textureIndex], fragmentTextureCoordinate);

			break;
		}

		case USER_INTERFACE_ELEMENT_TYPE_TEXT:
		{
			//Write the fragment.
			fragment = vec4(vec3(1.0f), texture(globalTextures[textureIndex], fragmentTextureCoordinate).r);

			break;
		}

		default:
		{
			//Write the fragment.
			fragment = texture(globalTextures[textureIndex], fragmentTextureCoordinate);

			break;
		}
	}
    
}