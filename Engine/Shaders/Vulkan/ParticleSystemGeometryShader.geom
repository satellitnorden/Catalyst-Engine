//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) vec3 particleSystemInitialMinimumPosition;
	layout (offset = 16) vec3 particleSystemInitialMaximumPosition;
	layout (offset = 32) vec3 particleSystemInitialMinimumVelocity;
	layout (offset = 48) vec3 particleSystemInitialMaximumVelocity;
	layout (offset = 64) vec3 particleSystemWorldPosition;
	layout (offset = 80) vec2 particleSystemInitialMinimumScale;
	layout (offset = 88) vec2 particleSystemInitialMaximumScale;
	layout (offset = 96) float fadeTime;
	layout (offset = 100) float lifetime;
	layout (offset = 104) float spawnFrequency;
	layout (offset = 108) int properties;
	layout (offset = 112) float particleSystemRandomSeed;
    layout (offset = 116) float particleSystemTotalTime;
};

//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//In parameters.
layout (location = 0) in int instanceIndex[];

//Out parameters.
layout (location = 0) out vec3 fragmentNormal;
layout (location = 1) out vec2 fragmentTextureCoordinate;
layout (location = 2) out float fragmentFadeFactor;

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
*	Generates a new random scale.
*/
vec2 GenerateRandomScale()
{
	float randomBlendFactor = RandomFloat(vec3(float(particleIndex) * 1.4f, float(particleIndex) * 1.3f, particleSystemRandomSeed * 0.1f));

	float randomX = mix(particleSystemInitialMinimumScale.x, particleSystemInitialMaximumScale.x, randomBlendFactor);
	float randomY = mix(particleSystemInitialMinimumScale.y, particleSystemInitialMaximumScale.y, randomBlendFactor);

	return vec2(randomX, randomY);
}

/*
*	Generates a random position.
*/
vec3 GenerateRandomPosition()
{
	float randomX = mix(particleSystemInitialMinimumPosition.x, particleSystemInitialMaximumPosition.x, RandomFloat(vec3(float(particleIndex) * 1.2f, float(particleIndex) * 1.0f, particleSystemRandomSeed * 0.2f)));
	float randomY = mix(particleSystemInitialMinimumPosition.y, particleSystemInitialMaximumPosition.y, RandomFloat(vec3(float(particleIndex) * 1.0f, float(particleIndex) * 0.9f, particleSystemRandomSeed * 0.3f)));
	float randomZ = mix(particleSystemInitialMinimumPosition.z, particleSystemInitialMaximumPosition.z, RandomFloat(vec3(float(particleIndex) * 0.8f, float(particleIndex) * 0.7f, particleSystemRandomSeed * 0.4f)));

	return vec3(randomX, randomY, randomZ) + particleSystemWorldPosition;
}

/*
*	Generates a random velocity.
*/
vec3 GenerateRandomVelocity()
{
	float randomX = mix(particleSystemInitialMinimumVelocity.x, particleSystemInitialMaximumVelocity.x, RandomFloat(vec3(float(particleIndex) * 0.6f, float(particleIndex) * 0.5f, particleSystemRandomSeed * 0.5f)));
	float randomY = mix(particleSystemInitialMinimumVelocity.y, particleSystemInitialMaximumVelocity.y, RandomFloat(vec3(float(particleIndex) * 0.4f, float(particleIndex) * 0.3f, particleSystemRandomSeed * 0.6f)));
	float randomZ = mix(particleSystemInitialMinimumVelocity.z, particleSystemInitialMaximumVelocity.z, RandomFloat(vec3(float(particleIndex) * 0.2f, float(particleIndex) * 0.1f, particleSystemRandomSeed * 0.7f)));

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
		vec3 forwardVector = normalize(perceiverWorldPosition - position);
		vec3 rightVector = normalize(cross(forwardVector, vec3(0.0f, 1.0f, 0.0f)));
		vec3 upVector = cross(forwardVector, rightVector);

		//Construct all the vertices.
		vec3 worldPosition;

		fragmentNormal = forwardVector;
		fragmentTextureCoordinate = vec2(0.0f, 0.0f);
		fragmentFadeFactor = fadeFactor;
		worldPosition = vec3((-rightVector * scale.x) + (-upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(worldPosition, 1.0f);

		EmitVertex();

		fragmentNormal = forwardVector;
		fragmentTextureCoordinate = vec2(0.0f, 1.0f);
		fragmentFadeFactor = fadeFactor;
		worldPosition = vec3((-rightVector * scale.x) + (upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(worldPosition, 1.0f);

		EmitVertex();

		fragmentNormal = forwardVector;
		fragmentTextureCoordinate = vec2(1.0f, 0.0f);
		fragmentFadeFactor = fadeFactor;
		worldPosition = vec3((rightVector * scale.x) + (-upVector * scale.y) + position);
		
		gl_Position = viewMatrix * vec4(worldPosition, 1.0f);

		EmitVertex();

		fragmentNormal = forwardVector;
		fragmentTextureCoordinate = vec2(1.0f, 1.0f);
		fragmentFadeFactor = fadeFactor;
		worldPosition = vec3((rightVector * scale.x) + (upVector * scale.y) + position);

		gl_Position = viewMatrix * vec4(worldPosition, 1.0f);

		EmitVertex();

		EndPrimitive();
	}
}