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
		_LowBandOutputCurve.SetValue(0, 13.337019f); //22.501175dB
		_LowBandOutputCurve.SetValue(1, 13.274120f); //22.460115dB
		_LowBandOutputCurve.SetValue(2, 13.212552f); //22.419733dB
		_LowBandOutputCurve.SetValue(3, 13.152333f); //22.380056dB
		_LowBandOutputCurve.SetValue(4, 13.093461f); //22.341089dB
		_LowBandOutputCurve.SetValue(5, 13.035908f); //22.302824dB
		_LowBandOutputCurve.SetValue(6, 12.979665f); //22.265268dB
		_LowBandOutputCurve.SetValue(7, 12.924708f); //22.228415dB
		_LowBandOutputCurve.SetValue(8, 12.871065f); //22.192289dB
		_LowBandOutputCurve.SetValue(9, 12.818679f); //22.156866dB
		_LowBandOutputCurve.SetValue(10, 12.767537f); //22.122141dB
		_LowBandOutputCurve.SetValue(11, 12.717639f); //22.088129dB
		_LowBandOutputCurve.SetValue(12, 12.668992f); //22.054842dB
		_LowBandOutputCurve.SetValue(13, 12.621537f); //22.022243dB
		_LowBandOutputCurve.SetValue(14, 12.575281f); //21.990355dB
		_LowBandOutputCurve.SetValue(15, 12.530182f); //21.959146dB
		_LowBandOutputCurve.SetValue(16, 12.486259f); //21.928646dB
		_LowBandOutputCurve.SetValue(17, 12.443456f); //21.898819dB
		_LowBandOutputCurve.SetValue(18, 12.401764f); //21.869669dB
		_LowBandOutputCurve.SetValue(19, 12.361215f); //21.841223dB
		_LowBandOutputCurve.SetValue(20, 12.321729f); //21.813433dB
		_LowBandOutputCurve.SetValue(21, 12.283314f); //21.786310dB
		_LowBandOutputCurve.SetValue(22, 12.245942f); //21.759844dB
		_LowBandOutputCurve.SetValue(23, 12.209605f); //21.734032dB
		_LowBandOutputCurve.SetValue(24, 12.174273f); //21.708860dB
		_LowBandOutputCurve.SetValue(25, 12.139938f); //21.684328dB
		_LowBandOutputCurve.SetValue(26, 12.106575f); //21.660425dB
		_LowBandOutputCurve.SetValue(27, 12.074171f); //21.637146dB
		_LowBandOutputCurve.SetValue(28, 12.042730f); //21.614498dB
		_LowBandOutputCurve.SetValue(29, 12.012205f); //21.592455dB
		_LowBandOutputCurve.SetValue(30, 11.982571f); //21.570999dB
		_LowBandOutputCurve.SetValue(31, 11.953861f); //21.550163dB
		_LowBandOutputCurve.SetValue(32, 11.926023f); //21.529913dB
		_LowBandOutputCurve.SetValue(33, 11.899031f); //21.510231dB
		_LowBandOutputCurve.SetValue(34, 11.872910f); //21.491142dB
		_LowBandOutputCurve.SetValue(35, 11.847621f); //21.472624dB
		_LowBandOutputCurve.SetValue(36, 11.823172f); //21.454679dB
		_LowBandOutputCurve.SetValue(37, 11.799518f); //21.437284dB
		_LowBandOutputCurve.SetValue(38, 11.776667f); //21.420446dB
		_LowBandOutputCurve.SetValue(39, 11.754600f); //21.404156dB
		_LowBandOutputCurve.SetValue(40, 11.733315f); //21.388414dB
		_LowBandOutputCurve.SetValue(41, 11.712784f); //21.373201dB
		_LowBandOutputCurve.SetValue(42, 11.693022f); //21.358534dB
		_LowBandOutputCurve.SetValue(43, 11.673991f); //21.344387dB
		_LowBandOutputCurve.SetValue(44, 11.655715f); //21.330778dB
		_LowBandOutputCurve.SetValue(45, 11.638148f); //21.317677dB
		_LowBandOutputCurve.SetValue(46, 11.621334f); //21.305120dB
		_LowBandOutputCurve.SetValue(47, 11.605217f); //21.293064dB
		_LowBandOutputCurve.SetValue(48, 11.589797f); //21.281515dB
		_LowBandOutputCurve.SetValue(49, 11.575070f); //21.270472dB
		_LowBandOutputCurve.SetValue(50, 11.561046f); //21.259941dB
		_LowBandOutputCurve.SetValue(51, 11.547684f); //21.249897dB
		_LowBandOutputCurve.SetValue(52, 11.535006f); //21.240355dB
		_LowBandOutputCurve.SetValue(53, 11.523002f); //21.231312dB
		_LowBandOutputCurve.SetValue(54, 11.511672f); //21.222767dB
		_LowBandOutputCurve.SetValue(55, 11.500983f); //21.214699dB
		_LowBandOutputCurve.SetValue(56, 11.490952f); //21.207121dB
		_LowBandOutputCurve.SetValue(57, 11.481569f); //21.200024dB
		_LowBandOutputCurve.SetValue(58, 11.472817f); //21.193401dB
		_LowBandOutputCurve.SetValue(59, 11.464727f); //21.187273dB
		_LowBandOutputCurve.SetValue(60, 11.457274f); //21.181625dB
		_LowBandOutputCurve.SetValue(61, 11.450424f); //21.176432dB
		_LowBandOutputCurve.SetValue(62, 11.444212f); //21.171717dB
		_LowBandOutputCurve.SetValue(63, 11.438619f); //21.167471dB
		_LowBandOutputCurve.SetValue(64, 11.433634f); //21.163685dB
		_LowBandOutputCurve.SetValue(65, 11.429284f); //21.160379dB
		_LowBandOutputCurve.SetValue(66, 11.425508f); //21.157511dB
		_LowBandOutputCurve.SetValue(67, 11.422354f); //21.155111dB
		_LowBandOutputCurve.SetValue(68, 11.419789f); //21.153162dB
		_LowBandOutputCurve.SetValue(69, 11.417808f); //21.151653dB
		_LowBandOutputCurve.SetValue(70, 11.416404f); //21.150585dB
		_LowBandOutputCurve.SetValue(71, 11.415592f); //21.149969dB
		_LowBandOutputCurve.SetValue(72, 11.415336f); //21.149773dB
		_LowBandOutputCurve.SetValue(73, 11.415636f); //21.150002dB
		_LowBandOutputCurve.SetValue(74, 11.416530f); //21.150681dB
		_LowBandOutputCurve.SetValue(75, 11.417958f); //21.151768dB
		_LowBandOutputCurve.SetValue(76, 11.419946f); //21.153280dB
		_LowBandOutputCurve.SetValue(77, 11.422457f); //21.155190dB
		_LowBandOutputCurve.SetValue(78, 11.425508f); //21.157511dB
		_LowBandOutputCurve.SetValue(79, 11.429087f); //21.160231dB
		_LowBandOutputCurve.SetValue(80, 11.433182f); //21.163342dB
		_LowBandOutputCurve.SetValue(81, 11.437771f); //21.166826dB
		_LowBandOutputCurve.SetValue(82, 11.442870f); //21.170698dB
		_LowBandOutputCurve.SetValue(83, 11.448460f); //21.174940dB
		_LowBandOutputCurve.SetValue(84, 11.454533f); //21.179546dB
		_LowBandOutputCurve.SetValue(85, 11.461071f); //21.184504dB
		_LowBandOutputCurve.SetValue(86, 11.468070f); //21.189806dB
		_LowBandOutputCurve.SetValue(87, 11.475520f); //21.195448dB
		_LowBandOutputCurve.SetValue(88, 11.483432f); //21.201433dB
		_LowBandOutputCurve.SetValue(89, 11.491751f); //21.207724dB
		_LowBandOutputCurve.SetValue(90, 11.500504f); //21.214336dB
		_LowBandOutputCurve.SetValue(91, 11.509646f); //21.221239dB
		_LowBandOutputCurve.SetValue(92, 11.519214f); //21.228456dB
		_LowBandOutputCurve.SetValue(93, 11.529143f); //21.235941dB
		_LowBandOutputCurve.SetValue(94, 11.539445f); //21.243698dB
		_LowBandOutputCurve.SetValue(95, 11.550157f); //21.251757dB
		_LowBandOutputCurve.SetValue(96, 11.561185f); //21.260046dB
		_LowBandOutputCurve.SetValue(97, 11.572574f); //21.268599dB
		_LowBandOutputCurve.SetValue(98, 11.584275f); //21.277376dB
		_LowBandOutputCurve.SetValue(99, 11.596301f); //21.286388dB
		_LowBandOutputCurve.SetValue(100, 11.608639f); //21.295626dB
		_MidBandOutputCurve.SetValue(0, 47.634190f); //33.558376dB
		_MidBandOutputCurve.SetValue(1, 44.974319f); //33.059292dB
		_MidBandOutputCurve.SetValue(2, 42.577656f); //32.583633dB
		_MidBandOutputCurve.SetValue(3, 40.429081f); //32.133877dB
		_MidBandOutputCurve.SetValue(4, 38.510078f); //31.711487dB
		_MidBandOutputCurve.SetValue(5, 36.799873f); //31.316925dB
		_MidBandOutputCurve.SetValue(6, 35.276558f); //30.949724dB
		_MidBandOutputCurve.SetValue(7, 33.917961f); //30.608595dB
		_MidBandOutputCurve.SetValue(8, 32.702015f); //30.291491dB
		_MidBandOutputCurve.SetValue(9, 31.607529f); //29.995810dB
		_MidBandOutputCurve.SetValue(10, 30.615437f); //29.718809dB
		_MidBandOutputCurve.SetValue(11, 29.710024f); //29.458059dB
		_MidBandOutputCurve.SetValue(12, 28.879919f); //29.211920dB
		_MidBandOutputCurve.SetValue(13, 28.118013f); //28.979692dB
		_MidBandOutputCurve.SetValue(14, 27.420624f); //28.761545dB
		_MidBandOutputCurve.SetValue(15, 26.786226f); //28.558229dB
		_MidBandOutputCurve.SetValue(16, 26.213909f); //28.370634dB
		_MidBandOutputCurve.SetValue(17, 25.702475f); //28.199499dB
		_MidBandOutputCurve.SetValue(18, 25.249716f); //28.045130dB
		_MidBandOutputCurve.SetValue(19, 24.852461f); //27.907387dB
		_MidBandOutputCurve.SetValue(20, 24.506828f); //27.785742dB
		_MidBandOutputCurve.SetValue(21, 24.208813f); //27.679468dB
		_MidBandOutputCurve.SetValue(22, 23.954548f); //27.587759dB
		_MidBandOutputCurve.SetValue(23, 23.740602f); //27.509834dB
		_MidBandOutputCurve.SetValue(24, 23.564039f); //27.444994dB
		_MidBandOutputCurve.SetValue(25, 23.422272f); //27.392580dB
		_MidBandOutputCurve.SetValue(26, 23.313046f); //27.351980dB
		_MidBandOutputCurve.SetValue(27, 23.234238f); //27.322569dB
		_MidBandOutputCurve.SetValue(28, 23.183897f); //27.303728dB
		_MidBandOutputCurve.SetValue(29, 23.160231f); //27.294857dB
		_MidBandOutputCurve.SetValue(30, 23.161528f); //27.295343dB
		_MidBandOutputCurve.SetValue(31, 23.186234f); //27.304604dB
		_MidBandOutputCurve.SetValue(32, 23.232975f); //27.322096dB
		_MidBandOutputCurve.SetValue(33, 23.300404f); //27.347269dB
		_MidBandOutputCurve.SetValue(34, 23.387260f); //27.379587dB
		_MidBandOutputCurve.SetValue(35, 23.492294f); //27.418509dB
		_MidBandOutputCurve.SetValue(36, 23.614239f); //27.463478dB
		_MidBandOutputCurve.SetValue(37, 23.751795f); //27.513927dB
		_MidBandOutputCurve.SetValue(38, 23.903637f); //27.569279dB
		_MidBandOutputCurve.SetValue(39, 24.068392f); //27.628941dB
		_MidBandOutputCurve.SetValue(40, 24.244688f); //27.692331dB
		_MidBandOutputCurve.SetValue(41, 24.431093f); //27.758858dB
		_MidBandOutputCurve.SetValue(42, 24.626184f); //27.827942dB
		_MidBandOutputCurve.SetValue(43, 24.828564f); //27.899031dB
		_MidBandOutputCurve.SetValue(44, 25.036831f); //27.971586dB
		_MidBandOutputCurve.SetValue(45, 25.249653f); //28.045107dB
		_MidBandOutputCurve.SetValue(46, 25.465767f); //28.119135dB
		_MidBandOutputCurve.SetValue(47, 25.683962f); //28.193239dB
		_MidBandOutputCurve.SetValue(48, 25.903158f); //28.267054dB
		_MidBandOutputCurve.SetValue(49, 26.122313f); //28.340231dB
		_MidBandOutputCurve.SetValue(50, 26.340515f); //28.412485dB
		_MidBandOutputCurve.SetValue(51, 26.556910f); //28.483549dB
		_MidBandOutputCurve.SetValue(52, 26.770803f); //28.553228dB
		_MidBandOutputCurve.SetValue(53, 26.981474f); //28.621313dB
		_MidBandOutputCurve.SetValue(54, 27.188406f); //28.687674dB
		_MidBandOutputCurve.SetValue(55, 27.391090f); //28.752186dB
		_MidBandOutputCurve.SetValue(56, 27.589119f); //28.814756dB
		_MidBandOutputCurve.SetValue(57, 27.782167f); //28.875322dB
		_MidBandOutputCurve.SetValue(58, 27.969969f); //28.933840dB
		_MidBandOutputCurve.SetValue(59, 28.152317f); //28.990282dB
		_MidBandOutputCurve.SetValue(60, 28.329058f); //29.044641dB
		_MidBandOutputCurve.SetValue(61, 28.500107f); //29.096930dB
		_MidBandOutputCurve.SetValue(62, 28.665407f); //29.147161dB
		_MidBandOutputCurve.SetValue(63, 28.824940f); //29.195368dB
		_MidBandOutputCurve.SetValue(64, 28.978718f); //29.241583dB
		_MidBandOutputCurve.SetValue(65, 29.126768f); //29.285845dB
		_MidBandOutputCurve.SetValue(66, 29.269178f); //29.328209dB
		_MidBandOutputCurve.SetValue(67, 29.406012f); //29.368721dB
		_MidBandOutputCurve.SetValue(68, 29.537355f); //29.407431dB
		_MidBandOutputCurve.SetValue(69, 29.663292f); //29.444387dB
		_MidBandOutputCurve.SetValue(70, 29.783970f); //29.479650dB
		_MidBandOutputCurve.SetValue(71, 29.899466f); //29.513268dB
		_MidBandOutputCurve.SetValue(72, 30.009907f); //29.545292dB
		_MidBandOutputCurve.SetValue(73, 30.115391f); //29.575769dB
		_MidBandOutputCurve.SetValue(74, 30.216036f); //29.604750dB
		_MidBandOutputCurve.SetValue(75, 30.311975f); //29.632284dB
		_MidBandOutputCurve.SetValue(76, 30.403303f); //29.658415dB
		_MidBandOutputCurve.SetValue(77, 30.490110f); //29.683180dB
		_MidBandOutputCurve.SetValue(78, 30.572515f); //29.706623dB
		_MidBandOutputCurve.SetValue(79, 30.650610f); //29.728781dB
		_MidBandOutputCurve.SetValue(80, 30.724491f); //29.749693dB
		_MidBandOutputCurve.SetValue(81, 30.794239f); //29.769388dB
		_MidBandOutputCurve.SetValue(82, 30.859936f); //29.787901dB
		_MidBandOutputCurve.SetValue(83, 30.921690f); //29.805264dB
		_MidBandOutputCurve.SetValue(84, 30.979519f); //29.821493dB
		_MidBandOutputCurve.SetValue(85, 31.033531f); //29.836622dB
		_MidBandOutputCurve.SetValue(86, 31.083778f); //29.850676dB
		_MidBandOutputCurve.SetValue(87, 31.130322f); //29.863672dB
		_MidBandOutputCurve.SetValue(88, 31.173204f); //29.875628dB
		_MidBandOutputCurve.SetValue(89, 31.212479f); //29.886564dB
		_MidBandOutputCurve.SetValue(90, 31.248199f); //29.896500dB
		_MidBandOutputCurve.SetValue(91, 31.280399f); //29.905445dB
		_MidBandOutputCurve.SetValue(92, 31.309114f); //29.913416dB
		_MidBandOutputCurve.SetValue(93, 31.334366f); //29.920418dB
		_MidBandOutputCurve.SetValue(94, 31.356203f); //29.926470dB
		_MidBandOutputCurve.SetValue(95, 31.374653f); //29.931578dB
		_MidBandOutputCurve.SetValue(96, 31.389711f); //29.935745dB
		_MidBandOutputCurve.SetValue(97, 31.401405f); //29.938980dB
		_MidBandOutputCurve.SetValue(98, 31.409740f); //29.941286dB
		_MidBandOutputCurve.SetValue(99, 31.414751f); //29.942671dB
		_MidBandOutputCurve.SetValue(100, 31.416414f); //29.943132dB
		_HighBandOutputCurve.SetValue(0, 73.872993f); //37.369713dB
		_HighBandOutputCurve.SetValue(1, 70.822823f); //37.003464dB
		_HighBandOutputCurve.SetValue(2, 68.234055f); //36.680023dB
		_HighBandOutputCurve.SetValue(3, 65.980751f); //36.388344dB
		_HighBandOutputCurve.SetValue(4, 63.971569f); //36.119740dB
		_HighBandOutputCurve.SetValue(5, 62.156864f); //35.869781dB
		_HighBandOutputCurve.SetValue(6, 60.519276f); //35.637875dB
		_HighBandOutputCurve.SetValue(7, 59.056576f); //35.425365dB
		_HighBandOutputCurve.SetValue(8, 57.765202f); //35.233326dB
		_HighBandOutputCurve.SetValue(9, 56.630226f); //35.060966dB
		_HighBandOutputCurve.SetValue(10, 55.623898f); //34.905228dB
		_HighBandOutputCurve.SetValue(11, 54.710957f); //34.761486dB
		_HighBandOutputCurve.SetValue(12, 53.856632f); //34.624783dB
		_HighBandOutputCurve.SetValue(13, 53.034180f); //34.491116dB
		_HighBandOutputCurve.SetValue(14, 52.228386f); //34.358131dB
		_HighBandOutputCurve.SetValue(15, 51.435947f); //34.225334dB
		_HighBandOutputCurve.SetValue(16, 50.662182f); //34.093678dB
		_HighBandOutputCurve.SetValue(17, 49.917030f); //33.964973dB
		_HighBandOutputCurve.SetValue(18, 49.210682f); //33.841187dB
		_HighBandOutputCurve.SetValue(19, 48.550900f); //33.723946dB
		_HighBandOutputCurve.SetValue(20, 47.941330f); //33.614201dB
		_HighBandOutputCurve.SetValue(21, 47.381714f); //33.512215dB
		_HighBandOutputCurve.SetValue(22, 46.868793f); //33.417675dB
		_HighBandOutputCurve.SetValue(23, 46.397438f); //33.329880dB
		_HighBandOutputCurve.SetValue(24, 45.961864f); //33.247952dB
		_HighBandOutputCurve.SetValue(25, 45.556492f); //33.171005dB
		_HighBandOutputCurve.SetValue(26, 45.176601f); //33.098270dB
		_HighBandOutputCurve.SetValue(27, 44.818352f); //33.029118dB
		_HighBandOutputCurve.SetValue(28, 44.479057f); //32.963112dB
		_HighBandOutputCurve.SetValue(29, 44.156994f); //32.899990dB
		_HighBandOutputCurve.SetValue(30, 43.851166f); //32.839622dB
		_HighBandOutputCurve.SetValue(31, 43.561344f); //32.782024dB
		_HighBandOutputCurve.SetValue(32, 43.287613f); //32.727272dB
		_HighBandOutputCurve.SetValue(33, 43.030457f); //32.675518dB
		_HighBandOutputCurve.SetValue(34, 42.790401f); //32.626926dB
		_HighBandOutputCurve.SetValue(35, 42.568027f); //32.581669dB
		_HighBandOutputCurve.SetValue(36, 42.363853f); //32.539909dB
		_HighBandOutputCurve.SetValue(37, 42.178230f); //32.501766dB
		_HighBandOutputCurve.SetValue(38, 42.011265f); //32.467316dB
		_HighBandOutputCurve.SetValue(39, 41.862873f); //32.436581dB
		_HighBandOutputCurve.SetValue(40, 41.732723f); //32.409534dB
		_HighBandOutputCurve.SetValue(41, 41.620281f); //32.386101dB
		_HighBandOutputCurve.SetValue(42, 41.524963f); //32.366184dB
		_HighBandOutputCurve.SetValue(43, 41.445957f); //32.349644dB
		_HighBandOutputCurve.SetValue(44, 41.382481f); //32.336330dB
		_HighBandOutputCurve.SetValue(45, 41.333733f); //32.326092dB
		_HighBandOutputCurve.SetValue(46, 41.298904f); //32.318771dB
		_HighBandOutputCurve.SetValue(47, 41.277203f); //32.314205dB
		_HighBandOutputCurve.SetValue(48, 41.267925f); //32.312252dB
		_HighBandOutputCurve.SetValue(49, 41.270248f); //32.312740dB
		_HighBandOutputCurve.SetValue(50, 41.283482f); //32.315525dB
		_HighBandOutputCurve.SetValue(51, 41.306889f); //32.320450dB
		_HighBandOutputCurve.SetValue(52, 41.339760f); //32.327358dB
		_HighBandOutputCurve.SetValue(53, 41.381321f); //32.336086dB
		_HighBandOutputCurve.SetValue(54, 41.430855f); //32.346478dB
		_HighBandOutputCurve.SetValue(55, 41.487572f); //32.358360dB
		_HighBandOutputCurve.SetValue(56, 41.550777f); //32.371582dB
		_HighBandOutputCurve.SetValue(57, 41.619720f); //32.385983dB
		_HighBandOutputCurve.SetValue(58, 41.693668f); //32.401402dB
		_HighBandOutputCurve.SetValue(59, 41.771954f); //32.417694dB
		_HighBandOutputCurve.SetValue(60, 41.853867f); //32.434711dB
		_HighBandOutputCurve.SetValue(61, 41.938782f); //32.452316dB
		_HighBandOutputCurve.SetValue(62, 42.026085f); //32.470379dB
		_HighBandOutputCurve.SetValue(63, 42.115200f); //32.488777dB
		_HighBandOutputCurve.SetValue(64, 42.205559f); //32.507393dB
		_HighBandOutputCurve.SetValue(65, 42.296703f); //32.526131dB
		_HighBandOutputCurve.SetValue(66, 42.388126f); //32.544884dB
		_HighBandOutputCurve.SetValue(67, 42.479424f); //32.563572dB
		_HighBandOutputCurve.SetValue(68, 42.570187f); //32.582111dB
		_HighBandOutputCurve.SetValue(69, 42.660061f); //32.600430dB
		_HighBandOutputCurve.SetValue(70, 42.748734f); //32.618465dB
		_HighBandOutputCurve.SetValue(71, 42.835926f); //32.636162dB
		_HighBandOutputCurve.SetValue(72, 42.921341f); //32.653465dB
		_HighBandOutputCurve.SetValue(73, 43.004799f); //32.670338dB
		_HighBandOutputCurve.SetValue(74, 43.086063f); //32.686737dB
		_HighBandOutputCurve.SetValue(75, 43.165009f); //32.702637dB
		_HighBandOutputCurve.SetValue(76, 43.241440f); //32.718002dB
		_HighBandOutputCurve.SetValue(77, 43.315247f); //32.732815dB
		_HighBandOutputCurve.SetValue(78, 43.386295f); //32.747051dB
		_HighBandOutputCurve.SetValue(79, 43.454529f); //32.760700dB
		_HighBandOutputCurve.SetValue(80, 43.519825f); //32.773743dB
		_HighBandOutputCurve.SetValue(81, 43.582142f); //32.786171dB
		_HighBandOutputCurve.SetValue(82, 43.641422f); //32.797977dB
		_HighBandOutputCurve.SetValue(83, 43.697617f); //32.809155dB
		_HighBandOutputCurve.SetValue(84, 43.750675f); //32.819695dB
		_HighBandOutputCurve.SetValue(85, 43.800602f); //32.829601dB
		_HighBandOutputCurve.SetValue(86, 43.847347f); //32.838867dB
		_HighBandOutputCurve.SetValue(87, 43.890915f); //32.847492dB
		_HighBandOutputCurve.SetValue(88, 43.931244f); //32.855469dB
		_HighBandOutputCurve.SetValue(89, 43.968376f); //32.862808dB
		_HighBandOutputCurve.SetValue(90, 44.002254f); //32.869499dB
		_HighBandOutputCurve.SetValue(91, 44.032955f); //32.875557dB
		_HighBandOutputCurve.SetValue(92, 44.060390f); //32.880966dB
		_HighBandOutputCurve.SetValue(93, 44.084583f); //32.885735dB
		_HighBandOutputCurve.SetValue(94, 44.105572f); //32.889870dB
		_HighBandOutputCurve.SetValue(95, 44.123287f); //32.893356dB
		_HighBandOutputCurve.SetValue(96, 44.137825f); //32.896217dB
		_HighBandOutputCurve.SetValue(97, 44.149128f); //32.898441dB
		_HighBandOutputCurve.SetValue(98, 44.157177f); //32.900024dB
		_HighBandOutputCurve.SetValue(99, 44.162025f); //32.900978dB
		_HighBandOutputCurve.SetValue(100, 44.163651f); //32.901299dB
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
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(8.0f, 32.0f, _LowBandCompression)) };

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
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(16.0f, 96.0f, BaseMath::InverseSquare(_MidBandDistortion))) };

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
				const float32 gain{ Audio::DecibelsToGain(BaseMath::LinearlyInterpolate(16.0f, 96.0f, BaseMath::InverseSquare(_HighBandDistortion))) };

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
		_PostFilters._PeakFilter.Process(context, (*outputs), outputs, number_of_channels, number_of_samples);
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

		//The peak filter.
		PeakFilter _PeakFilter{ 400.0f, Audio::DecibelsToGain(-3.0f), 1.0f };

		//The high shelf filter.
		HighShelfFilter _HighShelfFilter{ 10'000.0f, Audio::DecibelsToGain(1.5f) };

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