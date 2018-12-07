#if !defined(CATALYST_SHADER_COMMON)
#define CATALYST_SHADER_COMMON

//Extensions.
#extension GL_ARB_separate_shader_objects : require
#extension GL_EXT_nonuniform_qualifier : require

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8
#define MAXIMUM_NUMBER_OF_TERRAIN_PATCHES 255

//Terrain patch data definition.
struct TerrainPatchData
{
    vec2 worldPosition;
    float patchSize;
    int borders;
    int heightTextureIndex;
    int normalTextureIndex;
    int materialTextureIndex;
    float materialTextureResolution;
    float inverseMaterialTextureResolution;
    int padding1;
    int padding2;
    int padding3;
};

//Terrain material data definition.
struct TerrainMaterialData
{
    int albedoTextureIndex;
    int normalMapTextureIndex;
    int materialPropertiesTextureIndex;
    int padding;
};

//Global uniform data.
layout (std140, set = 0, binding = 0) uniform GlobalUniformData
{
    //Viewer data.
    layout (offset = 0) float cameraFieldOfViewCosine; //Offset; 0 - Size; 16
    layout (offset = 16) mat4 inverseCameraMatrix; //Offset; 16 - Size; 64
    layout (offset = 80) mat4 inverseProjectionMatrix; //Offset; 80 - Size; 64
    layout (offset = 144) mat4 originViewMatrix; //Offset; 144 - Size; 64
    layout (offset = 208) mat4 viewMatrix; //Offset; 208 - Size; 64
    layout (offset = 272) vec3 cameraForwardVector; //Offset; 272 - Size; 16
    layout (offset = 288) vec3 cameraWorldPosition; //Offset; 288 - Size; 16

    //Directional light data.
    layout (offset = 304) float directionalLightIntensity; //Offset; 304 - Size; 16
    layout (offset = 320) mat4 directionalLightViewMatrix; //Offset; 320 - Size; 64
    layout (offset = 384) vec3 directionalLightDirection; //Offset; 384 - Size; 16
    layout (offset = 400) vec3 directionalLightColor; //Offset; 400 - Size; 16
    layout (offset = 416) vec3 directionalLightScreenSpacePosition; //Offset; 416 - Size; 16

    //Environment data.
    layout (offset = 432) float environmentBlend; //Offset; 432 - Size; 4

    //General data.
    layout (offset = 436) float deltaTime; //Offset; 436 - Size; 4
    layout (offset = 440) float totalGameTime; //Offset; 440 - Size; 4

    //Point light data.
    layout (offset = 444) int numberOfPointLights; //Offset; 444 - Size; 4
    layout (offset = 448) float pointLightAttenuationDistances[MaximumNumberOfPointLights]; //Offset; 448 - Size; 128
    layout (offset = 576) float pointLightIntensities[MaximumNumberOfPointLights]; //Offset; 576 - Size; 128
    layout (offset = 704) vec3 pointLightColors[MaximumNumberOfPointLights]; //Offset; 704 - Size; 128
    layout (offset = 832) vec3 pointLightWorldPositions[MaximumNumberOfPointLights]; //Offset; 832 - Size; 128

    //Spot light data.
    layout (offset = 960) int numberOfSpotLights; //Offset; 960 - Size; 16
    layout (offset = 976) float spotLightAttenuationDistances[MaximumNumberOfSpotLights]; //Offset; 976 - Size; 128
    layout (offset = 1104) float spotLightIntensities[MaximumNumberOfSpotLights]; //Offset; 1104 - Size; 128
    layout (offset = 1232) float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1232 - Size; 128
    layout (offset = 1360) float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1360 - Size; 128
    layout (offset = 1488) vec3 spotLightColors[MaximumNumberOfSpotLights]; //Offset; 1488 - Size; 128
    layout (offset = 1616) vec3 spotLightDirections[MaximumNumberOfSpotLights]; //Offset; 1616 - Size; 128
    layout (offset = 1744) vec3 spotLightWorldPositions[MaximumNumberOfSpotLights]; //Offset; 1744 - Size; 128

    //Physics data.
    layout (offset = 1872) float windSpeed; //Offset; 1872 - Size; 16
    layout (offset = 1888) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1904
};

//The global samplers.
layout (set = 0, binding = 1) uniform sampler globalSamplers[6];

//The global textures.
layout (set = 0, binding = 2) uniform texture2D globalTextures[512];

//The terrain height textures.
layout (set = 0, binding = 3) uniform sampler2D terrainHeightTextures[MAXIMUM_NUMBER_OF_TERRAIN_PATCHES];

//The terrain patch uniform data.
layout (std140, set = 0, binding = 4) uniform TerrainPatchUniformData
{
    layout (offset = 0) TerrainPatchData terrainPatchData[MAXIMUM_NUMBER_OF_TERRAIN_PATCHES];
};

//The terrain material uniform data.
layout (std140, set = 0, binding = 5) uniform TerrainMaterialUniformData
{
    layout (offset = 0) TerrainMaterialData terrainMaterialData[MAXIMUM_NUMBER_OF_TERRAIN_PATCHES];
};

//Constants.
#define FilterLinear_MipmapModeLinear_AddressModeClampToEdge_Index 0
#define FilterLinear_MipmapModeLinear_AddressModeRepeat_Index 1
#define FilterLinear_MipmapModeNearest_AddressModeClampToEdge_Index 2
#define FilterLinear_MipmapModeNearest_AddressModeRepeat_Index 3
#define FilterNearest_MipmapModeNearest_AddressModeClampToBorder_Index 4
#define FilterNearest_MipmapModeNearest_AddressModeClampToEdge_Index 5

#define EULERS_NUMBER (2.718281f)
#define PHI (1.618033f)
#define PI (3.141592f)
#define INVERSE_PI (0.318309f)
#define SQUARE_ROOT_OF_TWO (1.414213f)

/*
*   Defines the bit at the specified index.
*/
#define BIT(index) (1 << (index))

/*
*   Calculates the average of a fragment.
*/
float CalculateAverage(vec3 fragment)
{
    return fragment.r * 0.2126f + fragment.g * 0.7152f + fragment.b * 0.0722f;
}

/*
*   Calculates a fragment's world position.
*/
vec3 CalculateFragmentWorldPosition(vec2 textureCoordinate, float depth)
{
    vec2 nearPlaneCoordinate = textureCoordinate * 2.0f - 1.0f;
    vec4 viewSpacePosition = inverseProjectionMatrix * vec4(vec3(nearPlaneCoordinate, depth), 1.0f);
    float inverseViewSpacePositionDenominator = 1.0f / viewSpacePosition.w;
    viewSpacePosition *= inverseViewSpacePositionDenominator;
    vec4 worldSpacePosition = inverseCameraMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

/*
*   Returns the length of a vector with two components squared.
*/
float LengthSquared2(vec2 vector)
{
    return vector.x * vector.x + vector.y * vector.y;
}

/*
*   Returns the length of a vector with three components squared.
*/
float LengthSquared3(vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}


/*
*	Linear interpolation between three values.
*/
float LinearInterpolation(float a, float b, float c, float alpha)
{
	return alpha >= 0.5f ? mix(b, c, (alpha - 0.5f) * 2.0f) : mix(a, b, alpha  * 2.0f);
}

/*
*   Given a seed, returns a random number.
*/
float RandomFloat(vec3 seed)
{
    return fract(sin(dot(seed.xy * seed.z, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

/*
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmoothStep(float number)
{
    return number * number * (3.0f - 2.0f * number);
}

/*
*   Returns a smoothed vector in the range 0.0f-1.0f.
*/
vec3 SmoothStep(vec3 vector)
{
    return vec3(SmoothStep(vector.x), SmoothStep(vector.y), SmoothStep(vector.z));
}

/*
*   Returns a smoothed number in the range 0.0f-1.0f.
*/
float SmootherStep(float number)
{
    return number * number * number * (number * (number * 6.0f - 15.0f) + 10.0f);
}

/*
*   Returns a smoothed vector in the range 0.0f-1.0f.
*/
vec3 SmootherStep(vec3 vector)
{
    return vec3(SmootherStep(vector.x), SmootherStep(vector.y), SmootherStep(vector.z));
}

#endif