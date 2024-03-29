#if !defined(CATALYST_TRANSPARENCY)
#define CATALYST_TRANSPARENCY

/*
*   Calculates the distance based on the opacity.
*/
float CalculateDistanceBasedOpacity(float current_distance_squared, float maximum_distance_squared)
{
    return 1.0f - clamp((current_distance_squared - (maximum_distance_squared * 0.5f)) / (maximum_distance_squared * 0.5f), 0.0f, 1.0f);
}

/*
*	Based on the X and Y coordinate and an opacity value, return if the fragment should clip.
*/
bool ShouldClip(uint X, uint Y, float opacity, bool reverse)
{
	mat4 CLIP_THRESHOLDS = mat4
	(
		0.05882352941176470588235294117647f, 0.52941176470588235294117647058824f, 0.17647058823529411764705882352941f, 0.64705882352941176470588235294118f,
		0.76470588235294117647058823529412f, 0.29411764705882352941176470588235f, 0.88235294117647058823529411764706f, 0.41176470588235294117647058823529f,
		0.23529411764705882352941176470588f, 0.70588235294117647058823529411765f, 0.1176470588235294117647058823529f,  0.58823529411764705882352941176471f,
		0.94117647058823529411764705882353f, 0.47058823529411764705882352941176f, 0.82352941176470588235294117647059f, 0.3529411764705882352941176470588f
	);

	float clip_threshold = CLIP_THRESHOLDS[X & 3][Y & 3];
	clip_threshold = reverse ? 1.0f - clip_threshold : clip_threshold;

	return (opacity - clip_threshold) < 0.0f;
}

#endif