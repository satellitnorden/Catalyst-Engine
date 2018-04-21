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
    layout (offset = 320) vec3 directionalLightDirection; //Offset; 320 - Size; 16
    layout (offset = 336) vec3 directionalLightColor; //Offset; 336 - Size; 16
    layout (offset = 352) vec3 directionalLightScreenSpacePosition; //Offset; 352 - Size; 16

    //Environment data.
    layout (offset = 368) float environmentBlend; //Offset; 368 - Size; 4

    //General data.
    layout (offset = 372) float deltaTime; //Offset; 368 - Size; 4
    layout (offset = 376) float totalGameTime; //Offset; 376 - Size; 4

    //Point light data.
    layout (offset = 380) int numberOfPointLights; //Offset; 380 - Size; 4
    layout (offset = 384) float pointLightAttenuationDistances[MaximumNumberOfPointLights]; //Offset; 384 - Size; 128
    layout (offset = 512) float pointLightIntensities[MaximumNumberOfPointLights]; //Offset; 512 - Size; 128
    layout (offset = 640) vec3 pointLightColors[MaximumNumberOfPointLights]; //Offset; 640 - Size; 128
    layout (offset = 768) vec3 pointLightWorldPositions[MaximumNumberOfPointLights]; //Offset; 768 - Size; 128

    //Spot light data.
    layout (offset = 896) int numberOfSpotLights; //Offset; 896 - Size; 16
    layout (offset = 912) float spotLightAttenuationDistances[MaximumNumberOfSpotLights]; //Offset; 912 - Size; 128
    layout (offset = 1040) float spotLightIntensities[MaximumNumberOfSpotLights]; //Offset; 1040 - Size; 128
    layout (offset = 1168) float spotLightInnerCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1168 - Size; 128
    layout (offset = 1296) float spotLightOuterCutoffAngles[MaximumNumberOfSpotLights]; //Offset; 1296 - Size; 128
    layout (offset = 1424) vec3 spotLightColors[MaximumNumberOfSpotLights]; //Offset; 1424 - Size; 128
    layout (offset = 1552) vec3 spotLightDirections[MaximumNumberOfSpotLights]; //Offset; 1552 - Size; 128
    layout (offset = 1680) vec3 spotLightWorldPositions[MaximumNumberOfSpotLights]; //Offset; 1680 - Size; 128

    //Physics data.
    layout (offset = 1808) float windStrength; //Offset; 1808 - Size; 16
    layout (offset = 1824) vec3 windDirection; //Offset; 1824 - Size; 16

    //Total size; 1840
};

//Push constant data.
layout (push_constant) uniform PushConstantData
{
    float particleSystemRandomSeed;
};

//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//Preprocessor defines.
#define MaximumNumberOfParticles 16384

//The particle system storage data.
layout (std140, set = 1, binding = 1) buffer ParticleSystemStorageData
{ 
	layout (offset = 0) float spawnTimer; //Size: 4
	layout (offset = 4) int nextParticleToSpawn; //Size: 4

	layout (offset = 16) bool isSpawned[MaximumNumberOfParticles]; //Size: 262 144
	layout (offset = 262160) float lifetimes[MaximumNumberOfParticles]; //Size: 262 144
	layout (offset = 524304) vec2 particleScales[MaximumNumberOfParticles]; //Size: 262 144
	layout (offset = 786448) vec3 particlePositions[MaximumNumberOfParticles]; //Size: 262 144
	layout (offset = 1048592) vec3 particleVelocities[MaximumNumberOfParticles]; //Size: 262 144

	//Total size; 1310736
};

//The particle system uniform data.
layout (std140, set = 1, binding = 2) uniform ParticleSystemUniformData
{ 
	layout (offset = 0) float fadeTime;
	layout (offset = 4) float lifetime;
	layout (offset = 8) float spawnFrequency;
	layout (offset = 16) vec2 particleSystemInitialMinimumScale;
	layout (offset = 24) vec2 particleSystemInitialMaximumScale;
	layout (offset = 32) vec3 particleSystemInitialMinimumPosition;
	layout (offset = 48) vec3 particleSystemInitialMaximumPosition;
	layout (offset = 64) vec3 particleSystemInitialMinimumVelocity;
	layout (offset = 80) vec3 particleSystemInitialMaximumVelocity;
	layout (offset = 96) vec3 particleSystemWorldPosition;

	//Total size; 112
};

//In parameters.
layout (location = 0) in int vertexIndex[];

//Out parameters.
layout (location = 0) out float fragmentFadeFactor;
layout (location = 1) out vec2 fragmentTextureCoordinate;
layout (location = 2) out vec3 fragmentNormal;
layout (location = 3) out vec3 fragmentWorldPosition;

/*
*	Given a seed, generate a random float in the 0.0f to 1.0f range.
*/
float RandomFloat(float seed)
{
	return fract(sin(seed * 100.0f) * 100000.0f);
}

/*
*	Generates a new random scale.
*/
vec2 GenerateRandomScale()
{
	float randomBlendFactor = RandomFloat(particleSystemRandomSeed * 0.2f);

	float randomX = mix(particleSystemInitialMinimumScale.x, particleSystemInitialMaximumScale.x, randomBlendFactor);
	float randomY = mix(particleSystemInitialMinimumScale.y, particleSystemInitialMaximumScale.y, randomBlendFactor);

	return vec2(randomX, randomY);
}

/*
*	Generates a random position.
*/
vec3 GenerateRandomPosition()
{
	float randomX = mix(particleSystemInitialMinimumPosition.x, particleSystemInitialMaximumPosition.x, RandomFloat(particleSystemRandomSeed * 0.3f));
	float randomY = mix(particleSystemInitialMinimumPosition.y, particleSystemInitialMaximumPosition.y, RandomFloat(particleSystemRandomSeed * 0.4f));
	float randomZ = mix(particleSystemInitialMinimumPosition.z, particleSystemInitialMaximumPosition.z, RandomFloat(particleSystemRandomSeed * 0.5f));

	return vec3(randomX, randomY, randomZ) + particleSystemWorldPosition;
}

/*
*	Generates a random velocity.
*/
vec3 GenerateRandomVelocity()
{
	float randomX = mix(particleSystemInitialMinimumVelocity.x, particleSystemInitialMaximumVelocity.x, RandomFloat(particleSystemRandomSeed * 0.6f));
	float randomY = mix(particleSystemInitialMinimumVelocity.y, particleSystemInitialMaximumVelocity.y, RandomFloat(particleSystemRandomSeed * 0.7f));
	float randomZ = mix(particleSystemInitialMinimumVelocity.z, particleSystemInitialMaximumVelocity.z, RandomFloat(particleSystemRandomSeed * 0.8f));

	return vec3(randomX, randomY, randomZ);
}

/*
*	Calculates the fade factor for one particle.
*/
float CalculateFadeFactor(int index)
{
	float currentLifetime = lifetimes[index];
	return currentLifetime < fadeTime ? currentLifetime / fadeTime : currentLifetime > (lifetime - fadeTime) ? abs(lifetime - currentLifetime) / fadeTime : 1.0f;
}

void main()
{
	int particleIndex = vertexIndex[0];

	//Handle unspawned particles.
	if (particleIndex == nextParticleToSpawn)
	{
		//If this particle is the next particle to spawn, then it is responsible for increasing the spawn timer.
		spawnTimer += deltaTime;

		if (spawnTimer >= spawnFrequency)
		{
			spawnTimer -= spawnFrequency;

			isSpawned[particleIndex] = true;
			lifetimes[particleIndex] = 0.0f;

			particleScales[particleIndex] = GenerateRandomScale();
			particlePositions[particleIndex] = GenerateRandomPosition();
			particleVelocities[particleIndex] = GenerateRandomVelocity();

			nextParticleToSpawn = nextParticleToSpawn < MaximumNumberOfParticles - 1 ? nextParticleToSpawn + 1 : 0;
		}
	}

	//Handle spawned particles.
	if (isSpawned[particleIndex] == true)
	{
		//Update the lifetime of this particle.
		lifetimes[particleIndex] += deltaTime;

		//If this particle has outlive it's lifetime, unspawn it.
		if (lifetimes[particleIndex] >= lifetime)
		{
			isSpawned[particleIndex] = false;

			return;
		}

		//Calculate the fade factor.
		float fadeFactor = CalculateFadeFactor(particleIndex);

		//Modify the position of the particle.
		particlePositions[particleIndex] += particleVelocities[particleIndex] * deltaTime;

		//Cache the properties for this specific particle.
		vec2 scale = particleScales[particleIndex];
		vec3 position = particlePositions[particleIndex];

		//Calculate the forward, right and up vector.
		vec3 forwardVector = normalize(cameraWorldPosition - position);
		vec3 rightVector = normalize(cross(forwardVector, vec3(0.0f, 1.0f, 0.0f)));
		vec3 upVector = cross(forwardVector, rightVector);

		//Calculate the fragment normal.
		vec3 normal = normalize(forwardVector);

		//Construct all the vertices.
		fragmentFadeFactor = fadeFactor;
		fragmentTextureCoordinate = vec2(0.0f, 0.0f);
		fragmentNormal = normal;
		fragmentWorldPosition = vec3((-rightVector * scale.x) + (-upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		EmitVertex();

		fragmentFadeFactor = fadeFactor;
		fragmentTextureCoordinate = vec2(0.0f, 1.0f);
		fragmentNormal = normal;
		fragmentWorldPosition = vec3((-rightVector * scale.x) + (upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		EmitVertex();

		fragmentFadeFactor = fadeFactor;
		fragmentTextureCoordinate = vec2(1.0f, 0.0f);
		fragmentNormal = normal;
		fragmentWorldPosition = vec3((rightVector * scale.x) + (-upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		EmitVertex();

		fragmentFadeFactor = fadeFactor;
		fragmentTextureCoordinate = vec2(1.0f, 1.0f);
		fragmentNormal = normal;
		fragmentWorldPosition = vec3((rightVector * scale.x) + (upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		EmitVertex();

		EndPrimitive();
	}
}