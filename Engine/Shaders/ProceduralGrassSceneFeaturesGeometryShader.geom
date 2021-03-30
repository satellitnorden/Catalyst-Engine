//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

//Push constant data.
layout (push_constant) uniform PushConstantData
{
	layout (offset = 0) uvec2 RESOLUTION;
	layout (offset = 8) vec2 RESOLUTION_RECIPROCAL;
	layout (offset = 16) float DISTANCE_BETWEEN_BLADES;
};

//In parameters.
layout (location = 0) in uint geometry_indices[];

//Out parameters.
layout (location = 0) out vec3 fragment_albedo;
layout (location = 1) out vec3 fragment_normal;
layout (location = 2) out vec4 fragment_material_properties;

/*
*	Returns the random world position offset.
*/
vec3 GetRandomWorldPositionOffset(uint world_position_hash)
{
	vec2 LOOKUP[] = vec2[]
	(
		vec2(-0.5f, -0.5f),
		vec2(-0.375f, 0.0f),
		vec2(-0.25f, -0.25f),
		vec2(-0.125f, 0.25f),
		vec2(0.0f, -0.375f),
		vec2(0.125f, 0.125f),
		vec2(0.25f, -0.125f),
		vec2(0.375f, 0.375f)
	);

	vec2 offset_coordinate = LOOKUP[world_position_hash & 7];

	return vec3(offset_coordinate.x * DISTANCE_BETWEEN_BLADES, 0.0f, offset_coordinate.y * DISTANCE_BETWEEN_BLADES);
}

/*
*	Returns the random right vector .
*/
vec3 GetRandomRightVector(uint world_position_hash)
{
	vec3 LOOKUP[] = vec3[]
	(
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.707107f, 0.0f, -0.707107f),
		vec3(3.4946e-07f, 0.0f, -1.0f),
		vec3(-0.707106f, 0.0f, -0.707107f),
		vec3(-1.0f, 0.0f, -3.4946e-07f),
		vec3(-0.707106f, 0.0f, 0.707106f),
		vec3(3.4946e-07f, 0.0f, 1.0f),
		vec3(0.707107f, 0.0f, 0.707106f)
	);

	return LOOKUP[world_position_hash & 7];
}

void CatalystShaderMain()
{
	//Convert the vertex index to a screen coordinate.
	uint vertex_index = geometry_indices[0];

	vec2 screen_coordinate;

	screen_coordinate.x = float(mod(vertex_index, RESOLUTION.x)) * RESOLUTION_RECIPROCAL.x;
	screen_coordinate.y = float(vertex_index / RESOLUTION.x) * RESOLUTION_RECIPROCAL.y;

	//Retrieve the albedo/material. at the screen coordinate.
	vec4 scene_features_1 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_1_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), screen_coordinate);
	//vec3 albedo = scene_features_1.rgb;
	vec3 albedo = vec3(0.0392156877f, 0.0627451017, 0.00000000f);
	uint material_index = uint(scene_features_1.w * float(UINT8_MAXIMUM));
	Material material = GLOBAL_MATERIALS[uint(scene_features_1.w * float(UINT8_MAXIMUM))];

	//Should test property flag instead.
	if (material_index == 11)
	{
		//Retrieve the normal/depth at the screen coordinate.
		vec4 scene_features_2 = texture(sampler2D(RENDER_TARGETS[SCENE_FEATURES_2_RENDER_TARGET_INDEX], GLOBAL_SAMPLERS[GLOBAL_SAMPLER_FILTER_NEAREST_MIPMAP_MODE_NEAREST_ADDRESS_MODE_CLAMP_TO_EDGE_INDEX]), screen_coordinate);
		vec3 normal = scene_features_2.xyz;
		float depth = scene_features_2.w;

		//Retrieve the material properties.
		vec4 material_properties = vec4(1.0f, 0.0f, 1.0f, 0.0f);

		//Calculate the world position.
		vec3 world_position = CalculateWorldPosition(screen_coordinate, depth);

		//Constrain the world position to the grid.
		world_position.x = round(world_position.x / DISTANCE_BETWEEN_BLADES) * DISTANCE_BETWEEN_BLADES;
		world_position.z = round(world_position.z / DISTANCE_BETWEEN_BLADES) * DISTANCE_BETWEEN_BLADES;

		//Calculate the world position hash.
		uint world_position_hash = Hash2(uvec2(floatBitsToUint(world_position.x), floatBitsToUint(world_position.z)));

		//Apply the world position offset.
		world_position += GetRandomWorldPositionOffset(world_position_hash);

		//Construct the right vector.
		vec3 right_vector = GetRandomRightVector(world_position_hash);

		//Construct all the vertices.
		fragment_albedo = albedo;
		fragment_normal = normal;
		fragment_material_properties = material_properties;
		gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position + right_vector * -0.025f, 1.0f);

		EmitVertex();

		fragment_albedo = albedo;
		fragment_normal = normal;
		fragment_material_properties = material_properties;
		gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position + right_vector * 0.025f, 1.0f);

		EmitVertex();

		fragment_albedo = albedo;
		fragment_normal = normal;
		fragment_material_properties = material_properties;
		gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position + vec3(0.0f, 0.5f, 0.0f), 1.0f);

		EmitVertex();

		EndPrimitive();
	}
}