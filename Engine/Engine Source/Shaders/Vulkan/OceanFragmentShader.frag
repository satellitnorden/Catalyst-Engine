//Version declaration.
#version 450
#extension GL_ARB_separate_shader_objects : enable

//Preprocessor defines.
#define MaximumNumberOfPointLights 8
#define MaximumNumberOfSpotLights 8

layout (std140, set = 0, binding = 0) uniform DynamicUniformData
{
    //Camera data.
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
    layout (offset = 1872) float windStrength; //Offset; 1872 - Size; 16
    layout (offset = 1888) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1904
};

//Preprocessor defines.
#define MAXIMUM_WAVE_HEIGHT 1.0f
#define NUMBER_OF_ITERATIONS 3

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Texture samplers.
layout (set = 1, binding = 0) uniform samplerCube nightTexture;
layout (set = 1, binding = 2) uniform samplerCube dayTexture;
layout (set = 2, binding = 0) uniform sampler2D sceneTexture;
layout (set = 2, binding = 1) uniform sampler2D sceneNormalDepthTexture;
layout (set = 2, binding = 2) uniform sampler2D oceanNormalTexture;

//Out parameters.
layout (location = 0) out vec4 fragment;

//Globals.
vec3 intersectionPoint;
vec3 normalDirection;
vec3 sceneWorldPosition;

/*
*   Calculates the reflection.
*/
vec3 CalculateReflection()
{
    vec3 viewDirection = sceneWorldPosition - cameraWorldPosition;
    normalDirection = texture(oceanNormalTexture, (intersectionPoint.xz * 0.025f) + (vec2(totalGameTime, totalGameTime) * 0.025f)).xzy * 2.0f - 1.0f;
    vec3 compareVector = cameraWorldPosition.y >= 0.0f ? vec3(0.0f, 1.0f, 0.0f) : vec3(0.0f, -1.0f, 0.0f);
    normalDirection = mix(normalDirection, compareVector, 0.75f);

    vec3 reflectionDirection = reflect(viewDirection, normalDirection);

    return mix(texture(nightTexture, reflectionDirection).rgb, texture(dayTexture, reflectionDirection).rgb, environmentBlend);
}

/*
*   Calculates the world position.
*/
vec3 CalculateWorldPosition(vec2 textureCoordinate, float depth)
{
    vec2 nearPlaneCoordinate = textureCoordinate * 2.0f - 1.0f;
    vec3 fragmentScreenSpacePosition = vec3(nearPlaneCoordinate, depth);
    vec4 viewSpacePosition = inverseProjectionMatrix * vec4(fragmentScreenSpacePosition, 1.0f);
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = inverseCameraMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

void main()
{
	 //Sample the depth of the scene at this point.
    float sceneDepth = texture(sceneNormalDepthTexture, fragmentTextureCoordinate).a;

    //Calculate the scene world position.
    sceneWorldPosition = CalculateWorldPosition(fragmentTextureCoordinate, sceneDepth);

    //Calculate the intersection point.
    vec3 lineDirection = normalize(cameraWorldPosition - sceneWorldPosition);
    intersectionPoint = (dot(-sceneWorldPosition, vec3(0.0f, 1.0f, 0.0f)) / dot (lineDirection, vec3(0.0f, 1.0f, 0.0f))) * lineDirection + sceneWorldPosition;

	//Calculate the reflection.
    vec3 reflection = CalculateReflection();

    //Calculate the foam weight.
    //float distanceToBottom = length(sceneWorldPosition - intersectionPoint);
    //float foamWeight = 1.0f - min(distanceToBottom / 25.0f, 1.0f);

    //Sample the scene texture.
    vec2 sceneTextureCoordinate = sceneWorldPosition.y > 0.0f ? fragmentTextureCoordinate : fragmentTextureCoordinate + normalDirection.xz;
    vec4 sceneTextureSampler = texture(sceneTexture, sceneTextureCoordinate);

    //Calculate the transparency.
    float transparency = 1.0f - clamp(dot(normalDirection, normalize(cameraWorldPosition - sceneWorldPosition)), 0.0f, 1.0f);

    //Calculate the final ocean color.
    vec3 finalOceanColor = sceneWorldPosition.y > 0.0f ? sceneTextureSampler.rgb : mix(sceneTextureSampler.rgb, reflection, transparency);

    //Write the fragment
    fragment = vec4(finalOceanColor, 1.0f);
}

/*
*	Calculates the tangent frame.
*/
mat3 CalculateTangentFrame(vec3 normal, vec3 view, vec2 textureCoordinate)
{
	vec3 dp1 = dFdx(view);
	vec3 dp2 = dFdy(view);

	vec2 duv1 = dFdx(textureCoordinate);
	vec2 duv2 = dFdy(textureCoordinate);

	mat3 m = mat3(dp1, dp2, cross(dp1, dp2));
	mat2x3 inverseM = mat2x3(cross(m[1], m[2]), cross(m[2], m[0]));
	vec3 tangent = inverseM * vec2(duv1.x, duv2.x);
	vec3 binormal = inverseM * vec2(duv1.y, duv2.y);

	return mat3(normalize(tangent), normalize(binormal), normal);
}

/*
*	Given a position in world space, calculate it's texture coordinate for lookup in height/normal maps.
*/
vec2 CalculateTextureCoordinate(vec3 position)
{
	return position.xz * 0.025f;
}

/*
void main()
{
	//Calculate the scene world position.
    float sceneDepth = texture(sceneNormalDepthTexture, fragmentTextureCoordinate).a;
    sceneWorldPosition = CalculateWorldPosition(fragmentTextureCoordinate, sceneDepth);

    //If the scene world position is above the water level + the maximum wave height, do nothing.
    if (sceneWorldPosition.y > MAXIMUM_WAVE_HEIGHT)
    {
    	//Write the fragment.
		fragment = texture(sceneTexture, fragmentTextureCoordinate);
    }

    //Else, calculate the ocean color.
	else
	{
		//Calculate the view direction.
		vec3 viewDirection = normalize(sceneWorldPosition - cameraWorldPosition);

    	//Store the current water level.
    	float waterLevel = 0.0f;
    	vec3 intersectionPoint;

    	for (int i = 0; i < NUMBER_OF_ITERATIONS; ++i)
    	{
    		//Find the intersection point.
    		intersectionPoint = (dot(vec3(0.0f, waterLevel, 0.0f) - sceneWorldPosition, vec3(0.0f, 1.0f, 0.0f)) / dot(viewDirection, vec3(0.0f, 1.0f, 0.0f))) * viewDirection + sceneWorldPosition;

    		//Sample the height at the intersection point.
    		float height = max(dot(texture(oceanNormalTexture, intersectionPoint.xz * 0.025f).xzy, vec3(0.0f, 1.0f, 0.0f)), 0.0f) * MAXIMUM_WAVE_HEIGHT;

    		//Update the water level.
    		waterLevel += height;
    	}

    	//Calculate the accumulated water.
    	float accumulatedWater = length(sceneWorldPosition - intersectionPoint);

    	//Calculate the depth of the water.
    	float waterDepth = intersectionPoint.y - sceneWorldPosition.y;

    	//Calculate the normal vector.
    	vec2 normalTextureCoordinate = CalculateTextureCoordinate(intersectionPoint);

    	float W = max(dot(texture(oceanNormalTexture, normalTextureCoordinate).xzy, vec3(0.0f, 1.0f, 0.0f)), 0.0f) * MAXIMUM_WAVE_HEIGHT;
    	float E = max(dot(texture(oceanNormalTexture, normalTextureCoordinate + vec2(0.0f, 0.0009f)).xzy, vec3(0.0f, 1.0f, 0.0f)), 0.0f) * MAXIMUM_WAVE_HEIGHT;
    	float S = max(dot(texture(oceanNormalTexture, normalTextureCoordinate + vec2(0.0009f, 0.0009f)).xzy, vec3(0.0f, 1.0f, 0.0f)), 0.0f) * MAXIMUM_WAVE_HEIGHT;
    	float N = max(dot(texture(oceanNormalTexture, normalTextureCoordinate + vec2(0.0009f, 0.0f)).xzy, vec3(0.0f, 1.0f, 0.0f)), 0.0f) * MAXIMUM_WAVE_HEIGHT;

    	vec3 normal = vec3(W - E, 2.0f * waterDepth, S - N);

    	mat3 tangentSpaceMatrix = CalculateTangentFrame(normal, viewDirection, normalTextureCoordinate);

    	normal = normalize(tangeSpaceMatrix * (2.0f * texture(oceanNormalTexture, normalTextureCoordinate).xyz - 1.0f));

		//Write the fragment.
		//fragment = texture(sceneTexture, intersectionPoint.xz * 0.025f);

		fragment = vec4(normal, 1.0f);
	}
}
*/