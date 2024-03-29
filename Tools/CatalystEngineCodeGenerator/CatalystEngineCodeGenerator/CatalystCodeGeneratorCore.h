#pragma once

//STL includes.
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

//Windows.
#include <Windows.h>

//Undefine annoying Windows defines. Fuck Windows. I hate everything.
#undef CreateDirectory
#undef max
#undef min

//Include the intrinsics header.
#include <intrin.h>

/*
*	Builtin types.
*/
using uint8 = unsigned char;
using uint16 = unsigned short int;
using uint32 = unsigned int;
using uint64 = unsigned long long int;

using int8 = char;
using int16 = short int;
using int32 = int;
using int64 = long long int;

using byte = uint8;

using float32 = float;
using float64 = double;

/*
*	Ensure that all builtin types are indeed of the correct size.
*/
static_assert(sizeof(uint8) == 1, "Size of uint8 is not 1 bytes, how could this be?");
static_assert(sizeof(uint16) == 2, "Size of uint16 is not 2 bytes, how could this be?");
static_assert(sizeof(uint32) == 4, "Size of uint32 is not 4 bytes, how could this be?");
static_assert(sizeof(uint64) == 8, "Size of uint64 is not 8 bytes, how could this be?");

static_assert(sizeof(int8) == 1, "Size of int8 is not 1 bytes, how could this be?");
static_assert(sizeof(int16) == 2, "Size of int16 is not 2 bytes, how could this be?");
static_assert(sizeof(int32) == 4, "Size of int32 is not 4 bytes, how could this be?");
static_assert(sizeof(int64) == 8, "Size of int64 is not 8 bytes, how could this be?");

static_assert(sizeof(byte) == 1, "Size of byte is not 1 bytes, how could this be?");

static_assert(sizeof(float32) == 4, "Size of float32 is not 4 bytes, how could this be?");
static_assert(sizeof(float64) == 8, "Size of float64 is not 8 bytes, how could this be?");

/*
*	Type maximums/minimums.
*/
#define FLOAT_MAXIMUM (std::numeric_limits<float>::max())
#define FLOAT_MINIMUM (std::numeric_limits<float>::min())

#define DOUBLE_MAXIMUM (std::numeric_limits<double>::max())
#define DOUBLE_MINIMUM (std::numeric_limits<double>::min())

#define FLOAT32_MAXIMUM (FLOAT_MAXIMUM)
#define FLOAT32_MINIMUM (FLOAT_MINIMUM)

#define FLOAT64_MAXIMUM (DOUBLE_MAXIMUM)
#define FLOAT64_MINIMUM (DOUBLE_MINIMUM)

#define INT8_MAXIMUM (std::numeric_limits<int8>::max())
#define INT16_MAXIMUM (std::numeric_limits<int16>::max())
#define INT24_MAXIMUM (8'388'607)
#define INT32_MAXIMUM (std::numeric_limits<int32>::max())
#define INT64_MAXIMUM (std::numeric_limits<int64>::max())

#define INT8_MINIMUM (std::numeric_limits<int8>::min())
#define INT16_MINIMUM (std::numeric_limits<int16>::min())
#define INT24_MINIMUM (-8'388'608)
#define INT32_MINIMUM (std::numeric_limits<int32>::min())
#define INT64_MINIMUM (std::numeric_limits<int64>::min())

#define UINT8_MAXIMUM (std::numeric_limits<uint8>::max())
#define UINT16_MAXIMUM (std::numeric_limits<uint16>::max())
#define UINT24_MAXIMUM (16'777'215)
#define UINT32_MAXIMUM (std::numeric_limits<uint32>::max())
#define UINT64_MAXIMUM (std::numeric_limits<uint64>::max())

#define UINT8_MINIMUM (std::numeric_limits<uint8>::min())
#define UINT16_MINIMUM (std::numeric_limits<uint16>::min())
#define UINT24_MINIMUM (0)
#define UINT32_MINIMUM (std::numeric_limits<uint32>::min())
#define UINT64_MINIMUM (std::numeric_limits<uint64>::min())

#define BYTE_MAXIMUM (UINT8_MAXIMUM)

#define FLOAT32_EPSILON (std::numeric_limits<float32>::epsilon())
#define FLOAT64_EPSILON (std::numeric_limits<float64>::epsilon())

/*
*	Defines the maximum file path length. Matches Window's MAX_PATH currently.
*/
#define MAXIMUM_FILE_PATH_LENGTH (260)

/*
*	Macros!
*/
#define PRINT_TO_OUTPUT(MESSAGE) { std::ostringstream output; output << MESSAGE << std::endl; OutputDebugStringA(output.str().c_str()); }
#define BREAKPOINT() { __nop(); __debugbreak(); }
#define ASSERT(CONDITION, MESSAGE) if (!(CONDITION)) { PRINT_TO_OUTPUT(MESSAGE); BREAKPOINT(); }
#define BIT(INDEX) (static_cast<uint64>(1) << static_cast<uint64>(INDEX))
#define TEST_BIT(BIT_FIELD, BIT) ((BIT_FIELD & BIT) == BIT)

/*
*	Returns the modulo of two integers.
*/
template <typename FIRST, typename SECOND>
inline FIRST Modulo(const FIRST input, const SECOND ceiling)
{
	return (input >= ceiling) ? input % ceiling : input;
}

/*
*	Given arbitrary data and a length of the data, specified in bytes, returns a 64-bit hashed value.
*	Uses the Catalyst Engine's own hashing algorithm.
*/
inline uint64 CatalystHash(const char *const data, const uint64 length)
{
	//Defines the table of randomly pregenerated 64-bit values that is used in the mixing step.
	constexpr uint64 randomTable[UINT8_MAXIMUM]
	{
		static_cast<uint64>(17560930965631933617U),
		static_cast<uint64>(9878389245448494471U),
		static_cast<uint64>(7123207422871460316U),
		static_cast<uint64>(6698490550206600215U),
		static_cast<uint64>(9237603597315756049U),
		static_cast<uint64>(12722304790779024227U),
		static_cast<uint64>(10168806444686865565U),
		static_cast<uint64>(16285218618110221225U),
		static_cast<uint64>(14127781781355445864U),
		static_cast<uint64>(16706438048241833235U),
		static_cast<uint64>(14464401887428085303U),
		static_cast<uint64>(16579578339588149880U),
		static_cast<uint64>(6942338271136362943U),
		static_cast<uint64>(16245062713127040960U),
		static_cast<uint64>(5999197467307359585U),
		static_cast<uint64>(5917387243795852351U),
		static_cast<uint64>(6567527800446256564U),
		static_cast<uint64>(5761816604579013021U),
		static_cast<uint64>(17400772023355560990U),
		static_cast<uint64>(5273102312404278468U),
		static_cast<uint64>(4825053491189533542U),
		static_cast<uint64>(5904959865574041029U),
		static_cast<uint64>(4889189317970114367U),
		static_cast<uint64>(3488005562488628376U),
		static_cast<uint64>(2121092745091322887U),
		static_cast<uint64>(9059212359729980553U),
		static_cast<uint64>(4442338194285452607U),
		static_cast<uint64>(15117033522949622184U),
		static_cast<uint64>(1458304701614515527U),
		static_cast<uint64>(437080939375235345U),
		static_cast<uint64>(9713084737376650878U),
		static_cast<uint64>(12556364091623256880U),
		static_cast<uint64>(629526290968010433U),
		static_cast<uint64>(7109881675154368976U),
		static_cast<uint64>(8079449294824467805U),
		static_cast<uint64>(4210613093758892982U),
		static_cast<uint64>(16685686997581889613U),
		static_cast<uint64>(7484488847049490381U),
		static_cast<uint64>(55550918462184376U),
		static_cast<uint64>(2046934102092302774U),
		static_cast<uint64>(7878763160060843423U),
		static_cast<uint64>(15527813280429559639U),
		static_cast<uint64>(386851097423212803U),
		static_cast<uint64>(6535797858248330869U),
		static_cast<uint64>(5423499223124499027U),
		static_cast<uint64>(8425043641261968359U),
		static_cast<uint64>(1630309645080150463U),
		static_cast<uint64>(4573091028862423225U),
		static_cast<uint64>(9531278009924459613U),
		static_cast<uint64>(2902555699688352993U),
		static_cast<uint64>(5430040719693447842U),
		static_cast<uint64>(8463221362061156698U),
		static_cast<uint64>(18079439368152049660U),
		static_cast<uint64>(17481576187591344632U),
		static_cast<uint64>(15258782866438137765U),
		static_cast<uint64>(13671043426869992539U),
		static_cast<uint64>(11523974334480015284U),
		static_cast<uint64>(2242684098709626479U),
		static_cast<uint64>(1603581146017372540U),
		static_cast<uint64>(13930984894239873454U),
		static_cast<uint64>(6454235925450647192U),
		static_cast<uint64>(8042921848422548862U),
		static_cast<uint64>(17021743764326421699U),
		static_cast<uint64>(17660205641301376799U),
		static_cast<uint64>(18345960330194627008U),
		static_cast<uint64>(10739049733917709181U),
		static_cast<uint64>(5819599072956005704U),
		static_cast<uint64>(16715787726674004790U),
		static_cast<uint64>(10098674264774589280U),
		static_cast<uint64>(7335259469627022024U),
		static_cast<uint64>(5586743101090900325U),
		static_cast<uint64>(16909226101384191449U),
		static_cast<uint64>(8456344613471931049U),
		static_cast<uint64>(1702089858021400761U),
		static_cast<uint64>(10502337575256943561U),
		static_cast<uint64>(11342508755000169717U),
		static_cast<uint64>(13419610813988877866U),
		static_cast<uint64>(9145853618156203000U),
		static_cast<uint64>(15297192116476679531U),
		static_cast<uint64>(5334826950151293876U),
		static_cast<uint64>(4441628924317161225U),
		static_cast<uint64>(5595227637178392561U),
		static_cast<uint64>(11634246325195094389U),
		static_cast<uint64>(18431120865783043506U),
		static_cast<uint64>(6433708343870734073U),
		static_cast<uint64>(11923891671214093118U),
		static_cast<uint64>(17292019166095057658U),
		static_cast<uint64>(3903761957218636057U),
		static_cast<uint64>(7571174660802980829U),
		static_cast<uint64>(16336788816291233552U),
		static_cast<uint64>(1616053025093478843U),
		static_cast<uint64>(14221530156006716009U),
		static_cast<uint64>(8885873772994714320U),
		static_cast<uint64>(1503468572789489100U),
		static_cast<uint64>(7932692404711885073U),
		static_cast<uint64>(3023380487341274199U),
		static_cast<uint64>(6027865223932014067U),
		static_cast<uint64>(14618944186979899246U),
		static_cast<uint64>(7374149712926027258U),
		static_cast<uint64>(9899615458023758499U),
		static_cast<uint64>(15645626340599618779U),
		static_cast<uint64>(13038828797915336173U),
		static_cast<uint64>(3343292409014951839U),
		static_cast<uint64>(12284007219927916801U),
		static_cast<uint64>(7623155688078502817U),
		static_cast<uint64>(10130196601614046266U),
		static_cast<uint64>(139524192823030337U),
		static_cast<uint64>(13982651473474441254U),
		static_cast<uint64>(8458957429409283487U),
		static_cast<uint64>(3150880015742192015U),
		static_cast<uint64>(16491526491584716781U),
		static_cast<uint64>(441383822570780277U),
		static_cast<uint64>(14395947883029623279U),
		static_cast<uint64>(16176729819131575022U),
		static_cast<uint64>(12631400571397848932U),
		static_cast<uint64>(477696502127384991U),
		static_cast<uint64>(18111543051088346010U),
		static_cast<uint64>(7301110232659242906U),
		static_cast<uint64>(5021042578144855442U),
		static_cast<uint64>(5559965715883244721U),
		static_cast<uint64>(4818369469476311287U),
		static_cast<uint64>(5673267995010496719U),
		static_cast<uint64>(3131441627842406220U),
		static_cast<uint64>(3157805480291616952U),
		static_cast<uint64>(12468739066043259038U),
		static_cast<uint64>(14520272622262618344U),
		static_cast<uint64>(12200478646907301935U),
		static_cast<uint64>(17972731949073123118U),
		static_cast<uint64>(6363439102129774495U),
		static_cast<uint64>(3029430450490758832U),
		static_cast<uint64>(9193354786883952988U),
		static_cast<uint64>(231613257057551906U),
		static_cast<uint64>(5699602898195495711U),
		static_cast<uint64>(13152013893290532427U),
		static_cast<uint64>(16969264609051194218U),
		static_cast<uint64>(15781363309394362734U),
		static_cast<uint64>(16499644598354659592U),
		static_cast<uint64>(4134221085285872712U),
		static_cast<uint64>(5768377975636145591U),
		static_cast<uint64>(11343724794510637935U),
		static_cast<uint64>(9516506914304647632U),
		static_cast<uint64>(16244724826618111628U),
		static_cast<uint64>(2938660290701361526U),
		static_cast<uint64>(270373338765637592U),
		static_cast<uint64>(16240695788612074641U),
		static_cast<uint64>(10688811985406375394U),
		static_cast<uint64>(13194179790656822243U),
		static_cast<uint64>(795896873785436423U),
		static_cast<uint64>(12278686837318321663U),
		static_cast<uint64>(16062393251605163499U),
		static_cast<uint64>(15316711656562544220U),
		static_cast<uint64>(11256582052759458820U),
		static_cast<uint64>(14069231963854362226U),
		static_cast<uint64>(10099326549293633216U),
		static_cast<uint64>(3285504040772588683U),
		static_cast<uint64>(4083196111133406086U),
		static_cast<uint64>(9357152923138465588U),
		static_cast<uint64>(2740055713841083069U),
		static_cast<uint64>(7596912462954479253U),
		static_cast<uint64>(15808642734361812210U),
		static_cast<uint64>(14672499501134105388U),
		static_cast<uint64>(3185000875032109346U),
		static_cast<uint64>(7981343786475614476U),
		static_cast<uint64>(14976414755787503311U),
		static_cast<uint64>(13760825524207499899U),
		static_cast<uint64>(4802442425895954020U),
		static_cast<uint64>(11184182672562818620U),
		static_cast<uint64>(11930758450393804920U),
		static_cast<uint64>(15385392137856142917U),
		static_cast<uint64>(14360222137396784768U),
		static_cast<uint64>(5021000807680764291U),
		static_cast<uint64>(8147842345265562422U),
		static_cast<uint64>(683328664264105926U),
		static_cast<uint64>(17032902170503547258U),
		static_cast<uint64>(2315611460452587777U),
		static_cast<uint64>(16209669469918845303U),
		static_cast<uint64>(12238257316779748139U),
		static_cast<uint64>(16492927976952858421U),
		static_cast<uint64>(11746954235584597083U),
		static_cast<uint64>(17448698143922352388U),
		static_cast<uint64>(10224459365132228068U),
		static_cast<uint64>(15610780510487898405U),
		static_cast<uint64>(555397559573470958U),
		static_cast<uint64>(3200040278766167449U),
		static_cast<uint64>(13621731200257105059U),
		static_cast<uint64>(13658911007573174888U),
		static_cast<uint64>(4947041075210717727U),
		static_cast<uint64>(15664614996667553260U),
		static_cast<uint64>(16262648131803621244U),
		static_cast<uint64>(13675858870192689714U),
		static_cast<uint64>(5387394292628478745U),
		static_cast<uint64>(2431913351323671724U),
		static_cast<uint64>(10385359613925650346U),
		static_cast<uint64>(5027019375344491316U),
		static_cast<uint64>(16999489862911890799U),
		static_cast<uint64>(14327761695570564117U),
		static_cast<uint64>(15906911084456499735U),
		static_cast<uint64>(2310744715197719042U),
		static_cast<uint64>(16817715175396358130U),
		static_cast<uint64>(7788773128024837938U),
		static_cast<uint64>(16346007038775322793U),
		static_cast<uint64>(9574699190750158950U),
		static_cast<uint64>(11094202584864470966U),
		static_cast<uint64>(5485529148679679365U),
		static_cast<uint64>(12073209595404636942U),
		static_cast<uint64>(6804300515681679052U),
		static_cast<uint64>(15646963769387501335U),
		static_cast<uint64>(13231633774834277183U),
		static_cast<uint64>(3977649630609396415U),
		static_cast<uint64>(9007942203254162378U),
		static_cast<uint64>(18347901722589457699U),
		static_cast<uint64>(14803917054395996681U),
		static_cast<uint64>(6842498892724525893U),
		static_cast<uint64>(11370231912640114008U),
		static_cast<uint64>(16293070536396496206U),
		static_cast<uint64>(10813581543748995520U),
		static_cast<uint64>(8924463008898768742U),
		static_cast<uint64>(299017367097661342U),
		static_cast<uint64>(13681999915919429722U),
		static_cast<uint64>(14676538330499685604U),
		static_cast<uint64>(15855402473737452315U),
		static_cast<uint64>(10735514207718494340U),
		static_cast<uint64>(17586422898649520138U),
		static_cast<uint64>(6843651765758058097U),
		static_cast<uint64>(9617216232026421895U),
		static_cast<uint64>(10399277233902024095U),
		static_cast<uint64>(12736377535140326080U),
		static_cast<uint64>(13371140394543587478U),
		static_cast<uint64>(15735123989542841806U),
		static_cast<uint64>(2047191309885259279U),
		static_cast<uint64>(7409861310296529474U),
		static_cast<uint64>(9990111466832781446U),
		static_cast<uint64>(13624044273613610664U),
		static_cast<uint64>(3474038971823852255U),
		static_cast<uint64>(5664058380916814672U),
		static_cast<uint64>(7259945411997285368U),
		static_cast<uint64>(17451065295276712581U),
		static_cast<uint64>(1966762466573869896U),
		static_cast<uint64>(3103304002335441231U),
		static_cast<uint64>(16174273197300480942U),
		static_cast<uint64>(15886003863429191327U),
		static_cast<uint64>(8923646403357828608U),
		static_cast<uint64>(3647356717218173353U),
		static_cast<uint64>(11380199030156496062U),
		static_cast<uint64>(7118193621894595080U),
		static_cast<uint64>(3004255291799261373U),
		static_cast<uint64>(9069001090692446168U),
		static_cast<uint64>(11151500216389048467U),
		static_cast<uint64>(11436382684436066291U),
		static_cast<uint64>(11558553152582577279U),
		static_cast<uint64>(7088991897746320725U),
		static_cast<uint64>(240062035296455618U),
		static_cast<uint64>(4719602276927472541U),
		static_cast<uint64>(5211034984813859333U),
		static_cast<uint64>(8501398579569192760U)
	};

	//Set up the hash's initial value.
	uint64 hash{ 0 };

	//For each input byte, mutate the hash.
	for (uint64 i = 0; i < length; ++i)
	{
		const byte input{ static_cast<byte>(data[i]) };

		//Fill the hash bits up with data at the "beginning" and then shift those bits towards the "end",
		//filling up the "beginning" with different bits based on the input byte.
		hash += input;
		hash = hash << 3;
		hash += input ^ UINT8_MAXIMUM;
		hash = hash << 5;
		hash += hash / input;
		hash = hash << 7;
		hash += hash * input;

		//Shift the bits of the input byte a random number of steps to the left
		//in the range of 0-64 and flip the bits of the hash at those positions.
		hash ^= (static_cast<uint64>(input) << (input / 5));
		hash ^= (static_cast<uint64>(input) << ((input ^ UINT8_MAXIMUM) / 6));
		hash ^= (static_cast<uint64>(input ^ UINT8_MAXIMUM) << (input / 7));
		hash ^= (static_cast<uint64>(input ^ UINT8_MAXIMUM) << ((input ^ UINT8_MAXIMUM) / 8));

		//Use the input byte, as well as the current hash forced into a 0-256 range,
		//as an index into the table of randomly pregenerated 64-bit values,
		//and use that value to flip certain bits of the hash.
		hash ^= randomTable[input];
		hash ^= randomTable[Modulo(hash, UINT8_MAXIMUM)];
		hash ^= randomTable[input ^ UINT8_MAXIMUM];
		hash ^= randomTable[Modulo(hash, UINT8_MAXIMUM)];
	}

	return hash;
}

/*
*	Given arbitrary data and a length of the data, specified in bytes, returns a 64-bit hashed value.
*	Uses the Jenkins hashing algorithm.
*/
inline uint64 MurmurHash64(const void *const input, const uint64 length, const uint64 seed = 0)
{
	constexpr uint64 M{ 0xc6a4a7935bd1e995 };
	constexpr int32 R{ 47 };

	uint64_t hash{ seed ^ (length * M) };

	const uint64_t* data{ (const uint64_t*)input };
	const uint64_t* end{ data + (length / 8) };

	while (data != end)
	{
		uint64_t k{ *(data++) };

		k *= M;
		k ^= k >> R;
		k *= M;

		hash ^= k;
		hash *= M;
	}

	const unsigned char* data_2{ (const unsigned char*)data };

	switch (length & 7)
	{
	case 7: hash ^= uint64_t(data_2[6]) << 48;
	case 6: hash ^= uint64_t(data_2[5]) << 40;
	case 5: hash ^= uint64_t(data_2[4]) << 32;
	case 4: hash ^= uint64_t(data_2[3]) << 24;
	case 3: hash ^= uint64_t(data_2[2]) << 16;
	case 2: hash ^= uint64_t(data_2[1]) << 8;
	case 1: hash ^= uint64_t(data_2[0]);
		hash *= M;
	};

	hash ^= hash >> R;
	hash *= M;
	hash ^= hash >> R;

	return hash;
}