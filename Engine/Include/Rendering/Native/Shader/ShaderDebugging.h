/*
*	This file is set up to be able to essentially copy-paste shader code, without modification, and debug it.
*	Won't be perfectly 1-to-1, but might help get to the bottom of some stuff.
*/
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/RenderingSystem.h>

//////////////////////////
// CAMERA FUNCTIONALITY //
//////////////////////////

#define FAR_PLANE (RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFarPlane())
#define NEAR_PLANE (RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetNearPlane())
#define WORLD_TO_CLIP_MATRIX (*RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetViewMatrix())
#define INVERSE_CAMERA_TO_CLIP_MATRIX (*RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetInverseProjectionMatrix())
#define INVERSE_WORLD_TO_CAMERA_MATRIX (*RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetInverseCameraMatrix())

/*
*   Linearizes a depth value.
*/
float LinearizeDepth(float depth)
{
    return ((FAR_PLANE * NEAR_PLANE) / (depth * (FAR_PLANE - NEAR_PLANE) + NEAR_PLANE));
}

/*
*   Calculates the view space position.
*/
vec3 CalculateViewSpacePosition(vec2 texture_coordinate, float depth)
{
    vec2 near_plane_coordinate = texture_coordinate * 2.0f - 1.0f;
    vec4 view_space_position = INVERSE_CAMERA_TO_CLIP_MATRIX * vec4(vec3(near_plane_coordinate, depth), 1.0f);
    float inverse_view_space_position_denominator = 1.0f / view_space_position.w;
    view_space_position.x *= inverse_view_space_position_denominator;
    view_space_position.y *= inverse_view_space_position_denominator;
    view_space_position.z *= inverse_view_space_position_denominator;

    return vec3(view_space_position.x, view_space_position.y, view_space_position.z);
}

/*
*   Calculates the world position.
*/
vec3 CalculateWorldPosition(vec2 screen_coordinate, float depth)
{
    vec2 near_plane_coordinate = screen_coordinate * 2.0f - 1.0f;
    vec4 view_space_position = INVERSE_CAMERA_TO_CLIP_MATRIX * vec4(vec3(near_plane_coordinate, depth), 1.0f);
    float inverse_view_space_position_denominator = 1.0f / view_space_position.w;
    view_space_position *= inverse_view_space_position_denominator;
    vec4 world_space_position = INVERSE_WORLD_TO_CAMERA_MATRIX * view_space_position;

    return vec3(world_space_position.x, world_space_position.y, world_space_position.z);
}

/*
*   Returns the current screen coordinate with the given view matrix and world position.
*/
vec2 CalculateScreenCoordinate(vec3 world_position)
{
    vec4 view_space_position = WORLD_TO_CLIP_MATRIX * vec4(world_position, 1.0f);
    float denominator = 1.0f / view_space_position.w;
    view_space_position.x *= denominator;
    view_space_position.y *= denominator;

    return vec2(view_space_position.x, view_space_position.y) * 0.5f + 0.5f;
}

/*
*   Calculates a screen position, including the (linearized) depth from the given world position.
*/
vec3 CalculateScreenPosition(vec3 world_position)
{
    vec4 view_space_position = WORLD_TO_CLIP_MATRIX * vec4(world_position, 1.0f);
    float view_space_position_coefficient_reciprocal = 1.0f / view_space_position.w;
    view_space_position.x *= view_space_position_coefficient_reciprocal;
    view_space_position.y *= view_space_position_coefficient_reciprocal;
    view_space_position.z *= view_space_position_coefficient_reciprocal;

    view_space_position.x = view_space_position.x * 0.5f + 0.5f;
    view_space_position.y = view_space_position.y * 0.5f + 0.5f;
    view_space_position.z = LinearizeDepth(view_space_position.z);

    return vec3(view_space_position.x, view_space_position.y, view_space_position.z);
}