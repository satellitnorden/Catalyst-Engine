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
    layout (offset = 1872) float windSpeed; //Offset; 1872 - Size; 16
    layout (offset = 1888) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1904
};

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 particleSystemWorldPosition;
    layout (offset = 16) float particleSystemRandomSeed;
    layout (offset = 20) float particleSystemTotalTime;
};

//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//The particle system uniform data.
layout (std140, set = 2, binding = 0) uniform ParticleSystemUniformData
{ 
	layout (offset = 0) int properties;
	layout (offset = 4) float fadeTime;
	layout (offset = 8) float lifetime;
	layout (offset = 12) float spawnFrequency;
	layout (offset = 16) vec2 particleSystemInitialMinimumScale;
	layout (offset = 24) vec2 particleSystemInitialMaximumScale;
	layout (offset = 32) vec3 particleSystemInitialMinimumPosition;
	layout (offset = 48) vec3 particleSystemInitialMaximumPosition;
	layout (offset = 64) vec3 particleSystemInitialMinimumVelocity;
	layout (offset = 80) vec3 particleSystemInitialMaximumVelocity;
};

//In parameters.
layout (location = 0) in int instanceIndex[];

//Out parameters.
layout (location = 0) out float fragmentFadeFactor;
layout (location = 1) out vec2 fragmentTextureCoordinate;
layout (location = 2) out vec2 fragmentSceneTextureCoordinate;
layout (location = 3) out vec3 fragmentWorldPosition;

//Globals.
int particleIndex;

/*
*	Returns whether or not this particle system is affected by wind.
*/
bool IsAffectedByWind()
{
	return (properties & (1 << 0)) == (1 << 0);
}

/*
*	Returns whether or not this particle system is looping.
*/
bool IsLooping()
{
	return (properties & (1 << 1)) == (1 << 1);
}

/*
*	Given a seed, generate a random float in the 0.0f to 1.0f range.
*/
float RandomFloat(float seed)
{
	return fract(sin(seed) * cos(seed * float(particleIndex)));
}

/*
*	Generates a new random scale.
*/
vec2 GenerateRandomScale()
{
	float randomBlendFactor = RandomFloat(float(particleIndex) * 0.9f + particleSystemRandomSeed * 0.1f);

	float randomX = mix(particleSystemInitialMinimumScale.x, particleSystemInitialMaximumScale.x, randomBlendFactor);
	float randomY = mix(particleSystemInitialMinimumScale.y, particleSystemInitialMaximumScale.y, randomBlendFactor);

	return vec2(randomX, randomY);
}

/*
*	Generates a random position.
*/
vec3 GenerateRandomPosition()
{
	float randomX = mix(particleSystemInitialMinimumPosition.x, particleSystemInitialMaximumPosition.x, RandomFloat(float(particleIndex) * 0.8f + particleSystemRandomSeed * 0.2f));
	float randomY = mix(particleSystemInitialMinimumPosition.y, particleSystemInitialMaximumPosition.y, RandomFloat(float(particleIndex) * 0.7f + particleSystemRandomSeed * 0.3f));
	float randomZ = mix(particleSystemInitialMinimumPosition.z, particleSystemInitialMaximumPosition.z, RandomFloat(float(particleIndex) * 0.6f + particleSystemRandomSeed * 0.4f));

	return vec3(randomX, randomY, randomZ) + particleSystemWorldPosition;
}

/*
*	Generates a random velocity.
*/
vec3 GenerateRandomVelocity()
{
	float randomX = mix(particleSystemInitialMinimumVelocity.x, particleSystemInitialMaximumVelocity.x, RandomFloat(float(particleIndex) * 0.5f + particleSystemRandomSeed * 0.5f));
	float randomY = mix(particleSystemInitialMinimumVelocity.y, particleSystemInitialMaximumVelocity.y, RandomFloat(float(particleIndex) * 0.4f + particleSystemRandomSeed * 0.6f));
	float randomZ = mix(particleSystemInitialMinimumVelocity.z, particleSystemInitialMaximumVelocity.z, RandomFloat(float(particleIndex) * 0.3f + particleSystemRandomSeed * 0.7f));

	return vec3(randomX, randomY, randomZ) + (IsAffectedByWind() ? windDirection * windSpeed * 0.2f : vec3(0.0f));
}

/*
*	Calculates the fade factor for one particle.
*/
float CalculateFadeFactor(float currentLifetime)
{
	return currentLifetime < fadeTime ? currentLifetime / fadeTime : currentLifetime > (lifetime - fadeTime) ? abs(lifetime - currentLifetime) / fadeTime : 1.0f;
}

void main()
{
	//Set the particle index.
	particleIndex = instanceIndex[0];

	//Calculate this particle's spawn time.
	float particleSpawnTime = float(particleIndex) * spawnFrequency;

	//Calculate the current iteration.
	int iteration = int((particleSystemTotalTime - particleSpawnTime) / lifetime);

	//Caluate this iteration's spawn time.
	particleSpawnTime = particleSpawnTime + lifetime * iteration;

	//Is this particle spawned?
	if (particleSystemTotalTime >= particleSpawnTime && (IsLooping() || iteration == 0))
	{
		//Calculate the current lifetime.
		float currentLifetime = particleSystemTotalTime - particleSpawnTime;

		//Calculate the fade factor.
		float fadeFactor = CalculateFadeFactor(currentLifetime);

		//Calculate the position.
		vec3 position = GenerateRandomPosition() + GenerateRandomVelocity() * currentLifetime;

		//Calculate the scale.
		vec2 scale = GenerateRandomScale();

		//Calculate the forward, right and up vector.
		vec3 forwardVector = normalize(cameraWorldPosition - position);
		vec3 rightVector = normalize(cross(forwardVector, vec3(0.0f, 1.0f, 0.0f)));
		vec3 upVector = cross(forwardVector, rightVector);

		//Construct all the vertices.
		vec4 screenSpacePosition;

		fragmentFadeFactor = fadeFactor;
		fragmentTextureCoordinate = vec2(0.0f, 0.0f);
		fragmentWorldPosition = vec3((-rightVector * scale.x) + (-upVector * scale.y) + position);
		screenSpacePosition = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		gl_Position = screenSpacePosition;

		fragmentSceneTextureCoordinate = vec2(screenSpacePosition.xy / screenSpacePosition.w * 0.5f + 0.5f);

		EmitVertex();

		fragmentFadeFactor = fadeFactor;
		fragmentTextureCoordinate = vec2(0.0f, 1.0f);
		fragmentWorldPosition = vec3((-rightVector * scale.x) + (upVector * scale.y) + position);
		screenSpacePosition = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		gl_Position = screenSpacePosition;

		fragmentSceneTextureCoordinate = vec2(screenSpacePosition.xy / screenSpacePosition.w * 0.5f + 0.5f);

		EmitVertex();

		fragmentFadeFactor = fadeFactor;
		fragmentTextureCoordinate = vec2(1.0f, 0.0f);
		fragmentWorldPosition = vec3((rightVector * scale.x) + (-upVector * scale.y) + position);
		screenSpacePosition = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		gl_Position = screenSpacePosition;

		fragmentSceneTextureCoordinate = vec2(screenSpacePosition.xy / screenSpacePosition.w * 0.5f + 0.5f);

		EmitVertex();

		fragmentFadeFactor = fadeFactor;
		fragmentTextureCoordinate = vec2(1.0f, 1.0f);
		fragmentWorldPosition = vec3((rightVector * scale.x) + (upVector * scale.y) + position);
		screenSpacePosition = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		gl_Position = screenSpacePosition;

		fragmentSceneTextureCoordinate = vec2(screenSpacePosition.xy / screenSpacePosition.w * 0.5f + 0.5f);

		EmitVertex();

		EndPrimitive();
	}
}