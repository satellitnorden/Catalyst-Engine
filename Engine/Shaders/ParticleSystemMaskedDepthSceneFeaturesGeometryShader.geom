//Push constant data.
layout (push_constant) uniform PushConstantData
{
    layout (offset = 0) vec3 POSITION_DELTA;
    layout (offset = 16) float LIFETIME;
    layout (offset = 20) float FADE_TIME;
};

//Layout specification.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//In parameters.
layout (location = 0) in vec3 geometry_positions[];
layout (location = 1) in vec2 geometry_scales[];
layout (location = 2) in float geometry_times[];

//Out parameters.
layout (location = 0) out vec2 fragment_texture_coordinate;
layout (location = 1) out float fragment_opacity;

void CatalystShaderMain()
{
	//Retrieve the world position.
	vec3 world_position = geometry_positions[0] + POSITION_DELTA;

	//Retrieve the scale.
	vec2 scale = geometry_scales[0];

	//Retrieve the time.
	float time = geometry_times[0];

	//Calculate the forward, up and right vectors.
	vec3 offset_camera_world_position = CAMERA_WORLD_POSITION - POSITION_DELTA;
	vec3 forward_vector = normalize(offset_camera_world_position - world_position);
	vec3 up_vector = vec3(0.0f, 1.0f, 0.0f);
	vec3 right_vector = normalize(cross(forward_vector, up_vector));
	
	//Calculate the opacity.
	float opacity = 1.0f;

	opacity *= min(time / FADE_TIME, 1.0f);
	opacity *= clamp(1.0f - ((time - (LIFETIME - FADE_TIME)) / FADE_TIME), 0.0f, 1.0f);
	opacity *= float(time <= LIFETIME);

	//Construct all the vertices.
	fragment_texture_coordinate = vec2(0.0f, 1.0f);
	fragment_opacity = opacity;
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position - right_vector * (scale.x * 0.5f) - up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	fragment_texture_coordinate = vec2(0.0f, 0.0f);
	fragment_opacity = opacity;
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position - right_vector * (scale.x * 0.5f) + up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	fragment_texture_coordinate = vec2(1.0f, 1.0f);
	fragment_opacity = opacity;
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position + right_vector * (scale.x * 0.5f) - up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	fragment_texture_coordinate = vec2(1.0f, 0.0f);
	fragment_opacity = opacity;
	gl_Position = WORLD_TO_CLIP_MATRIX * vec4(world_position + right_vector * (scale.x * 0.5f) + up_vector * (scale.y * 0.5f), 1.0f);

	EmitVertex();

	EndPrimitive();
}