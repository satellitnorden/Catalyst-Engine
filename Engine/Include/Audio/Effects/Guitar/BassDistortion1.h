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
	float32 _MidBandGain{ 1.0f };

	//Denotes whether or not the high band is enabled.
	bool _HighBandEnabled{ true };

	//The high band high pass frequency.
	float32 _HighBandHighPassFrequency{ HIGH_BAND_HIGH_PASS_MAXIMUM_FREQUENCY };

	//The high band distortion.
	float32 _HighBandDistortion{ 0.5f };

	//The high band gain.
	float32 _HighBandGain{ 1.0f };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE BassDistortion1() NOEXCEPT
	{
		//Set up the output curves.
		_LowBandOutputCurve.SetValue(0, 11.721323f); //21.379532dB
		_LowBandOutputCurve.SetValue(1, 11.512923f); //21.223711dB
		_LowBandOutputCurve.SetValue(2, 11.309107f); //21.068565dB
		_LowBandOutputCurve.SetValue(3, 11.109771f); //20.914101dB
		_LowBandOutputCurve.SetValue(4, 10.914804f); //20.760319dB
		_LowBandOutputCurve.SetValue(5, 10.724123f); //20.607235dB
		_LowBandOutputCurve.SetValue(6, 10.537634f); //20.454861dB
		_LowBandOutputCurve.SetValue(7, 10.355252f); //20.303213dB
		_LowBandOutputCurve.SetValue(8, 10.176860f); //20.152275dB
		_LowBandOutputCurve.SetValue(9, 10.002391f); //20.002077dB
		_LowBandOutputCurve.SetValue(10, 9.831762f); //19.852627dB
		_LowBandOutputCurve.SetValue(11, 9.664883f); //19.703932dB
		_LowBandOutputCurve.SetValue(12, 9.501640f); //19.555971dB
		_LowBandOutputCurve.SetValue(13, 9.342001f); //19.408798dB
		_LowBandOutputCurve.SetValue(14, 9.185849f); //19.262386dB
		_LowBandOutputCurve.SetValue(15, 9.033119f); //19.116755dB
		_LowBandOutputCurve.SetValue(16, 8.883727f); //18.971905dB
		_LowBandOutputCurve.SetValue(17, 8.737598f); //18.827841dB
		_LowBandOutputCurve.SetValue(18, 8.594672f); //18.684586dB
		_LowBandOutputCurve.SetValue(19, 8.454834f); //18.542101dB
		_LowBandOutputCurve.SetValue(20, 8.318059f); //18.400440dB
		_LowBandOutputCurve.SetValue(21, 8.184251f); //18.259579dB
		_LowBandOutputCurve.SetValue(22, 8.053350f); //18.119532dB
		_LowBandOutputCurve.SetValue(23, 7.925285f); //17.980297dB
		_LowBandOutputCurve.SetValue(24, 7.799992f); //17.841883dB
		_LowBandOutputCurve.SetValue(25, 7.677400f); //17.704283dB
		_LowBandOutputCurve.SetValue(26, 7.557462f); //17.567518dB
		_LowBandOutputCurve.SetValue(27, 7.440095f); //17.431570dB
		_LowBandOutputCurve.SetValue(28, 7.325251f); //17.296450dB
		_LowBandOutputCurve.SetValue(29, 7.212856f); //17.162146dB
		_LowBandOutputCurve.SetValue(30, 7.102886f); //17.028696dB
		_LowBandOutputCurve.SetValue(31, 6.995244f); //16.896057dB
		_LowBandOutputCurve.SetValue(32, 6.889888f); //16.764244dB
		_LowBandOutputCurve.SetValue(33, 6.786794f); //16.633293dB
		_LowBandOutputCurve.SetValue(34, 6.685865f); //16.503153dB
		_LowBandOutputCurve.SetValue(35, 6.587075f); //16.373852dB
		_LowBandOutputCurve.SetValue(36, 6.490359f); //16.245375dB
		_LowBandOutputCurve.SetValue(37, 6.395680f); //16.117735dB
		_LowBandOutputCurve.SetValue(38, 6.302988f); //15.990930dB
		_LowBandOutputCurve.SetValue(39, 6.212227f); //15.864946dB
		_LowBandOutputCurve.SetValue(40, 6.123356f); //15.739790dB
		_LowBandOutputCurve.SetValue(41, 6.036336f); //15.615468dB
		_LowBandOutputCurve.SetValue(42, 5.951116f); //15.491968dB
		_LowBandOutputCurve.SetValue(43, 5.867661f); //15.369301dB
		_LowBandOutputCurve.SetValue(44, 5.785920f); //15.247448dB
		_LowBandOutputCurve.SetValue(45, 5.705866f); //15.126431dB
		_LowBandOutputCurve.SetValue(46, 5.627437f); //15.006213dB
		_LowBandOutputCurve.SetValue(47, 5.550611f); //14.886817dB
		_LowBandOutputCurve.SetValue(48, 5.475352f); //14.768241dB
		_LowBandOutputCurve.SetValue(49, 5.401616f); //14.650474dB
		_LowBandOutputCurve.SetValue(50, 5.329370f); //14.533519dB
		_LowBandOutputCurve.SetValue(51, 5.258583f); //14.417374dB
		_LowBandOutputCurve.SetValue(52, 5.189218f); //14.302037dB
		_LowBandOutputCurve.SetValue(53, 5.121237f); //14.187498dB
		_LowBandOutputCurve.SetValue(54, 5.054612f); //14.073757dB
		_LowBandOutputCurve.SetValue(55, 4.989318f); //13.960824dB
		_LowBandOutputCurve.SetValue(56, 4.925316f); //13.848681dB
		_LowBandOutputCurve.SetValue(57, 4.862580f); //13.737335dB
		_LowBandOutputCurve.SetValue(58, 4.801083f); //13.626784dB
		_LowBandOutputCurve.SetValue(59, 4.740788f); //13.517010dB
		_LowBandOutputCurve.SetValue(60, 4.681678f); //13.408031dB
		_LowBandOutputCurve.SetValue(61, 4.623720f); //13.299831dB
		_LowBandOutputCurve.SetValue(62, 4.566892f); //13.192413dB
		_LowBandOutputCurve.SetValue(63, 4.511160f); //13.085764dB
		_LowBandOutputCurve.SetValue(64, 4.456507f); //12.979892dB
		_LowBandOutputCurve.SetValue(65, 4.402904f); //12.874784dB
		_LowBandOutputCurve.SetValue(66, 4.350336f); //12.770456dB
		_LowBandOutputCurve.SetValue(67, 4.298771f); //12.666887dB
		_LowBandOutputCurve.SetValue(68, 4.248194f); //12.564087dB
		_LowBandOutputCurve.SetValue(69, 4.198580f); //12.462049dB
		_LowBandOutputCurve.SetValue(70, 4.149910f); //12.360773dB
		_LowBandOutputCurve.SetValue(71, 4.102157f); //12.260246dB
		_LowBandOutputCurve.SetValue(72, 4.055309f); //12.160479dB
		_LowBandOutputCurve.SetValue(73, 4.009346f); //12.061471dB
		_LowBandOutputCurve.SetValue(74, 3.964236f); //11.963191dB
		_LowBandOutputCurve.SetValue(75, 3.919978f); //11.865672dB
		_LowBandOutputCurve.SetValue(76, 3.876541f); //11.768888dB
		_LowBandOutputCurve.SetValue(77, 3.833915f); //11.672850dB
		_LowBandOutputCurve.SetValue(78, 3.792079f); //11.577547dB
		_LowBandOutputCurve.SetValue(79, 3.751015f); //11.482976dB
		_LowBandOutputCurve.SetValue(80, 3.710711f); //11.389143dB
		_LowBandOutputCurve.SetValue(81, 3.671152f); //11.296048dB
		_LowBandOutputCurve.SetValue(82, 3.632317f); //11.203675dB
		_LowBandOutputCurve.SetValue(83, 3.594192f); //11.112026dB
		_LowBandOutputCurve.SetValue(84, 3.556767f); //11.021109dB
		_LowBandOutputCurve.SetValue(85, 3.520024f); //10.930912dB
		_LowBandOutputCurve.SetValue(86, 3.483949f); //10.841434dB
		_LowBandOutputCurve.SetValue(87, 3.448527f); //10.752671dB
		_LowBandOutputCurve.SetValue(88, 3.413744f); //10.664619dB
		_LowBandOutputCurve.SetValue(89, 3.379595f); //10.577293dB
		_LowBandOutputCurve.SetValue(90, 3.346059f); //10.490671dB
		_LowBandOutputCurve.SetValue(91, 3.313127f); //10.404761dB
		_LowBandOutputCurve.SetValue(92, 3.280783f); //10.319551dB
		_LowBandOutputCurve.SetValue(93, 3.249021f); //10.235051dB
		_LowBandOutputCurve.SetValue(94, 3.217828f); //10.151255dB
		_LowBandOutputCurve.SetValue(95, 3.187180f); //10.068131dB
		_LowBandOutputCurve.SetValue(96, 3.157088f); //9.985732dB
		_LowBandOutputCurve.SetValue(97, 3.127523f); //9.904011dB
		_LowBandOutputCurve.SetValue(98, 3.098486f); //9.822991dB
		_LowBandOutputCurve.SetValue(99, 3.069961f); //9.742656dB
		_LowBandOutputCurve.SetValue(100, 3.041940f); //9.663013dB
		_MidBandOutputCurve.SetValue(0, 77.220314f); //37.754631dB
		_MidBandOutputCurve.SetValue(1, 69.244522f); //36.807709dB
		_MidBandOutputCurve.SetValue(2, 62.302620f); //35.890125dB
		_MidBandOutputCurve.SetValue(3, 56.250835f); //35.002579dB
		_MidBandOutputCurve.SetValue(4, 50.965214f); //34.145477dB
		_MidBandOutputCurve.SetValue(5, 46.338970f); //33.318928dB
		_MidBandOutputCurve.SetValue(6, 42.280106f); //32.522720dB
		_MidBandOutputCurve.SetValue(7, 38.709255f); //31.756296dB
		_MidBandOutputCurve.SetValue(8, 35.558113f); //31.018774dB
		_MidBandOutputCurve.SetValue(9, 32.767773f); //30.308937dB
		_MidBandOutputCurve.SetValue(10, 30.287605f); //29.625299dB
		_MidBandOutputCurve.SetValue(11, 28.074308f); //28.966181dB
		_MidBandOutputCurve.SetValue(12, 26.090975f); //28.329805dB
		_MidBandOutputCurve.SetValue(13, 24.306471f); //27.714437dB
		_MidBandOutputCurve.SetValue(14, 22.694653f); //27.118469dB
		_MidBandOutputCurve.SetValue(15, 21.233711f); //26.540518dB
		_MidBandOutputCurve.SetValue(16, 19.905495f); //25.979458dB
		_MidBandOutputCurve.SetValue(17, 18.694822f); //25.434427dB
		_MidBandOutputCurve.SetValue(18, 17.589016f); //24.904829dB
		_MidBandOutputCurve.SetValue(19, 16.577263f); //24.390257dB
		_MidBandOutputCurve.SetValue(20, 15.650332f); //23.890471dB
		_MidBandOutputCurve.SetValue(21, 14.800098f); //23.405291dB
		_MidBandOutputCurve.SetValue(22, 14.019440f); //22.934612dB
		_MidBandOutputCurve.SetValue(23, 13.301987f); //22.478329dB
		_MidBandOutputCurve.SetValue(24, 12.641970f); //22.036295dB
		_MidBandOutputCurve.SetValue(25, 12.034195f); //21.608339dB
		_MidBandOutputCurve.SetValue(26, 11.473932f); //21.194246dB
		_MidBandOutputCurve.SetValue(27, 10.956907f); //20.793758dB
		_MidBandOutputCurve.SetValue(28, 10.479239f); //20.406593dB
		_MidBandOutputCurve.SetValue(29, 10.037439f); //20.032457dB
		_MidBandOutputCurve.SetValue(30, 9.628355f); //19.671041dB
		_MidBandOutputCurve.SetValue(31, 9.249163f); //19.322048dB
		_MidBandOutputCurve.SetValue(32, 8.897305f); //18.985168dB
		_MidBandOutputCurve.SetValue(33, 8.570490f); //18.660112dB
		_MidBandOutputCurve.SetValue(34, 8.266642f); //18.346582dB
		_MidBandOutputCurve.SetValue(35, 7.983897f); //18.044298dB
		_MidBandOutputCurve.SetValue(36, 7.720551f); //17.752966dB
		_MidBandOutputCurve.SetValue(37, 7.475059f); //17.472292dB
		_MidBandOutputCurve.SetValue(38, 7.246017f); //17.201986dB
		_MidBandOutputCurve.SetValue(39, 7.032153f); //16.941765dB
		_MidBandOutputCurve.SetValue(40, 6.832292f); //16.691328dB
		_MidBandOutputCurve.SetValue(41, 6.645375f); //16.450390dB
		_MidBandOutputCurve.SetValue(42, 6.470431f); //16.218664dB
		_MidBandOutputCurve.SetValue(43, 6.306573f); //15.995869dB
		_MidBandOutputCurve.SetValue(44, 6.152995f); //15.781732dB
		_MidBandOutputCurve.SetValue(45, 6.008962f); //15.575990dB
		_MidBandOutputCurve.SetValue(46, 5.873797f); //15.378379dB
		_MidBandOutputCurve.SetValue(47, 5.746885f); //15.188649dB
		_MidBandOutputCurve.SetValue(48, 5.627656f); //15.006550dB
		_MidBandOutputCurve.SetValue(49, 5.515595f); //14.831846dB
		_MidBandOutputCurve.SetValue(50, 5.410224f); //14.664305dB
		_MidBandOutputCurve.SetValue(51, 5.311105f); //14.503696dB
		_MidBandOutputCurve.SetValue(52, 5.217827f); //14.349794dB
		_MidBandOutputCurve.SetValue(53, 5.130022f); //14.202385dB
		_MidBandOutputCurve.SetValue(54, 5.047338f); //14.061247dB
		_MidBandOutputCurve.SetValue(55, 4.969455f); //13.926176dB
		_MidBandOutputCurve.SetValue(56, 4.896074f); //13.796959dB
		_MidBandOutputCurve.SetValue(57, 4.826915f); //13.673393dB
		_MidBandOutputCurve.SetValue(58, 4.761720f); //13.555278dB
		_MidBandOutputCurve.SetValue(59, 4.700248f); //13.442415dB
		_MidBandOutputCurve.SetValue(60, 4.642275f); //13.334618dB
		_MidBandOutputCurve.SetValue(61, 4.587592f); //13.231695dB
		_MidBandOutputCurve.SetValue(62, 4.536002f); //13.133465dB
		_MidBandOutputCurve.SetValue(63, 4.487325f); //13.039751dB
		_MidBandOutputCurve.SetValue(64, 4.441391f); //12.950381dB
		_MidBandOutputCurve.SetValue(65, 4.398045f); //12.865192dB
		_MidBandOutputCurve.SetValue(66, 4.357133f); //12.784016dB
		_MidBandOutputCurve.SetValue(67, 4.318522f); //12.706703dB
		_MidBandOutputCurve.SetValue(68, 4.282084f); //12.633104dB
		_MidBandOutputCurve.SetValue(69, 4.247701f); //12.563078dB
		_MidBandOutputCurve.SetValue(70, 4.215259f); //12.496485dB
		_MidBandOutputCurve.SetValue(71, 4.184657f); //12.433197dB
		_MidBandOutputCurve.SetValue(72, 4.155798f); //12.373090dB
		_MidBandOutputCurve.SetValue(73, 4.128594f); //12.316044dB
		_MidBandOutputCurve.SetValue(74, 4.102961f); //12.261948dB
		_MidBandOutputCurve.SetValue(75, 4.078820f); //12.210690dB
		_MidBandOutputCurve.SetValue(76, 4.056100f); //12.162174dB
		_MidBandOutputCurve.SetValue(77, 4.034737f); //12.116303dB
		_MidBandOutputCurve.SetValue(78, 4.014666f); //12.072988dB
		_MidBandOutputCurve.SetValue(79, 3.995831f); //12.032142dB
		_MidBandOutputCurve.SetValue(80, 3.978180f); //11.993688dB
		_MidBandOutputCurve.SetValue(81, 3.961661f); //11.957546dB
		_MidBandOutputCurve.SetValue(82, 3.946231f); //11.923650dB
		_MidBandOutputCurve.SetValue(83, 3.931847f); //11.891932dB
		_MidBandOutputCurve.SetValue(84, 3.918472f); //11.862334dB
		_MidBandOutputCurve.SetValue(85, 3.906069f); //11.834798dB
		_MidBandOutputCurve.SetValue(86, 3.894606f); //11.809271dB
		_MidBandOutputCurve.SetValue(87, 3.884054f); //11.785706dB
		_MidBandOutputCurve.SetValue(88, 3.874386f); //11.764058dB
		_MidBandOutputCurve.SetValue(89, 3.865576f); //11.744284dB
		_MidBandOutputCurve.SetValue(90, 3.857603f); //11.726351dB
		_MidBandOutputCurve.SetValue(91, 3.850447f); //11.710223dB
		_MidBandOutputCurve.SetValue(92, 3.844090f); //11.695871dB
		_MidBandOutputCurve.SetValue(93, 3.838518f); //11.683271dB
		_MidBandOutputCurve.SetValue(94, 3.833714f); //11.672395dB
		_MidBandOutputCurve.SetValue(95, 3.829669f); //11.663225dB
		_MidBandOutputCurve.SetValue(96, 3.826373f); //11.655746dB
		_MidBandOutputCurve.SetValue(97, 3.823817f); //11.649942dB
		_MidBandOutputCurve.SetValue(98, 3.821997f); //11.645806dB
		_MidBandOutputCurve.SetValue(99, 3.820906f); //11.643327dB
		_MidBandOutputCurve.SetValue(100, 3.820542f); //11.642499dB
		_HighBandOutputCurve.SetValue(0, 59.694256f); //35.518650dB
		_HighBandOutputCurve.SetValue(1, 55.476467f); //34.882175dB
		_HighBandOutputCurve.SetValue(2, 51.732002f); //34.275185dB
		_HighBandOutputCurve.SetValue(3, 48.382000f); //33.693676dB
		_HighBandOutputCurve.SetValue(4, 45.366379f); //33.134682dB
		_HighBandOutputCurve.SetValue(5, 42.639816f); //32.596306dB
		_HighBandOutputCurve.SetValue(6, 40.167995f); //32.077602dB
		_HighBandOutputCurve.SetValue(7, 37.924118f); //31.578308dB
		_HighBandOutputCurve.SetValue(8, 35.886063f); //31.098515dB
		_HighBandOutputCurve.SetValue(9, 34.034405f); //30.638363dB
		_HighBandOutputCurve.SetValue(10, 32.351131f); //30.197788dB
		_HighBandOutputCurve.SetValue(11, 30.819016f); //29.776375dB
		_HighBandOutputCurve.SetValue(12, 29.421490f); //29.373293dB
		_HighBandOutputCurve.SetValue(13, 28.142799f); //28.987345dB
		_HighBandOutputCurve.SetValue(14, 26.968353f); //28.617088dB
		_HighBandOutputCurve.SetValue(15, 25.884949f); //28.260946dB
		_HighBandOutputCurve.SetValue(16, 24.881020f); //27.917362dB
		_HighBandOutputCurve.SetValue(17, 23.946686f); //27.584908dB
		_HighBandOutputCurve.SetValue(18, 23.073769f); //27.262369dB
		_HighBandOutputCurve.SetValue(19, 22.255569f); //26.948774dB
		_HighBandOutputCurve.SetValue(20, 21.486750f); //26.643414dB
		_HighBandOutputCurve.SetValue(21, 20.762993f); //26.345798dB
		_HighBandOutputCurve.SetValue(22, 20.080860f); //26.055645dB
		_HighBandOutputCurve.SetValue(23, 19.437521f); //25.772818dB
		_HighBandOutputCurve.SetValue(24, 18.830549f); //25.497259dB
		_HighBandOutputCurve.SetValue(25, 18.257792f); //25.228964dB
		_HighBandOutputCurve.SetValue(26, 17.717243f); //24.967922dB
		_HighBandOutputCurve.SetValue(27, 17.206989f); //24.714098dB
		_HighBandOutputCurve.SetValue(28, 16.725166f); //24.467407dB
		_HighBandOutputCurve.SetValue(29, 16.269962f); //24.227732dB
		_HighBandOutputCurve.SetValue(30, 15.839618f); //23.994894dB
		_HighBandOutputCurve.SetValue(31, 15.432427f); //23.768684dB
		_HighBandOutputCurve.SetValue(32, 15.046786f); //23.548874dB
		_HighBandOutputCurve.SetValue(33, 14.681187f); //23.335222dB
		_HighBandOutputCurve.SetValue(34, 14.334213f); //23.127478dB
		_HighBandOutputCurve.SetValue(35, 14.004594f); //22.925409dB
		_HighBandOutputCurve.SetValue(36, 13.691134f); //22.728788dB
		_HighBandOutputCurve.SetValue(37, 13.392766f); //22.537405dB
		_HighBandOutputCurve.SetValue(38, 13.108511f); //22.351067dB
		_HighBandOutputCurve.SetValue(39, 12.837486f); //22.169600dB
		_HighBandOutputCurve.SetValue(40, 12.578896f); //21.992849dB
		_HighBandOutputCurve.SetValue(41, 12.332019f); //21.820683dB
		_HighBandOutputCurve.SetValue(42, 12.096185f); //21.652967dB
		_HighBandOutputCurve.SetValue(43, 11.870799f); //21.489599dB
		_HighBandOutputCurve.SetValue(44, 11.655318f); //21.330482dB
		_HighBandOutputCurve.SetValue(45, 11.449233f); //21.175528dB
		_HighBandOutputCurve.SetValue(46, 11.252083f); //21.024658dB
		_HighBandOutputCurve.SetValue(47, 11.063441f); //20.877804dB
		_HighBandOutputCurve.SetValue(48, 10.882905f); //20.734896dB
		_HighBandOutputCurve.SetValue(49, 10.710112f); //20.595879dB
		_HighBandOutputCurve.SetValue(50, 10.544714f); //20.460695dB
		_HighBandOutputCurve.SetValue(51, 10.386386f); //20.329288dB
		_HighBandOutputCurve.SetValue(52, 10.234829f); //20.201611dB
		_HighBandOutputCurve.SetValue(53, 10.089755f); //20.077612dB
		_HighBandOutputCurve.SetValue(54, 9.950892f); //19.957241dB
		_HighBandOutputCurve.SetValue(55, 9.817992f); //19.840454dB
		_HighBandOutputCurve.SetValue(56, 9.690810f); //19.727201dB
		_HighBandOutputCurve.SetValue(57, 9.569109f); //19.617430dB
		_HighBandOutputCurve.SetValue(58, 9.452676f); //19.511095dB
		_HighBandOutputCurve.SetValue(59, 9.341296f); //19.408142dB
		_HighBandOutputCurve.SetValue(60, 9.234774f); //19.308525dB
		_HighBandOutputCurve.SetValue(61, 9.132915f); //19.212187dB
		_HighBandOutputCurve.SetValue(62, 9.035540f); //19.119081dB
		_HighBandOutputCurve.SetValue(63, 8.942471f); //19.029150dB
		_HighBandOutputCurve.SetValue(64, 8.853541f); //18.942341dB
		_HighBandOutputCurve.SetValue(65, 8.768598f); //18.858603dB
		_HighBandOutputCurve.SetValue(66, 8.687480f); //18.777876dB
		_HighBandOutputCurve.SetValue(67, 8.610050f); //18.700113dB
		_HighBandOutputCurve.SetValue(68, 8.536168f); //18.625259dB
		_HighBandOutputCurve.SetValue(69, 8.465705f); //18.553263dB
		_HighBandOutputCurve.SetValue(70, 8.398540f); //18.484076dB
		_HighBandOutputCurve.SetValue(71, 8.334551f); //18.417645dB
		_HighBandOutputCurve.SetValue(72, 8.273632f); //18.353924dB
		_HighBandOutputCurve.SetValue(73, 8.215675f); //18.292866dB
		_HighBandOutputCurve.SetValue(74, 8.160583f); //18.234425dB
		_HighBandOutputCurve.SetValue(75, 8.108261f); //18.178555dB
		_HighBandOutputCurve.SetValue(76, 8.058624f); //18.125217dB
		_HighBandOutputCurve.SetValue(77, 8.011585f); //18.074369dB
		_HighBandOutputCurve.SetValue(78, 7.967070f); //18.025972dB
		_HighBandOutputCurve.SetValue(79, 7.925002f); //17.979988dB
		_HighBandOutputCurve.SetValue(80, 7.885316f); //17.936382dB
		_HighBandOutputCurve.SetValue(81, 7.847945f); //17.895119dB
		_HighBandOutputCurve.SetValue(82, 7.812831f); //17.856169dB
		_HighBandOutputCurve.SetValue(83, 7.779912f); //17.819494dB
		_HighBandOutputCurve.SetValue(84, 7.749144f); //17.785074dB
		_HighBandOutputCurve.SetValue(85, 7.720470f); //17.752874dB
		_HighBandOutputCurve.SetValue(86, 7.693851f); //17.722876dB
		_HighBandOutputCurve.SetValue(87, 7.669241f); //17.695047dB
		_HighBandOutputCurve.SetValue(88, 7.646606f); //17.669374dB
		_HighBandOutputCurve.SetValue(89, 7.625906f); //17.645828dB
		_HighBandOutputCurve.SetValue(90, 7.607113f); //17.624397dB
		_HighBandOutputCurve.SetValue(91, 7.590192f); //17.605055dB
		_HighBandOutputCurve.SetValue(92, 7.575124f); //17.587795dB
		_HighBandOutputCurve.SetValue(93, 7.561883f); //17.572599dB
		_HighBandOutputCurve.SetValue(94, 7.550448f); //17.559454dB
		_HighBandOutputCurve.SetValue(95, 7.540802f); //17.548349dB
		_HighBandOutputCurve.SetValue(96, 7.532930f); //17.539278dB
		_HighBandOutputCurve.SetValue(97, 7.526817f); //17.532227dB
		_HighBandOutputCurve.SetValue(98, 7.522460f); //17.527197dB
		_HighBandOutputCurve.SetValue(99, 7.519846f); //17.524179dB
		_HighBandOutputCurve.SetValue(100, 7.518975f); //17.523172dB
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
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(6.0f, 24.0f, _LowBandCompression)) };

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
				const float32 output_gain{ 0.125f * 0.5f * _LowBandGain * _LowBandOutputCurve.Sample(_LowBandCompression) };

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
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(18.0f, 72.0f, BaseMath::InverseSquare(_MidBandDistortion))) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
					{
						_MidBand._OverSamplers[channel_index].Feed(_WorkingBuffer.At(channel_index).At(sample_index));

						for (float32 &sample : _MidBand._OverSamplers[channel_index].GetSamples())
						{
							sample = std::tanh(sample * (sample >= 0.0f ? 2.0f : 0.5f) * gain);
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
				const float32 output_gain{ 0.125f * 0.25f * _MidBandGain * _MidBandOutputCurve.Sample(_MidBandDistortion) };

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
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(18.0f, 72.0f, BaseMath::InverseSquare(_HighBandDistortion))) };

				for (uint8 channel_index{ 0 }; channel_index < number_of_channels; ++channel_index)
				{
					for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
					{
						_HighBand._OverSamplers[channel_index].Feed(_WorkingBuffer.At(channel_index).At(sample_index));

						for (float32 &sample : _HighBand._OverSamplers[channel_index].GetSamples())
						{
							sample = std::tanh(sample * (sample >= 0.0f ? 0.5f : 2.0f) * gain);
						}

						_WorkingBuffer.At(channel_index).At(sample_index) = _HighBand._OverSamplers[channel_index].DownSample();
					}
				}
			}

			//Apply the post high pass filter.
			_HighBand._PostHighPassFilter._Frequency = _HighBandHighPassFrequency;
			_HighBand._PostHighPassFilter.Process(context, _WorkingBuffer, &_WorkingBuffer, number_of_channels, number_of_samples);

			//Apply the gain.
			{
				const float32 output_gain{ 0.125f * 0.125f * _HighBandGain * _HighBandOutputCurve.Sample(_HighBandDistortion) };

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
		HighPassFilter _PreHighPassFilter1{ 150.0f, 1.0f, 3 };

		//Pre high pass filter 2.
		HighPassFilter _PreHighPassFilter2{ 300.0f, 1.0f, 2 };

		//Pre high pass filter 3.
		HighPassFilter _PreHighPassFilter3{ 600.0f, 1.0f, 1 };

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
		HighPassFilter _HighPassFilter1{ 200.0f, 1.0f, 3 };

		//High pass filter 2.
		HighPassFilter _HighPassFilter2{ 400.0f, 1.0f, 2 };

		//High pass filter 3.
		HighPassFilter _HighPassFilter3{ 800.0f, 1.0f, 1 };

		//The peak filter.
		PeakFilter _PeakFilter{ 400.0f, Audio::DecibelsToGain(-12.0f), 1.0f };

		//The oversamplers.
		StaticArray<OverSampler<4>, 2> _OverSamplers;

		//The post high pass filter.
		HighPassFilter _PostHighPassFilter{ HIGH_BAND_HIGH_PASS_DEFAULT_FREQUENCY, 1.0f, 2 };

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

};