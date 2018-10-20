//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    float firstLayerDisplacementHeight;
    float secondLayerDisplacementHeight;
    float thirdLayerDisplacementHeight;
    float fourthLayerDisplacementHeight;
    float fifthLayerDisplacementHeight;
};

//Layout specification.
layout (triangles, equal_spacing, ccw) in;

//In parameters.
layout (location = 0) in vec3 tessellationEvaluationPosition[];
layout (location = 1) in vec3 tessellationEvaluationNormal[];
layout (location = 2) in vec4 tessellationEvaluationLayerWeights[];
layout (location = 3) in vec2 tessellationEvaluationTextureCoordinate[];

//Texture samplers.
layout (set = 1, binding = 2) uniform sampler2D layer1MaterialPropertiesTexture;
layout (set = 1, binding = 5) uniform sampler2D layer2MaterialPropertiesTexture;
layout (set = 1, binding = 8) uniform sampler2D layer3MaterialPropertiesTexture;
layout (set = 1, binding = 11) uniform sampler2D layer4MaterialPropertiesTexture;
layout (set = 1, binding = 14) uniform sampler2D layer5MaterialPropertiesTexture;

//Out parameters.
layout (location = 0) out vec3 fragmentNormal;
layout (location = 1) out vec4 fragmentLayerWeights;

//Globals.
vec2 fragmentTextureCoordinate;

/*
*   Returns the displacement value.
*/
float GetDisplacement()
{
    float blend1 = mix(firstLayerDisplacementHeight, secondLayerDisplacementHeight, fragmentLayerWeights.x);
    float blend2 = mix(blend1, thirdLayerDisplacementHeight, fragmentLayerWeights.y);
    float blend3 = mix(blend2, fourthLayerDisplacementHeight, fragmentLayerWeights.z);

    float terrainDisplacementHeight = mix(blend3, fifthLayerDisplacementHeight, fragmentLayerWeights.w);

    float layer1Displacement = texture(layer1MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer2Displacement = texture(layer2MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer3Displacement = texture(layer3MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer4Displacement = texture(layer4MaterialPropertiesTexture, fragmentTextureCoordinate).w;
    float layer5Displacement = texture(layer5MaterialPropertiesTexture, fragmentTextureCoordinate).w;

    blend1 = mix(layer1Displacement, layer2Displacement, fragmentLayerWeights.x);
    blend2 = mix(blend1, layer3Displacement, fragmentLayerWeights.y);
    blend3 = mix(blend2, layer3Displacement, fragmentLayerWeights.z);

    return (mix(blend3, layer5Displacement, fragmentLayerWeights.w) * terrainDisplacementHeight) - (terrainDisplacementHeight * 0.5f);
}

void main()
{
    //Pass information to the tessellation evaluation shader.
    fragmentNormal = gl_TessCoord.x * tessellationEvaluationNormal[0] + gl_TessCoord.y * tessellationEvaluationNormal[1] + gl_TessCoord.z * tessellationEvaluationNormal[2];
    fragmentLayerWeights = gl_TessCoord.x * tessellationEvaluationLayerWeights[0] + gl_TessCoord.y * tessellationEvaluationLayerWeights[1] + gl_TessCoord.z * tessellationEvaluationLayerWeights[2];

    vec3 position = gl_TessCoord.x * tessellationEvaluationPosition[0] + gl_TessCoord.y * tessellationEvaluationPosition[1] + gl_TessCoord.z * tessellationEvaluationPosition[2];
    position += fragmentNormal * GetDisplacement();

    gl_Position = viewMatrix * vec4(position, 1.0f);
}