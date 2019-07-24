#if !defined(CATALYST_VEGETATION_UTILITIES)
#define CATALYST_VEGETATION_UTILITIES

//Preprocessor defines.
#define MAXIMUM_WIND_SPEED (30.0f)
#define INVERSE_MAXIMUM_WIND_SPEED (0.033333f)
#define WIND_SPEED_TIME_MULTIPLIER 2.0f

vec3 windDirection = vec3(1.0f, 0.0f, 1.0f);

/*
*	Calculates the wind displacement for vegetation.
*/
vec3 CalculateWindDisplacement(vec3 instancePosition, vec3 vertexPosition, vec3 vertexNormal, float time)
{
	//Calculate the wind speed multiplier.
	float windSpeedMultiplier = windSpeed * INVERSE_MAXIMUM_WIND_SPEED;

	//Calculate the extended wind speed multiplier.
	float extendedWindSpeedMultiplier = 1.0f + windSpeedMultiplier;

	//Calculate the time factor.
	float timeFactor = time * (1.0f + (windSpeedMultiplier * WIND_SPEED_TIME_MULTIPLIER));

    //Large scale motion.
    float largeScaleX = windDirection.x * extendedWindSpeedMultiplier * (sin(instancePosition.x + instancePosition.y + instancePosition.z + timeFactor) + 1.0f);
    float largeScaleZ = windDirection.z * extendedWindSpeedMultiplier * (sin(instancePosition.x + instancePosition.y + instancePosition.z + timeFactor) + 1.0f);

    //Medium scale motion.
    float mediumScaleX = windDirection.x + EULERS_NUMBER * sin(PHI * (instancePosition.x + instancePosition.y + instancePosition.z + timeFactor));
    float mediumScaleZ = windDirection.z + PHI * sin(EULERS_NUMBER * (instancePosition.x + instancePosition.y + instancePosition.z + timeFactor));

    //Small scale motion.
    vec3 smallScale = vec3(	INVERSE_PI * 8.0f * sin(PI * (vertexPosition.x + vertexPosition.y + vertexPosition.z + timeFactor)),
    						INVERSE_PI * 2.0f * sin(EULERS_NUMBER * (vertexPosition.x + vertexPosition.y + vertexPosition.z + timeFactor)),
    						INVERSE_PI * 4.0f * sin(DOUBLE_PI * (vertexPosition.x + vertexPosition.y + vertexPosition.z + timeFactor))) * vertexNormal;

    return vec3(largeScaleX + mediumScaleX + smallScale.x, smallScale.y, largeScaleZ + mediumScaleZ + smallScale.z) * windSpeedMultiplier;
}
#endif