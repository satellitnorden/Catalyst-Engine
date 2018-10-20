//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in float fragmentLengthFactor;
layout (location = 1) in vec2 fragmentTextureCoordinate;
layout (location = 2) in mat3 fragmentTangentSpaceMatrix;

//Texture samplers.
layout (set = 1, binding = 1) uniform sampler2D albedoTexture;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

void main()
{
    //Discard this fragment according to the length factor.
    if (fragmentLengthFactor == 0.0f
        || fragmentLengthFactor < RandomFloat(gl_FragCoord.x * gl_FragCoord.y * gl_FragCoord.z))
    {
        discard;
    }

    //Write the albedo.
    albedo = texture(albedoTexture, fragmentTextureCoordinate);

    //Write the normal/depth.
    normalDepth = vec4(fragmentTangentSpaceMatrix[2], gl_FragCoord.z);

    //Write the material properties.
    materialProperties = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}