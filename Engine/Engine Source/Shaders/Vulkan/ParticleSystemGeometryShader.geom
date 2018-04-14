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

    //General data.
    layout (offset = 368) float totalGameTime; //Offset; 368 - Size; 4

    //Point light data.
    layout (offset = 372) int numberOfPointLights; //Offset; 372 - Size; 12
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

//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//Preprocessor defines.
#define MaximumNumberOfParticles 16384

//The shader buffer storage.
layout (std140, set = 1, binding = 1) buffer ParticleSystemData
{ 
	float spawnTimer;
	int nextParticleToSpawn;

	bool isSpawned[MaximumNumberOfParticles];
	vec2 particleScales[MaximumNumberOfParticles];
	vec3 particlePositions[MaximumNumberOfParticles];
	vec3 particleVelocities[MaximumNumberOfParticles];
};

//In parameters
in int geometryParticleID[];

//Uniforms.
uniform float randomSeed;
uniform float spawnFrequency;
uniform vec2 particleSystemInitialMaximumScale;
uniform vec2 particleSystemInitialMinimumScale;
uniform vec3 particleSystemInitialMaximumPosition;
uniform vec3 particleSystemInitialMinimumPosition;
uniform vec3 particleSystemInitialMaximumVelocity;
uniform vec3 particleSystemInitialMinimumVelocity;
uniform vec3 particleSystemWorldPosition;

//Out parameters.
out vec2 fragmentTextureCoordinate;
out vec3 fragmentNormal;
out vec3 fragmentWorldPosition;

/*
*	Given a seed, generate a random float in the 0.0f to 1.0f range.
*/
float RandomFloat(float seed)
{
	return fract(sin(seed) * 100000.0f);
}

/*
*	Generates a new random scale.
*/
vec2 GenerateRandomScale()
{
	float randomX = mix(particleSystemInitialMinimumScale.x, particleSystemInitialMaximumScale.x, RandomFloat(randomSeed * 0.1f));
	float randomY = mix(particleSystemInitialMinimumScale.y, particleSystemInitialMaximumScale.y, RandomFloat(randomSeed * 0.2f));

	return vec2(randomX, randomY);
}

/*
*	Generates a random position.
*/
vec3 GenerateRandomPosition()
{
	float randomX = mix(particleSystemInitialMinimumPosition.x, particleSystemInitialMaximumPosition.x, RandomFloat(randomSeed * 0.3f));
	float randomY = mix(particleSystemInitialMinimumPosition.y, particleSystemInitialMaximumPosition.y, RandomFloat(randomSeed * 0.4f));
	float randomZ = mix(particleSystemInitialMinimumPosition.z, particleSystemInitialMaximumPosition.z, RandomFloat(randomSeed * 0.5f));

	return vec3(randomX, randomY, randomZ) + particleSystemWorldPosition;
}

/*
*	Generates a random velocity.
*/
vec3 GenerateRandomVelocity()
{
	float randomX = mix(particleSystemInitialMinimumVelocity.x, particleSystemInitialMaximumVelocity.x, RandomFloat(randomSeed * 0.6f));
	float randomY = mix(particleSystemInitialMinimumVelocity.y, particleSystemInitialMaximumVelocity.y, RandomFloat(randomSeed * 0.7f));
	float randomZ = mix(particleSystemInitialMinimumVelocity.z, particleSystemInitialMaximumVelocity.z, RandomFloat(randomSeed * 0.8f));

	return vec3(randomX, randomY, randomZ);
}

void main()
{
	//Handle unspawned particles.
	if (gl_PrimitiveIDIn == nextParticleToSpawn)
	{
		//If this particle is the next particle to spawn, then it is responsible for increasing the spawn timer.
		spawnTimer += deltaTime;

		if (spawnTimer >= spawnFrequency)
		{
			spawnTimer -= spawnFrequency;

			isSpawned[gl_PrimitiveIDIn] = true;

			particleScales[gl_PrimitiveIDIn] = GenerateRandomScale();
			particlePositions[gl_PrimitiveIDIn] = GenerateRandomPosition();
			particleVelocities[gl_PrimitiveIDIn] = GenerateRandomVelocity();

			nextParticleToSpawn = nextParticleToSpawn <= MaximumNumberOfParticles - 1 ? nextParticleToSpawn + 1 : 0;
		}
	}

	//Handle spawned particles.
	if (isSpawned[gl_PrimitiveIDIn] == true)
	{
		//Modify the position of the particle.
		particlePositions[gl_PrimitiveIDIn] += particleVelocities[gl_PrimitiveIDIn] * deltaTime;

		//Cache the properties for this specific particle.
		vec2 scale = particleScales[gl_PrimitiveIDIn];
		vec3 position = particlePositions[gl_PrimitiveIDIn];

		//Calculate the forward, right and up vector.
		vec3 forwardVector = normalize(cameraWorldPosition - position);
		vec3 rightVector = normalize(cross(forwardVector, vec3(0.0f, 1.0f, 0.0f)));
		vec3 upVector = cross(forwardVector, rightVector);

		//Calculate the fragment normal.
		vec3 normal = normalize(forwardVector);

		//Construct all the vertices.
		fragmentTextureCoordinate = vec2(0.0f, 0.0f);
		fragmentNormal = normal;
		fragmentWorldPosition = vec3((-rightVector * scale.x) + (-upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		EmitVertex();

		fragmentTextureCoordinate = vec2(0.0f, 1.0f);
		fragmentNormal = normal;
		fragmentWorldPosition = vec3((-rightVector * scale.x) + (upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		EmitVertex();

		fragmentTextureCoordinate = vec2(1.0f, 0.0f);
		fragmentNormal = normal;
		fragmentWorldPosition = vec3((rightVector * scale.x) + (-upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		EmitVertex();

		fragmentTextureCoordinate = vec2(1.0f, 1.0f);
		fragmentNormal = normal;
		fragmentWorldPosition = vec3((rightVector * scale.x) + (upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(fragmentWorldPosition, 1.0f);

		EmitVertex();

		EndPrimitive();
	}
}