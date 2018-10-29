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
layout (set = 1, binding = 2) uniform sampler2D normalMapTexture;
layout (set = 1, binding = 3) uniform sampler2D materialPropertiesTexture;

//Out parameters.
layout (location = 0) out vec4 albedo;
layout (location = 1) out vec4 normalDepth;
layout (location = 2) out vec4 materialProperties;

void main()
{
    //Discard this fragment according to the length factor.
    if (fragmentLengthFactor == 0.0f
        || fragmentLengthFactor < RandomFloat(gl_FragCoord.xyz))
    {
        discard;
    }

    //Write the albedo.
    albedo = texture(albedoTexture, fragmentTextureCoordinate);

    //Write the normal/depth.
    vec3 normal = texture(normalMapTexture, fragmentTextureCoordinate).xyz * 2.0f - 1.0f;
    normal = fragmentTangentSpaceMatrix * normal;
    normalDepth = vec4(normal, gl_FragCoord.z);

    //Sample the material properties.
    vec4 materialPropertiesSampler = texture(materialPropertiesTexture, fragmentTextureCoordinate);

    //Write the roughness.
    materialProperties.r = materialPropertiesSampler.r;

    //Write the metallic.
    materialProperties.g = materialPropertiesSampler.g;

    //Write the ambient occlusion.
    materialProperties.b = materialPropertiesSampler.b;

    //Write the thickness.
    materialProperties.a = 1.0f;
}