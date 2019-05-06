#if !defined(CATALYST_LIGHTING_DATA)
#define CATALYST_LIGHTING_DATA

//Constants.
#define MAXIMUM_NUMBER_OF_LIGHTS (4)

//Light struct definition.
struct Light
{
	vec3 color;
	vec3 position;
	float size;
  float strength;
};

//Light uniform data.
layout (std140, set = 2, binding = 0) uniform LightUniformData
{
  layout (offset = 0) int numberOfLights;
  layout (offset = 16) vec4[MAXIMUM_NUMBER_OF_LIGHTS * 2] lightData;
};
layout (set = 2, binding = 1, r32f) uniform image2D directionalLightVisibilityTexture;
layout (set = 2, binding = 2, r32f) uniform image2D lightsVisibilityTextures[MAXIMUM_NUMBER_OF_LIGHTS];

/*
*	Unpacks the light at the given index.
*/
Light UnpackLight(uint index)
{
	Light light;

  	vec4 lightData1 = lightData[index * 2 + 0];
  	vec4 lightData2 = lightData[index * 2 + 1];

  	light.color = lightData1.xyz;
  	light.position = vec3(lightData1.w, lightData2.xy);
  	light.size = lightData2.z;
  	light.strength = lightData2.w;

  	return light;
}

#endif