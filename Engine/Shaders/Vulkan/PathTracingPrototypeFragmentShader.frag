//Version declaration.
#version 450

//Extensions.
#extension GL_GOOGLE_include_directive : enable

//Includes.
#include "CatalystShaderCommon.glsl"
#include "CatalystShaderPhysicallyBasedLighting.glsl"
#include "CatalystVectorMath.glsl"

//Preprocessor defines.
#define AMBIENT_OCCLUSION_MAXIMUM_RADIUS (10.0f)
#define GROUND_COLOR (vec3(0.25f, 0.25f, 0.25f))
#define GROUND_NORMAL (vec3(0.0f, 1.0f, 0.0f))
#define SKY_COLOR (vec3(0.8f, 0.9f, 1.0f))
#define NUMBER_OF_LIGHTS (1)
#define NUMBER_OF_SPHERES (5)

/*
*   Intersection data struct definition.
*/
struct IntersectionData
{
    vec3 _Albedo;
    vec3 _Normal;
    vec3 _Position;
    float _Roughness;
    float _Metallic;
};

/*
*   Light struct definition.
*/
struct Light
{
    vec3 _Color;
    vec3 _Position;
    float _Intensity;
};

/*
*   Sphere struct definition.
*/
struct Sphere
{
    vec3 _Albedo;
    vec3 _Position;
    float _Metallic;
    float _Radius;
    float _Roughness;
};

//Container for all lights.
Light LIGHTS[NUMBER_OF_LIGHTS] = Light[NUMBER_OF_LIGHTS]
(
    Light(vec3(1.0f, 0.9f, 0.8f), vec3(-5.0f, 5.0f, -5.0f), 12.5f)
    //Light(vec3(1.0f, 0.9f, 0.8f), vec3(5.0f, 5.0f, 5.0f), 2.5f),
    //Light(vec3(0.8f, 0.9f, 1.0f), vec3(-5.0f, 5.0f, 5.0f), 2.5f),
    //Light(vec3(0.8f, 0.9f, 1.0f), vec3(5.0f, 5.0f, -5.0f), 2.5f)
);

//Container for all spheres.
Sphere SPHERES[NUMBER_OF_SPHERES] = Sphere[NUMBER_OF_SPHERES]
(
    Sphere(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.5f, 0.0f), 0.0f, 0.5f, 1.0f),
    Sphere(vec3(0.0f, 1.0f, 0.0f), vec3(-4.0f, 1.0f, 0.0f), 0.25f, 1.0f, 0.75f),
    Sphere(vec3(0.0f, 0.0f, 1.0f), vec3(4.0f, 1.5f, 0.0f), 0.5f, 1.5f, 0.5f),
    Sphere(vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 2.0f, -4.0f), 0.75f, 2.0f, 0.25f),
    Sphere(vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, 2.5f, 4.0f), 1.0f, 2.5f, 0.0f)
);

//Layout specification.
layout (early_fragment_tests) in;

//In parameters.
layout (location = 0) in vec2 fragmentTextureCoordinate;

//Out parameters.
layout (location = 0) out vec4 fragment;

//Forward declarations.
float CalculateAmbientOcclusion(vec3 position, vec3 normal);
vec3 CalculateDirection();
vec3 CalculateLightingAtIntersection(IntersectionData intersectionData, vec3 perceivingPosition);
vec3 CalculateLightingInReflection(IntersectionData intersectionData, vec3 perceivingPosition);
float CalculateShadowMultiplier(vec3 lightPosition, vec3 origin, float lightRadius);
vec3 CalculateSpecularIrradiance(vec3 position, vec3 normal, vec3 viewDirection, float roughness);
vec3 GenerateRandomDirectionInHemisphere(vec3 normal);
vec3 GenerateRandomPosition(float radius);
bool IsCloser(vec3 position, vec3 A, vec3 B);
bool Intersect(vec3 origin, vec3 direction, out IntersectionData intersectionData);
bool IntersectGround(vec3 origin, vec3 direction, out IntersectionData intersectionData);
bool IntersectSpheres(vec3 origin, vec3 direction, out IntersectionData intersectionData);

/*
*   Calculates the ambient occlusion at the given position using the normal.
*/
float CalculateAmbientOcclusion(vec3 position, vec3 normal)
{
    //Generate a random direction.
    vec3 direction = GenerateRandomDirectionInHemisphere(normal);

    //Intersect in the direction, if it hits this point is occluded.
    IntersectionData temporary;
    return 1.0f - (Intersect(position, direction, temporary) ? 1.0f - min(length(position - temporary._Position) / AMBIENT_OCCLUSION_MAXIMUM_RADIUS, 1.0f) : 0.0f);
}

/*
*   Calculates the direction.
*/
vec3 CalculateDirection()
{
    //Calculate the fragment world position.
    vec3 fragmentWorldPosition = CalculateFragmentWorldPosition(fragmentTextureCoordinate, 1.0f);

    //The direction is from the perceiver to the fragment world position.
    return normalize(fragmentWorldPosition - perceiverWorldPosition);
}

/*
*   Calculates the lighting at the intersection.
*/
vec3 CalculateLightingAtIntersection(IntersectionData intersectionData, vec3 perceivingPosition)
{
    //Calculate the ambient lighting.
    vec3 lighting = vec3(0.0f);

    //Calculate the view direction.
    vec3 viewDirection = normalize(perceivingPosition - intersectionData._Position);

    //Calculate the specular irradiance.
    vec3 specularIrradiance = CalculateSpecularIrradiance(intersectionData._Position, intersectionData._Normal, viewDirection, intersectionData._Roughness);

    //Calculate the ambient.
    lighting += CalculateAmbient(intersectionData._Albedo, SKY_COLOR, intersectionData._Normal, specularIrradiance, viewDirection, CalculateAmbientOcclusion(intersectionData._Position, intersectionData._Normal), intersectionData._Metallic, intersectionData._Roughness);

    //Calculate all lights.
    for (int i = 0; i < NUMBER_OF_LIGHTS; ++i)
    {
        //Calculate the light direction.
        vec3 lightDirection = normalize(LIGHTS[i]._Position - intersectionData._Position);

        //Calculate the shadow multiplier.
        float shadowMultiplier = CalculateShadowMultiplier(LIGHTS[i]._Position, intersectionData._Position, 1.0f);

        lighting += CalculateLight(viewDirection, lightDirection, intersectionData._Normal, 1.0f, intersectionData._Roughness, intersectionData._Metallic, intersectionData._Albedo, LIGHTS[i]._Color * LIGHTS[i]._Intensity) * shadowMultiplier;
    }

    //Return the lighting.
    //return lighting;
    return vec3(CalculateAmbientOcclusion(intersectionData._Position, intersectionData._Normal));
}

/*
*   Calculates the lighting in the reflection.
*/
vec3 CalculateLightingInReflection(IntersectionData intersectionData, vec3 perceivingPosition)
{
    //Calculate the ambient lighting.
    vec3 lighting = vec3(0.0f);

    //Calculate the view direction.
    vec3 viewDirection = normalize(perceivingPosition - intersectionData._Position);

    //Calculate the specular irradiance.
    vec3 specularIrradiance = SKY_COLOR;

    //Calculate the ambient.
    lighting += CalculateAmbient(intersectionData._Albedo, SKY_COLOR, intersectionData._Normal, specularIrradiance, viewDirection, 1.0f, intersectionData._Metallic, intersectionData._Roughness);

    //Calculate all lights.
    for (int i = 0; i < NUMBER_OF_LIGHTS; ++i)
    {
        //Calculate the light direction.
        vec3 lightDirection = normalize(LIGHTS[i]._Position - intersectionData._Position);

        //Calculate the shadow multiplier.
        IntersectionData temporary;
        float shadowMultiplier = Intersect(intersectionData._Position, lightDirection, temporary) ? 0.0f : 1.0f;

        lighting += CalculateLight(viewDirection, lightDirection, intersectionData._Normal, 1.0f, intersectionData._Roughness, intersectionData._Metallic, intersectionData._Albedo, LIGHTS[i]._Color) * shadowMultiplier;
    }

    //Return the lighting.
    return lighting;
}

/*
*   Calculates the shadow multiplier for a given light.
*/
float CalculateShadowMultiplier(vec3 lightPosition, vec3 origin, float lightRadius)
{
    //Calculate the random offset
    vec3 randomOffset = GenerateRandomPosition(lightRadius);
    vec3 lightDirection = normalize(lightPosition + randomOffset - origin);

    //Calculate the shadow multiplier.
    IntersectionData temporary;
    return Intersect(origin, lightDirection, temporary) ? 0.0f : 1.0f;
}

/*
*   Calculates the specular irradiance.
*/
vec3 CalculateSpecularIrradiance(vec3 position, vec3 normal, vec3 viewDirection, float roughness)
{
    vec3 randomNormal = GenerateRandomDirectionInHemisphere(normal);
    vec3 reflectionNormal = mix(normal, randomNormal, roughness * roughness);
    vec3 reflectionDirection = reflect(-viewDirection, reflectionNormal);
    IntersectionData reflectionIntersectionData;

    if (Intersect(position, reflectionDirection, reflectionIntersectionData))
    {
        return CalculateLightingInReflection(reflectionIntersectionData, position);
    }

    else
    {
        return SKY_COLOR;
    }
}

/*
*   Generates a random normalized direction given the seed in the hemisphere defined by the normal.
*/
vec3 GenerateRandomDirectionInHemisphere(vec3 normal)
{
    //Generate the direction.
    vec3 direction = normalize( vec3(   RandomFloat(vec3(gl_FragCoord.xy, globalRandomSeed * EULERS_NUMBER)) * 2.0f - 1.0f,
                                        RandomFloat(vec3(gl_FragCoord.xy, globalRandomSeed * PHI)) * 2.0f - 1.0f,
                                        RandomFloat(vec3(gl_FragCoord.xy, globalRandomSeed * PI)) * 2.0f - 1.0f));

    //Flip the direction so that it fits within the hemisphere defined by the normal.
    direction *= dot(direction, normal) >= 0.0f ? 1.0f : -1.0f;

    //Return the direction.
    return direction;
}

/*
*   Generates a random position within the given radius.
*/
vec3 GenerateRandomPosition(float radius)
{
    //Generate the position.
    vec3 position = normalize( vec3(    RandomFloat(vec3(gl_FragCoord.xy, globalRandomSeed * INVERSE_PI)) * 2.0f - 1.0f,
                                        RandomFloat(vec3(gl_FragCoord.xy, globalRandomSeed * SQUARE_ROOT_OF_TWO)) * 2.0f - 1.0f,
                                        RandomFloat(vec3(gl_FragCoord.xy, globalRandomSeed * SQUARE_ROOT_OF_NINETY_NINE)) * 2.0f - 1.0f)) * radius;

    //Return the position.
    return position;
}

/*
*   Returns if A is closer to the position than B.
*/
bool IsCloser(vec3 position, vec3 A, vec3 B)
{
    return LengthSquared3(A - position) < LengthSquared3(B - position);
}

/*
*   Does an intersection against the scene, returns if there was an intersection and if so, fills out the intersection data.
*/
bool Intersect(vec3 origin, vec3 direction, out IntersectionData intersectionData)
{
    //First, intersect the floor.
    IntersectionData groundIntersectionData;
    bool intersectedFloor = IntersectGround(origin, direction, groundIntersectionData);

    //Secondly, intersect spheres.
    IntersectionData sphereIntersectionData;
    bool intersectedSpheres = IntersectSpheres(origin, direction, sphereIntersectionData);

    //Pick the nearest intersection point.
    if (intersectedFloor)
    {
        intersectionData = groundIntersectionData;
    }

    if (intersectedSpheres)
    {
        if (!intersectedFloor || (intersectedFloor && IsCloser(origin, sphereIntersectionData._Position, groundIntersectionData._Position)))
        {
            intersectionData = sphereIntersectionData;
        }
    }

    //Return if any intersection was found.
    return intersectedFloor || intersectedSpheres;
}

/*
*   Does an intersection against the floor, returns if there was an intersection and if so, fills out the intersection data.
*/
bool IntersectGround(vec3 origin, vec3 direction, out IntersectionData intersectionData)
{
    if (dot(direction, GROUND_NORMAL) >= 0.0f)
    {
        return false;
    }

    //Calculate the intersection point.
    vec3 intersectionPoint = dot(-perceiverWorldPosition, GROUND_NORMAL) / dot(direction, GROUND_NORMAL) * direction + perceiverWorldPosition;

    //Fill in the intersection data.
    intersectionData._Albedo = GROUND_COLOR;
    intersectionData._Normal = GROUND_NORMAL;
    intersectionData._Position = intersectionPoint;
    intersectionData._Roughness = 0.25f;
    intersectionData._Metallic = 0.75f;

    //Return that there was an intersection.
    return true;
}

/*
*   Does an intersection against all spheres, returns if there was an intersection and if so, fills out the intersection data.
*/
bool IntersectSpheres(vec3 origin, vec3 direction, out IntersectionData intersectionData)
{
    //Keep track of whether or not there was an intersection.
    bool foundIntersection = false;

    //Intersect all spheres.
    for (int i = 0; i < NUMBER_OF_SPHERES; ++i)
    {
        vec3 intersectionPoint;

        if (LineSphereIntersection(origin, direction, SPHERES[i]._Position, SPHERES[i]._Radius, intersectionPoint))
        {
            if (foundIntersection)
            {
                if (IsCloser(origin, intersectionPoint, intersectionData._Position))
                {
                    intersectionData._Albedo = SPHERES[i]._Albedo;
                    intersectionData._Normal = normalize(intersectionPoint - SPHERES[i]._Position);
                    intersectionData._Position = intersectionPoint;
                    intersectionData._Roughness = SPHERES[i]._Roughness;
                    intersectionData._Metallic = SPHERES[i]._Metallic;
                }
            }

            else
            {
                intersectionData._Albedo = SPHERES[i]._Albedo;
                intersectionData._Normal = normalize(intersectionPoint - SPHERES[i]._Position);
                intersectionData._Position = intersectionPoint;
                intersectionData._Roughness = SPHERES[i]._Roughness;
                intersectionData._Metallic = SPHERES[i]._Metallic;
            }

            foundIntersection = true;
        }
    }

    return foundIntersection;
}

void main()
{
    //Calculate the direction.
    vec3 direction = CalculateDirection();

    //Calculate the intersection data.
    IntersectionData intersectionData;

    //Test intersection against the scene.
    if (Intersect(perceiverWorldPosition, direction, intersectionData))
    {
        fragment = vec4(CalculateLightingAtIntersection(intersectionData, perceiverWorldPosition), 1.0f);
    }

    //If no intersection was found, paint the sky with.. the... sky color.
    else
    {
        fragment = vec4(SKY_COLOR, 1.0f);
    }
}