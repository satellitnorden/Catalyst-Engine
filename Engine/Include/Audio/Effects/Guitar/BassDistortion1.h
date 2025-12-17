#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Effects/Core/AudioEffect.h>
#include <Audio/Effects/General/HighPassFilter.h>
#include <Audio/Effects/General/LowPassFilter.h>
#include <Audio/Effects/General/PeakFilter.h>
#include <Audio/OverSampler.h>

//Math.
#include <Math/General/Curve.h>

/*
*	A bass distortion unit that splits the signal in a low, mid and high band and processes them differently.
*/
class BassDistortion1 final : public AudioEffect
{

public:
	
	//Constants.
	constexpr static float32 LOW_BAND_LOW_PASS_MINIMUM_FREQUENCY{ 50.0f };
	constexpr static float32 LOW_BAND_LOW_PASS_MAXIMUM_FREQUENCY{ 200.0f };
	constexpr static float32 LOW_BAND_LOW_PASS_DEFAULT_FREQUENCY{ 100.0f };

	constexpr static float32 MID_BAND_HIGH_PASS_MINIMUM_FREQUENCY{ 200.0f };
	constexpr static float32 MID_BAND_HIGH_PASS_MAXIMUM_FREQUENCY{ 8'000.0f };
	constexpr static float32 MID_BAND_HIGH_PASS_DEFAULT_FREQUENCY{ 400.0f };

	constexpr static float32 MID_BAND_LOW_PASS_MINIMUM_FREQUENCY{ 2'000.0f };
	constexpr static float32 MID_BAND_LOW_PASS_MAXIMUM_FREQUENCY{ 8'000.0f };
	constexpr static float32 MID_BAND_LOW_PASS_DEFAULT_FREQUENCY{ 4'000.0f };

	constexpr static float32 HIGH_BAND_HIGH_PASS_MINIMUM_FREQUENCY{ 1'000.0f };
	constexpr static float32 HIGH_BAND_HIGH_PASS_MAXIMUM_FREQUENCY{ 8'000.0f };
	constexpr static float32 HIGH_BAND_HIGH_PASS_DEFAULT_FREQUENCY{ 2'000.0f };

	//Denotes whether or not the low band is enabled.
	bool _LowBandEnabled{ true };

	//The low band low pass frequency.
	float32 _LowBandLowPassFrequency{ LOW_BAND_LOW_PASS_DEFAULT_FREQUENCY };

	//The low band compression.
	float32 _LowBandCompression{ 0.5f };

	//The low band gain.
	float32 _LowBandGain{ 1.0f };

	//Denotes whether or not the mid band is enabled.
	bool _MidBandEnabled{ true };

	//The mid band high pass frequency.
	float32 _MidBandHighPassFrequency{ MID_BAND_HIGH_PASS_DEFAULT_FREQUENCY };

	//The mid band low pass frequency.
	float32 _MidBandLowPassFrequency{ MID_BAND_HIGH_PASS_DEFAULT_FREQUENCY };

	//The mid band distortion.
	float32 _MidBandDistortion{ 0.5f };

	//The mid band gain.
	float32 _MidBandGain{ Audio::DecibelsToGain(-3.0f) };

	//Denotes whether or not the high band is enabled.
	bool _HighBandEnabled{ true };

	//The high band high pass frequency.
	float32 _HighBandHighPassFrequency{ HIGH_BAND_HIGH_PASS_MAXIMUM_FREQUENCY };

	//The high band distortion.
	float32 _HighBandDistortion{ 0.5f };

	//The high band gain.
	float32 _HighBandGain{ Audio::DecibelsToGain(-6.0f) };

	//Debug. (:
	float32 _Debug{ 0.0f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE BassDistortion1() NOEXCEPT
	{
		//Set up the output curves.
		_LowBandOutputCurve.SetValue(0, 13.337015f); //22.501173dB
		_LowBandOutputCurve.SetValue(1, 13.274120f); //22.460115dB
		_LowBandOutputCurve.SetValue(2, 13.212544f); //22.419729dB
		_LowBandOutputCurve.SetValue(3, 13.152327f); //22.380051dB
		_LowBandOutputCurve.SetValue(4, 13.093461f); //22.341089dB
		_LowBandOutputCurve.SetValue(5, 13.035904f); //22.302822dB
		_LowBandOutputCurve.SetValue(6, 12.979662f); //22.265266dB
		_LowBandOutputCurve.SetValue(7, 12.924708f); //22.228415dB
		_LowBandOutputCurve.SetValue(8, 12.871069f); //22.192291dB
		_LowBandOutputCurve.SetValue(9, 12.818679f); //22.156866dB
		_LowBandOutputCurve.SetValue(10, 12.767533f); //22.122139dB
		_LowBandOutputCurve.SetValue(11, 12.717639f); //22.088129dB
		_LowBandOutputCurve.SetValue(12, 12.668996f); //22.054844dB
		_LowBandOutputCurve.SetValue(13, 12.621537f); //22.022243dB
		_LowBandOutputCurve.SetValue(14, 12.575285f); //21.990356dB
		_LowBandOutputCurve.SetValue(15, 12.530186f); //21.959148dB
		_LowBandOutputCurve.SetValue(16, 12.486265f); //21.928650dB
		_LowBandOutputCurve.SetValue(17, 12.443456f); //21.898821dB
		_LowBandOutputCurve.SetValue(18, 12.401767f); //21.869671dB
		_LowBandOutputCurve.SetValue(19, 12.361218f); //21.841225dB
		_LowBandOutputCurve.SetValue(20, 12.321729f); //21.813433dB
		_LowBandOutputCurve.SetValue(21, 12.283310f); //21.786308dB
		_LowBandOutputCurve.SetValue(22, 12.245942f); //21.759844dB
		_LowBandOutputCurve.SetValue(23, 12.209605f); //21.734032dB
		_LowBandOutputCurve.SetValue(24, 12.174269f); //21.708858dB
		_LowBandOutputCurve.SetValue(25, 12.139938f); //21.684330dB
		_LowBandOutputCurve.SetValue(26, 12.106578f); //21.660427dB
		_LowBandOutputCurve.SetValue(27, 12.074171f); //21.637146dB
		_LowBandOutputCurve.SetValue(28, 12.042730f); //21.614498dB
		_LowBandOutputCurve.SetValue(29, 12.012202f); //21.592453dB
		_LowBandOutputCurve.SetValue(30, 11.982571f); //21.570999dB
		_LowBandOutputCurve.SetValue(31, 11.953861f); //21.550163dB
		_LowBandOutputCurve.SetValue(32, 11.926017f); //21.529907dB
		_LowBandOutputCurve.SetValue(33, 11.899027f); //21.510229dB
		_LowBandOutputCurve.SetValue(34, 11.872910f); //21.491142dB
		_LowBandOutputCurve.SetValue(35, 11.847621f); //21.472624dB
		_LowBandOutputCurve.SetValue(36, 11.823169f); //21.454678dB
		_LowBandOutputCurve.SetValue(37, 11.799520f); //21.437286dB
		_LowBandOutputCurve.SetValue(38, 11.776667f); //21.420448dB
		_LowBandOutputCurve.SetValue(39, 11.754602f); //21.404160dB
		_LowBandOutputCurve.SetValue(40, 11.733315f); //21.388412dB
		_LowBandOutputCurve.SetValue(41, 11.712784f); //21.373203dB
		_LowBandOutputCurve.SetValue(42, 11.693019f); //21.358532dB
		_LowBandOutputCurve.SetValue(43, 11.673998f); //21.344391dB
		_LowBandOutputCurve.SetValue(44, 11.655712f); //21.330776dB
		_LowBandOutputCurve.SetValue(45, 11.638148f); //21.317677dB
		_LowBandOutputCurve.SetValue(46, 11.621334f); //21.305120dB
		_LowBandOutputCurve.SetValue(47, 11.605217f); //21.293064dB
		_LowBandOutputCurve.SetValue(48, 11.589797f); //21.281517dB
		_LowBandOutputCurve.SetValue(49, 11.575070f); //21.270473dB
		_LowBandOutputCurve.SetValue(50, 11.561042f); //21.259939dB
		_LowBandOutputCurve.SetValue(51, 11.547677f); //21.249893dB
		_LowBandOutputCurve.SetValue(52, 11.535008f); //21.240358dB
		_LowBandOutputCurve.SetValue(53, 11.523002f); //21.231312dB
		_LowBandOutputCurve.SetValue(54, 11.511675f); //21.222771dB
		_LowBandOutputCurve.SetValue(55, 11.500983f); //21.214699dB
		_LowBandOutputCurve.SetValue(56, 11.490952f); //21.207121dB
		_LowBandOutputCurve.SetValue(57, 11.481572f); //21.200027dB
		_LowBandOutputCurve.SetValue(58, 11.472815f); //21.193399dB
		_LowBandOutputCurve.SetValue(59, 11.464724f); //21.187271dB
		_LowBandOutputCurve.SetValue(60, 11.457272f); //21.181623dB
		_LowBandOutputCurve.SetValue(61, 11.450424f); //21.176432dB
		_LowBandOutputCurve.SetValue(62, 11.444215f); //21.171719dB
		_LowBandOutputCurve.SetValue(63, 11.438619f); //21.167471dB
		_LowBandOutputCurve.SetValue(64, 11.433634f); //21.163685dB
		_LowBandOutputCurve.SetValue(65, 11.429284f); //21.160381dB
		_LowBandOutputCurve.SetValue(66, 11.425508f); //21.157509dB
		_LowBandOutputCurve.SetValue(67, 11.422357f); //21.155113dB
		_LowBandOutputCurve.SetValue(68, 11.419789f); //21.153162dB
		_LowBandOutputCurve.SetValue(69, 11.417811f); //21.151657dB
		_LowBandOutputCurve.SetValue(70, 11.416408f); //21.150589dB
		_LowBandOutputCurve.SetValue(71, 11.415596f); //21.149971dB
		_LowBandOutputCurve.SetValue(72, 11.415339f); //21.149776dB
		_LowBandOutputCurve.SetValue(73, 11.415633f); //21.150000dB
		_LowBandOutputCurve.SetValue(74, 11.416530f); //21.150682dB
		_LowBandOutputCurve.SetValue(75, 11.417958f); //21.151768dB
		_LowBandOutputCurve.SetValue(76, 11.419946f); //21.153280dB
		_LowBandOutputCurve.SetValue(77, 11.422457f); //21.155190dB
		_LowBandOutputCurve.SetValue(78, 11.425508f); //21.157511dB
		_LowBandOutputCurve.SetValue(79, 11.429084f); //21.160229dB
		_LowBandOutputCurve.SetValue(80, 11.433182f); //21.163342dB
		_LowBandOutputCurve.SetValue(81, 11.437771f); //21.166828dB
		_LowBandOutputCurve.SetValue(82, 11.442867f); //21.170696dB
		_LowBandOutputCurve.SetValue(83, 11.448462f); //21.174942dB
		_LowBandOutputCurve.SetValue(84, 11.454529f); //21.179544dB
		_LowBandOutputCurve.SetValue(85, 11.461071f); //21.184504dB
		_LowBandOutputCurve.SetValue(86, 11.468066f); //21.189804dB
		_LowBandOutputCurve.SetValue(87, 11.475520f); //21.195448dB
		_LowBandOutputCurve.SetValue(88, 11.483436f); //21.201435dB
		_LowBandOutputCurve.SetValue(89, 11.491747f); //21.207722dB
		_LowBandOutputCurve.SetValue(90, 11.500504f); //21.214336dB
		_LowBandOutputCurve.SetValue(91, 11.509644f); //21.221237dB
		_LowBandOutputCurve.SetValue(92, 11.519210f); //21.228455dB
		_LowBandOutputCurve.SetValue(93, 11.529143f); //21.235941dB
		_LowBandOutputCurve.SetValue(94, 11.539452f); //21.243702dB
		_LowBandOutputCurve.SetValue(95, 11.550157f); //21.251757dB
		_LowBandOutputCurve.SetValue(96, 11.561188f); //21.260048dB
		_LowBandOutputCurve.SetValue(97, 11.572574f); //21.268599dB
		_LowBandOutputCurve.SetValue(98, 11.584270f); //21.277372dB
		_LowBandOutputCurve.SetValue(99, 11.596301f); //21.286388dB
		_LowBandOutputCurve.SetValue(100, 11.608636f); //21.295624dB
		_MidBandOutputCurve.SetValue(0, 26.674217f); //28.521833dB
		_MidBandOutputCurve.SetValue(1, 25.227797f); //28.037586dB
		_MidBandOutputCurve.SetValue(2, 23.936722f); //27.581293dB
		_MidBandOutputCurve.SetValue(3, 22.791502f); //27.155458dB
		_MidBandOutputCurve.SetValue(4, 21.781345f); //26.761694dB
		_MidBandOutputCurve.SetValue(5, 20.894226f); //26.400526dB
		_MidBandOutputCurve.SetValue(6, 20.117035f); //26.071278dB
		_MidBandOutputCurve.SetValue(7, 19.436197f); //25.772226dB
		_MidBandOutputCurve.SetValue(8, 18.838259f); //25.500814dB
		_MidBandOutputCurve.SetValue(9, 18.310474f); //25.253992dB
		_MidBandOutputCurve.SetValue(10, 17.841524f); //25.028639dB
		_MidBandOutputCurve.SetValue(11, 17.422216f); //24.822067dB
		_MidBandOutputCurve.SetValue(12, 17.045971f); //24.632435dB
		_MidBandOutputCurve.SetValue(13, 16.708794f); //24.458900dB
		_MidBandOutputCurve.SetValue(14, 16.408722f); //24.301495dB
		_MidBandOutputCurve.SetValue(15, 16.145184f); //24.160860dB
		_MidBandOutputCurve.SetValue(16, 15.918120f); //24.037836dB
		_MidBandOutputCurve.SetValue(17, 15.727569f); //23.933231dB
		_MidBandOutputCurve.SetValue(18, 15.572989f); //23.847439dB
		_MidBandOutputCurve.SetValue(19, 15.453092f); //23.780308dB
		_MidBandOutputCurve.SetValue(20, 15.365757f); //23.731079dB
		_MidBandOutputCurve.SetValue(21, 15.308565f); //23.698690dB
		_MidBandOutputCurve.SetValue(22, 15.279150f); //23.681984dB
		_MidBandOutputCurve.SetValue(23, 15.275502f); //23.679909dB
		_MidBandOutputCurve.SetValue(24, 15.296103f); //23.691616dB
		_MidBandOutputCurve.SetValue(25, 15.339764f); //23.716373dB
		_MidBandOutputCurve.SetValue(26, 15.405708f); //23.753633dB
		_MidBandOutputCurve.SetValue(27, 15.493249f); //23.802849dB
		_MidBandOutputCurve.SetValue(28, 15.601843f); //23.863518dB
		_MidBandOutputCurve.SetValue(29, 15.730850f); //23.935043dB
		_MidBandOutputCurve.SetValue(30, 15.879437f); //24.016703dB
		_MidBandOutputCurve.SetValue(31, 16.046728f); //24.107731dB
		_MidBandOutputCurve.SetValue(32, 16.231878f); //24.207375dB
		_MidBandOutputCurve.SetValue(33, 16.433992f); //24.314861dB
		_MidBandOutputCurve.SetValue(34, 16.652134f); //24.429398dB
		_MidBandOutputCurve.SetValue(35, 16.885347f); //24.550200dB
		_MidBandOutputCurve.SetValue(36, 17.132639f); //24.676485dB
		_MidBandOutputCurve.SetValue(37, 17.392927f); //24.807453dB
		_MidBandOutputCurve.SetValue(38, 17.665060f); //24.942303dB
		_MidBandOutputCurve.SetValue(39, 17.947832f); //25.080240dB
		_MidBandOutputCurve.SetValue(40, 18.240114f); //25.220551dB
		_MidBandOutputCurve.SetValue(41, 18.540751f); //25.362547dB
		_MidBandOutputCurve.SetValue(42, 18.848675f); //25.505615dB
		_MidBandOutputCurve.SetValue(43, 19.162766f); //25.649162dB
		_MidBandOutputCurve.SetValue(44, 19.481865f); //25.792610dB
		_MidBandOutputCurve.SetValue(45, 19.804848f); //25.935431dB
		_MidBandOutputCurve.SetValue(46, 20.130589f); //26.077129dB
		_MidBandOutputCurve.SetValue(47, 20.458054f); //26.217287dB
		_MidBandOutputCurve.SetValue(48, 20.786276f); //26.355534dB
		_MidBandOutputCurve.SetValue(49, 21.114307f); //26.491537dB
		_MidBandOutputCurve.SetValue(50, 21.441273f); //26.625011dB
		_MidBandOutputCurve.SetValue(51, 21.766308f); //26.755695dB
		_MidBandOutputCurve.SetValue(52, 22.088625f); //26.883373dB
		_MidBandOutputCurve.SetValue(53, 22.407402f); //27.007830dB
		_MidBandOutputCurve.SetValue(54, 22.722004f); //27.128933dB
		_MidBandOutputCurve.SetValue(55, 23.031765f); //27.246544dB
		_MidBandOutputCurve.SetValue(56, 23.336151f); //27.360584dB
		_MidBandOutputCurve.SetValue(57, 23.634686f); //27.470997dB
		_MidBandOutputCurve.SetValue(58, 23.926960f); //27.577751dB
		_MidBandOutputCurve.SetValue(59, 24.212566f); //27.680817dB
		_MidBandOutputCurve.SetValue(60, 24.491173f); //27.780191dB
		_MidBandOutputCurve.SetValue(61, 24.762468f); //27.875879dB
		_MidBandOutputCurve.SetValue(62, 25.026175f); //27.967890dB
		_MidBandOutputCurve.SetValue(63, 25.282082f); //28.056255dB
		_MidBandOutputCurve.SetValue(64, 25.529953f); //28.141001dB
		_MidBandOutputCurve.SetValue(65, 25.769655f); //28.222172dB
		_MidBandOutputCurve.SetValue(66, 26.001129f); //28.299843dB
		_MidBandOutputCurve.SetValue(67, 26.224308f); //28.374081dB
		_MidBandOutputCurve.SetValue(68, 26.439224f); //28.444975dB
		_MidBandOutputCurve.SetValue(69, 26.645903f); //28.512609dB
		_MidBandOutputCurve.SetValue(70, 26.844469f); //28.577095dB
		_MidBandOutputCurve.SetValue(71, 27.035000f); //28.638527dB
		_MidBandOutputCurve.SetValue(72, 27.217617f); //28.697002dB
		_MidBandOutputCurve.SetValue(73, 27.392443f); //28.752615dB
		_MidBandOutputCurve.SetValue(74, 27.559631f); //28.805468dB
		_MidBandOutputCurve.SetValue(75, 27.719324f); //28.855652dB
		_MidBandOutputCurve.SetValue(76, 27.871634f); //28.903248dB
		_MidBandOutputCurve.SetValue(77, 28.016680f); //28.948334dB
		_MidBandOutputCurve.SetValue(78, 28.154621f); //28.990993dB
		_MidBandOutputCurve.SetValue(79, 28.285553f); //29.031292dB
		_MidBandOutputCurve.SetValue(80, 28.409599f); //29.069302dB
		_MidBandOutputCurve.SetValue(81, 28.526875f); //29.105083dB
		_MidBandOutputCurve.SetValue(82, 28.637480f); //29.138697dB
		_MidBandOutputCurve.SetValue(83, 28.741533f); //29.170198dB
		_MidBandOutputCurve.SetValue(84, 28.839106f); //29.199636dB
		_MidBandOutputCurve.SetValue(85, 28.930317f); //29.227064dB
		_MidBandOutputCurve.SetValue(86, 29.015219f); //29.252518dB
		_MidBandOutputCurve.SetValue(87, 29.093943f); //29.276051dB
		_MidBandOutputCurve.SetValue(88, 29.166498f); //29.297686dB
		_MidBandOutputCurve.SetValue(89, 29.233015f); //29.317471dB
		_MidBandOutputCurve.SetValue(90, 29.293522f); //29.335432dB
		_MidBandOutputCurve.SetValue(91, 29.348082f); //29.351595dB
		_MidBandOutputCurve.SetValue(92, 29.396788f); //29.365997dB
		_MidBandOutputCurve.SetValue(93, 29.439617f); //29.378643dB
		_MidBandOutputCurve.SetValue(94, 29.476683f); //29.389572dB
		_MidBandOutputCurve.SetValue(95, 29.507978f); //29.398788dB
		_MidBandOutputCurve.SetValue(96, 29.533545f); //29.406311dB
		_MidBandOutputCurve.SetValue(97, 29.553389f); //29.412146dB
		_MidBandOutputCurve.SetValue(98, 29.567547f); //29.416306dB
		_MidBandOutputCurve.SetValue(99, 29.576054f); //29.418804dB
		_MidBandOutputCurve.SetValue(100, 29.578871f); //29.419632dB
		_HighBandOutputCurve.SetValue(0, 51.876534f); //34.299419dB
		_HighBandOutputCurve.SetValue(1, 49.611599f); //33.911663dB
		_HighBandOutputCurve.SetValue(2, 47.712234f); //33.572594dB
		_HighBandOutputCurve.SetValue(3, 46.071705f); //33.268684dB
		_HighBandOutputCurve.SetValue(4, 44.619328f); //32.990459dB
		_HighBandOutputCurve.SetValue(5, 43.317303f); //32.733227dB
		_HighBandOutputCurve.SetValue(6, 42.151356f); //32.496231dB
		_HighBandOutputCurve.SetValue(7, 41.118603f); //32.280766dB
		_HighBandOutputCurve.SetValue(8, 40.215954f); //32.087967dB
		_HighBandOutputCurve.SetValue(9, 39.434490f); //31.917524dB
		_HighBandOutputCurve.SetValue(10, 38.758575f); //31.767357dB
		_HighBandOutputCurve.SetValue(11, 38.162739f); //31.632790dB
		_HighBandOutputCurve.SetValue(12, 37.611515f); //31.506416dB
		_HighBandOutputCurve.SetValue(13, 37.073498f); //31.381271dB
		_HighBandOutputCurve.SetValue(14, 36.534096f); //31.253967dB
		_HighBandOutputCurve.SetValue(15, 35.994579f); //31.124741dB
		_HighBandOutputCurve.SetValue(16, 35.463528f); //30.995638dB
		_HighBandOutputCurve.SetValue(17, 34.950974f); //30.869186dB
		_HighBandOutputCurve.SetValue(18, 34.465599f); //30.747717dB
		_HighBandOutputCurve.SetValue(19, 34.013775f); //30.633097dB
		_HighBandOutputCurve.SetValue(20, 33.598858f); //30.526489dB
		_HighBandOutputCurve.SetValue(21, 33.221348f); //30.428345dB
		_HighBandOutputCurve.SetValue(22, 32.879250f); //30.338438dB
		_HighBandOutputCurve.SetValue(23, 32.568684f); //30.256004dB
		_HighBandOutputCurve.SetValue(24, 32.285149f); //30.180056dB
		_HighBandOutputCurve.SetValue(25, 32.024822f); //30.109734dB
		_HighBandOutputCurve.SetValue(26, 31.785311f); //30.044529dB
		_HighBandOutputCurve.SetValue(27, 31.564924f); //29.984095dB
		_HighBandOutputCurve.SetValue(28, 31.361937f); //29.928057dB
		_HighBandOutputCurve.SetValue(29, 31.174086f); //29.875874dB
		_HighBandOutputCurve.SetValue(30, 30.999689f); //29.827147dB
		_HighBandOutputCurve.SetValue(31, 30.838631f); //29.781902dB
		_HighBandOutputCurve.SetValue(32, 30.691608f); //29.740393dB
		_HighBandOutputCurve.SetValue(33, 30.559319f); //29.702873dB
		_HighBandOutputCurve.SetValue(34, 30.442009f); //29.669466dB
		_HighBandOutputCurve.SetValue(35, 30.339911f); //29.640285dB
		_HighBandOutputCurve.SetValue(36, 30.253241f); //29.615438dB
		_HighBandOutputCurve.SetValue(37, 30.182016f); //29.594965dB
		_HighBandOutputCurve.SetValue(38, 30.125990f); //29.578827dB
		_HighBandOutputCurve.SetValue(39, 30.084681f); //29.566908dB
		_HighBandOutputCurve.SetValue(40, 30.057531f); //29.559067dB
		_HighBandOutputCurve.SetValue(41, 30.044086f); //29.555180dB
		_HighBandOutputCurve.SetValue(42, 30.044104f); //29.555185dB
		_HighBandOutputCurve.SetValue(43, 30.057062f); //29.558929dB
		_HighBandOutputCurve.SetValue(44, 30.082327f); //29.566229dB
		_HighBandOutputCurve.SetValue(45, 30.118986f); //29.576807dB
		_HighBandOutputCurve.SetValue(46, 30.166073f); //29.590374dB
		_HighBandOutputCurve.SetValue(47, 30.222788f); //29.606689dB
		_HighBandOutputCurve.SetValue(48, 30.288687f); //29.625608dB
		_HighBandOutputCurve.SetValue(49, 30.363272f); //29.646971dB
		_HighBandOutputCurve.SetValue(50, 30.446379f); //29.670713dB
		_HighBandOutputCurve.SetValue(51, 30.537659f); //29.696714dB
		_HighBandOutputCurve.SetValue(52, 30.636782f); //29.724861dB
		_HighBandOutputCurve.SetValue(53, 30.743092f); //29.754951dB
		_HighBandOutputCurve.SetValue(54, 30.855886f); //29.786760dB
		_HighBandOutputCurve.SetValue(55, 30.974222f); //29.820007dB
		_HighBandOutputCurve.SetValue(56, 31.097151f); //29.854412dB
		_HighBandOutputCurve.SetValue(57, 31.223644f); //29.889671dB
		_HighBandOutputCurve.SetValue(58, 31.352726f); //29.925507dB
		_HighBandOutputCurve.SetValue(59, 31.483595f); //29.961685dB
		_HighBandOutputCurve.SetValue(60, 31.615547f); //29.998013dB
		_HighBandOutputCurve.SetValue(61, 31.748009f); //30.034328dB
		_HighBandOutputCurve.SetValue(62, 31.880562f); //30.070518dB
		_HighBandOutputCurve.SetValue(63, 32.012844f); //30.106485dB
		_HighBandOutputCurve.SetValue(64, 32.144508f); //30.142136dB
		_HighBandOutputCurve.SetValue(65, 32.275253f); //30.177393dB
		_HighBandOutputCurve.SetValue(66, 32.404766f); //30.212177dB
		_HighBandOutputCurve.SetValue(67, 32.532795f); //30.246428dB
		_HighBandOutputCurve.SetValue(68, 32.659019f); //30.280064dB
		_HighBandOutputCurve.SetValue(69, 32.783192f); //30.313025dB
		_HighBandOutputCurve.SetValue(70, 32.905060f); //30.345253dB
		_HighBandOutputCurve.SetValue(71, 33.024410f); //30.376701dB
		_HighBandOutputCurve.SetValue(72, 33.140972f); //30.407305dB
		_HighBandOutputCurve.SetValue(73, 33.254593f); //30.437033dB
		_HighBandOutputCurve.SetValue(74, 33.365063f); //30.465839dB
		_HighBandOutputCurve.SetValue(75, 33.472260f); //30.493700dB
		_HighBandOutputCurve.SetValue(76, 33.575993f); //30.520576dB
		_HighBandOutputCurve.SetValue(77, 33.676140f); //30.546446dB
		_HighBandOutputCurve.SetValue(78, 33.772560f); //30.571280dB
		_HighBandOutputCurve.SetValue(79, 33.865192f); //30.595072dB
		_HighBandOutputCurve.SetValue(80, 33.953896f); //30.617792dB
		_HighBandOutputCurve.SetValue(81, 34.038609f); //30.639437dB
		_HighBandOutputCurve.SetValue(82, 34.119263f); //30.659992dB
		_HighBandOutputCurve.SetValue(83, 34.195789f); //30.679453dB
		_HighBandOutputCurve.SetValue(84, 34.268112f); //30.697803dB
		_HighBandOutputCurve.SetValue(85, 34.336243f); //30.715055dB
		_HighBandOutputCurve.SetValue(86, 34.400089f); //30.731192dB
		_HighBandOutputCurve.SetValue(87, 34.459667f); //30.746222dB
		_HighBandOutputCurve.SetValue(88, 34.514854f); //30.760120dB
		_HighBandOutputCurve.SetValue(89, 34.565750f); //30.772919dB
		_HighBandOutputCurve.SetValue(90, 34.612244f); //30.784595dB
		_HighBandOutputCurve.SetValue(91, 34.654377f); //30.795162dB
		_HighBandOutputCurve.SetValue(92, 34.692104f); //30.804613dB
		_HighBandOutputCurve.SetValue(93, 34.725391f); //30.812943dB
		_HighBandOutputCurve.SetValue(94, 34.754284f); //30.820168dB
		_HighBandOutputCurve.SetValue(95, 34.778713f); //30.826271dB
		_HighBandOutputCurve.SetValue(96, 34.798748f); //30.831272dB
		_HighBandOutputCurve.SetValue(97, 34.814331f); //30.835161dB
		_HighBandOutputCurve.SetValue(98, 34.825447f); //30.837933dB
		_HighBandOutputCurve.SetValue(99, 34.832138f); //30.839603dB
		_HighBandOutputCurve.SetValue(100, 34.834366f); //30.840158dB
	}

	/*
	*	Callback for when the sample rate changed.
	*/
	FORCE_INLINE void OnSampleRateChanged() NOEXCEPT override
	{
		
	}

	/*
	*	Callback for this audio effect to process the given buffer.
	*/
	FORCE_INLINE void Process
	(
		const AudioProcessContext &context,
		const DynamicArray<DynamicArray<float32>> &inputs,
		DynamicArray<DynamicArray<float32>> *const RESTRICT outputs,
		const uint8 number_of_channels,
		const uint32 number_of_samples
	) NOEXCEPT override
	{
		//If no band is enabled, just copy the inputs into the outputs and move on.
		if (!_LowBandEnabled && !_MidBandEnabled && !_HighBandEnabled)
		{
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				Memory::Copy(outputs->At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
			}

			return;
		}

		//Copy the inputs into the input buffer.
		_InputBuffer.Resize<true>(number_of_channels);

		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			_InputBuffer.At(channel_index).Resize<false>(number_of_samples);
			Memory::Copy(_InputBuffer.At(channel_index).Data(), inputs.At(channel_index).Data(), sizeof(float32) * number_of_samples);
		}

		//Set up the working buffer.
		_WorkingBuffer.Resize<true>(number_of_channels);

		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			_WorkingBuffer.At(channel_index).Resize<false>(number_of_samples);
		}

		//Set the outputs to zero, as we will continually add to it.
		for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
		{
			Memory::Set(outputs->At(channel_index).Data(), 0, sizeof(float32) * number_of_samples);
		}

		//Process the low band.
		if (_LowBandEnabled)
		{
			//Apply the pre low pass filter.
			_LowBand._PreLowPassFilter.Process(context, _InputBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			/*
			*	Apply compression.
			*	Since we're low-passing pretty aggressively here, we can get away with just doing a simple saturation.
			*/
			{
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(16.0f, 32.0f, _LowBandCompression)) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
					{
						_WorkingBuffer.At(channel_index).At(sample_index) = std::tanh(_WorkingBuffer.At(channel_index).At(sample_index) * gain);
					}
				}
			}

			//Apply the post low pass filter.
			_LowBand._PostLowPassFilter._Frequency = _LowBandLowPassFrequency;
			_LowBand._PostLowPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply the gain.
			{
				const float32 output_gain{ 0.125f * 0.25f * _LowBandGain * _LowBandOutputCurve.Sample(_LowBandCompression) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					SIMD::MultiplyByScalar(_WorkingBuffer.At(channel_index).Data(), number_of_samples, output_gain);
				}
			}

			//Add to the output.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				SIMD::Add(outputs->At(channel_index).Data(), _WorkingBuffer.At(channel_index).Data(), number_of_samples);
			}
		}

		//Process the mid band.
		if (_MidBandEnabled)
		{
			//Apply the filters.
			_MidBand._PreHighPassFilter1.Process(context, _InputBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_MidBand._PreHighPassFilter2.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_MidBand._PreHighPassFilter3.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_MidBand._PrePeakFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_MidBand._PreLowPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply distortion.
			{
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(50.0f, 100.0f, BaseMath::InverseSquare(_MidBandDistortion))) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
					{
						_MidBand._OverSamplers[channel_index].Feed(_WorkingBuffer.At(channel_index).At(sample_index));

						for (float32 &sample : _MidBand._OverSamplers[channel_index].GetSamples())
						{
							const float32 input_sample{ sample * gain };
							const float32 distortion_1{ std::atan(input_sample * (sample >= 0.0f ? 2.0f : 0.5f)) };
							const float32 distortion_2{ std::tanh(input_sample * (sample >= 0.0f ? 0.5f : 2.0f)) };
							float32 interpolator{ BaseMath::Absolute<float32>(std::tanh(input_sample * 0.1f * (sample >= 0.0f ? 2.0f : 0.5f))) };

							for (uint8 power_of_iterator{ 0 }; power_of_iterator < 8; ++power_of_iterator) interpolator *= interpolator;

							sample = BaseMath::LinearlyInterpolate(distortion_1, distortion_2, interpolator);
						}

						_WorkingBuffer.At(channel_index).At(sample_index) = _MidBand._OverSamplers[channel_index].DownSample();
					}
				}
			}

			//Apply the post high pass filter.
			_MidBand._PostHighPassFilter._Frequency = _MidBandHighPassFrequency;
			_MidBand._PostHighPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply the post low pass filter.
			_MidBand._PostLowPassFilter._Frequency = _MidBandLowPassFrequency;
			_MidBand._PostLowPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply the gain.
			{
				const float32 output_gain{ 0.125f * 0.125f * _MidBandGain * _MidBandOutputCurve.Sample(_MidBandDistortion) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					SIMD::MultiplyByScalar(_WorkingBuffer.At(channel_index).Data(), number_of_samples, output_gain);
				}
			}

			//Add to the output.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				SIMD::Add(outputs->At(channel_index).Data(), _WorkingBuffer.At(channel_index).Data(), number_of_samples);
			}
		}

		//Process the high band.
		if (_HighBandEnabled)
		{
			//Apply the filters.
			_HighBand._HighPassFilter1.Process(context, _InputBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_HighBand._HighPassFilter2.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_HighBand._HighPassFilter3.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);
			_HighBand._PeakFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply distortion.
			{
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(50.0f, 100.0f, BaseMath::InverseSquare(_HighBandDistortion))) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
					{
						_HighBand._OverSamplers[channel_index].Feed(_WorkingBuffer.At(channel_index).At(sample_index));

						for (float32 &sample : _HighBand._OverSamplers[channel_index].GetSamples())
						{
							const float32 input_sample{ sample * gain };
							const float32 distortion_1{ std::atan(input_sample * (sample >= 0.0f ? 0.25f : 4.0f)) };
							const float32 distortion_2{ std::tanh(input_sample * (sample >= 0.0f ? 0.25f : 4.0f)) };
							float32 interpolator{ BaseMath::Absolute<float32>(std::tanh(input_sample * 0.1f * (sample >= 0.0f ? 0.25f : 4.0f))) };

							for (uint8 power_of_iterator{ 0 }; power_of_iterator < 8; ++power_of_iterator) interpolator *= interpolator;

							sample = BaseMath::LinearlyInterpolate(distortion_1, distortion_2, interpolator);
						}

						_WorkingBuffer.At(channel_index).At(sample_index) = _HighBand._OverSamplers[channel_index].DownSample();
					}
				}
			}

			//Apply the post high and low pass filter.
			_HighBand._PostHighPassFilter._Frequency = _HighBandHighPassFrequency;
			_HighBand._PostHighPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			_HighBand._PostLowPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply the gain.
			{
				const float32 output_gain{ 0.125f * 0.125f * 0.5f * _HighBandGain * _HighBandOutputCurve.Sample(_HighBandDistortion) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					SIMD::MultiplyByScalar(_WorkingBuffer.At(channel_index).Data(), number_of_samples, output_gain);
				}
			}

			//Add to the output.
			for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
			{
				SIMD::Add(outputs->At(channel_index).Data(), _WorkingBuffer.At(channel_index).Data(), number_of_samples);
			}
		}

		//Apply the post filters.
		_PostFilters._PeakFilter1.Process(context, (*outputs), outputs, number_of_channels, number_of_samples);
		_PostFilters._PeakFilter2.Process(context, (*outputs), outputs, number_of_channels, number_of_samples);
		_PostFilters._PeakFilter3.Process(context, (*outputs), outputs, number_of_channels, number_of_samples);
		_PostFilters._PeakFilter4.Process(context, (*outputs), outputs, number_of_channels, number_of_samples);
		_PostFilters._HighShelfFilter.Process(context, (*outputs), outputs, number_of_channels, number_of_samples);
	}

private:

	/*
	*	Low band class definition.
	*/
	class LowBand final
	{

	public:

		//The pre low pass filter.
		LowPassFilter _PreLowPassFilter{ LOW_BAND_LOW_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

		//The post low pass filter.
		LowPassFilter _PostLowPassFilter{ LOW_BAND_LOW_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

	};

	/*
	*	Mid band class definition.
	*/
	class MidBand final
	{

	public:

		//Pre high pass filter 1.
		HighPassFilter _PreHighPassFilter1{ 125.0f, 1.0f, 3 };

		//Pre high pass filter 2.
		HighPassFilter _PreHighPassFilter2{ 275.0f, 1.0f, 2 };

		//Pre high pass filter 3.
		HighPassFilter _PreHighPassFilter3{ 575.0f, 1.0f, 1 };

		//The pre peak filter.
		PeakFilter _PrePeakFilter{ 400.0f, Audio::DecibelsToGain(-18.0f), 1.0f };

		//The pre low pass filter.
		LowPassFilter _PreLowPassFilter{ 4'000.0f, 1.0f, 1 };

		//The oversamplers.
		StaticArray<OverSampler<4>, 2> _OverSamplers;

		//The post high pass filter.
		HighPassFilter _PostHighPassFilter{ MID_BAND_HIGH_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

		//The post low pass filter.
		LowPassFilter _PostLowPassFilter{ MID_BAND_LOW_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

	};

	/*
	*	High band class definition.
	*/
	class HighBand final
	{

	public:

		//High pass filter 1.
		HighPassFilter _HighPassFilter1{ 175.0f, 1.0f, 3 };

		//High pass filter 2.
		HighPassFilter _HighPassFilter2{ 375.0f, 1.0f, 2 };

		//High pass filter 3.
		HighPassFilter _HighPassFilter3{ 775.0f, 1.0f, 1 };

		//The peak filter.
		PeakFilter _PeakFilter{ 400.0f, Audio::DecibelsToGain(-12.0f), 1.0f };

		//The oversamplers.
		StaticArray<OverSampler<4>, 2> _OverSamplers;

		//The post high pass filter.
		HighPassFilter _PostHighPassFilter{ HIGH_BAND_HIGH_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

		//The post low pass filter.
		LowPassFilter _PostLowPassFilter{ 10'000.0f, 1.0f, 2 };

	};

	/*
	*	Post filters class definition.
	*/
	class PostFilters final
	{

	public:

		//Peak filter 1.
		PeakFilter _PeakFilter1{ 250.0f, Audio::DecibelsToGain(-3.0f), 1.5f };

		//Peak filter 2.
		PeakFilter _PeakFilter2{ 400.0f, Audio::DecibelsToGain(-6.0f), 1.0f };

		//Peak filter 3.
		PeakFilter _PeakFilter3{ 800.0f, Audio::DecibelsToGain(-1.5f), 1.0f };

		//Peak filter 4.
		PeakFilter _PeakFilter4{ 4'500.0f, Audio::DecibelsToGain(-1.5f), 1.5f };

		//The high shelf filter.
		HighShelfFilter _HighShelfFilter{ 10'000.0f, Audio::DecibelsToGain(3.0f) };

	};

	//The input buffer.
	DynamicArray<DynamicArray<float32>> _InputBuffer;

	//The working buffer.
	DynamicArray<DynamicArray<float32>> _WorkingBuffer;

	//The low band.
	LowBand _LowBand;

	//The low band output curve.
	Curve<float32, 101> _LowBandOutputCurve;

	//The mid band.
	MidBand _MidBand;

	//The mid band output curve.
	Curve<float32, 101> _MidBandOutputCurve;

	//The high band.
	HighBand _HighBand;

	//The high band output curve.
	Curve<float32, 101> _HighBandOutputCurve;

	//The post filters.
	PostFilters _PostFilters;

};