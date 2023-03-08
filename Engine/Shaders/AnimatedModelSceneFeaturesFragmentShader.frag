//Includes.
#include "CatalystPackingUtilities.glsl"

layout (early_fragment_tests) in;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 128) int albedoTextureIndex;
    layout (offset = 132) int normalMapTextureIndex;
    layout (offset = 136) int materialPropertiesIndex;
    layout (offset = 140) int materialPropertyFlags;
    layout (offset = 144) float luminanceMultiplier;
};

//In parameters.
layout (location = 0) in mat3 fragmentTangentSpaceMatrix;
layout (location = 3) in vec3 fragmentPreviousWorldPosition;
layout (location = 4) in vec3 fragmentCurrentWorldPosition;
layout (location = 5) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 sceneFeatures1;
layout (location = 1) out vec4 sceneFeatures2;
layout (location = 2) out vec4 sceneFeatures3;
layout (location = 3) out vec4 scene_features_4;

void CatalystShaderMain()
{
    //Sample the albedo.
   vec3 albedo = texture(sampler2D(GLOBAL_TEXTURES[albedoTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate).rgb;

   //Sample the normal map.
   vec3 normalMap = texture(sampler2D(GLOBAL_TEXTURES[normalMapTextureIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate).xyz;

   //Sample the material properties.
   vec4 materialProperties = texture(sampler2D(GLOBAL_TEXTURES[materialPropertiesIndex], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_LINEAR_MIPMAP_MODE_LINEAR_ADDRESS_MODE_REPEAT_INDEX]), fragmentTextureCoordinate);

   //Calculate the shading normal.
    vec3 shadingNormal = normalMap * 2.0f - 1.0f;
    shadingNormal = fragmentTangentSpaceMatrix * shadingNormal;
    shadingNormal = normalize(shadingNormal);


    //Calculate the velocity.
    vec2 velocity = CalculateScreenCoordinate(WORLD_TO_CLIP_MATRIX, fragmentCurrentWorldPosition) - CalculateScreenCoordinate(PREVIOUS_WORLD_TO_CLIP_MATRIX, fragmentPreviousWorldPosition) - CURRENT_FRAME_JITTER;

    //Write the fragments.
    sceneFeatures1 = vec4(albedo, 1.0f);
    sceneFeatures2 = vec4(shadingNormal, gl_FragCoord.z);
    sceneFeatures3 = materialProperties;
    scene_features_4 = vec4(velocity, 0.0f, 0.0f);
}