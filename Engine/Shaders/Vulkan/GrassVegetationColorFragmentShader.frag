//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 4) float thickness;
};

//In parameters.
layout (location = 0) in mat3 fragmentTangentSpaceMatrix;
layout (location = 3) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 1) uniform sampler2D albedoTexture;
layout (set = 1, binding = 2) uniform sampler2D normalMapTexture;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

void main()
{
    //Write the albedo.
    albedo = texture(albedoTexture, fragmentTextureCoordinate);

    //Write the normal/depth.
    vec3 normal = texture(normalMapTexture, fragmentTextureCoordinate).xyz * 2.0f - 1.0f;
    normal = fragmentTangentSpaceMatrix * normal;
    normal *= gl_FrontFacing ? 1.0f : -1.0f;
    normalDepth = vec4(normal, gl_FragCoord.z);

    //Write the material properties.
    materialProperties = vec4(1.0f, 0.0f, 1.0f, thickness);
}