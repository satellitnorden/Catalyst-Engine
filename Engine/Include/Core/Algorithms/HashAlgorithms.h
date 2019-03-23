#pragma once

//Disable warning C4307 - "integral constant overflow"
#pragma warning(disable : 4307) 

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

namespace HashAlgorithms
{

	/*
	*	Given arbitrary data and a length of the data, specified in bytes, returns a 64-bit hashed value.
	*	Uses the Catalyst Engine's own hashing algorithm.
	*/
	static constexpr uint64 CatalystHash(const char *const RESTRICT data, const uint64 length) NOEXCEPT
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
			hash ^= randomTable[CatalystBaseMath::Modulo(hash, UINT8_MAXIMUM)];
			hash ^= randomTable[input ^ UINT8_MAXIMUM];
			hash ^= randomTable[CatalystBaseMath::Modulo(hash, UINT8_MAXIMUM)];
		}

		return hash;
	}

	/*
	*	Given arbitrary data and a length of the data, specified in bytes, returns a 64-bit hashed value.
	*	Uses the DJB2 hashing algorithm.
	*/
	static constexpr uint64 DJB2Hash(const char *const RESTRICT data, const uint64 length) NOEXCEPT
	{
		//Set up the hash's initial value.
		uint64 hash{ 5381 };

		//For each byte, mutate the hash.
		for (uint64 i = 0; i < length; ++i)
		{
			const byte byteData{ static_cast<byte>(data[i]) };

			hash = ((hash << 5) + hash) + byteData;
		}

		return hash;
	}

	/*
	*	Given arbitrary data and a length of the data, specified in bytes, returns a 64-bit hashed value.
	*	Uses the Jenkins hashing algorithm.
	*/
	static constexpr uint64 JenkinsHash(const char *const RESTRICT data, uint64 length) NOEXCEPT
	{
		//Define the mix function.
		#define mix(a,b,c)					\
		{									\
			a -= b; a -= c; a ^= (c >> 13); \
			b -= c; b -= a; b ^= (a << 8);	\
			c -= a; c -= b; c ^= (b >> 13); \
			a -= b; a -= c; a ^= (c >> 12); \
			b -= c; b -= a; b ^= (a << 16); \
			c -= a; c -= b; c ^= (b >> 5);	\
			a -= b; a -= c; a ^= (c >> 3);	\
			b -= c; b -= a; b ^= (a << 10); \
			c -= a; c -= b; c ^= (b >> 15); \
		}

		//Treat the data as a stream of bytes.
		const byte byteDataBegin{ static_cast<byte>(data[0]) };
		const byte *RESTRICT byteData{ static_cast<const byte *RESTRICT>(&byteDataBegin) };

		//Set up the hash's initial value.
		uint64 a{ 0x9e3779b };
		uint64 b{ a };
		uint64 hash{ 0 };

		while (length >= 12)
		{
			a += (byteData[0] + (static_cast<uint64>(byteData[1]) << 8) + (static_cast<uint64>(byteData[2]) << 16) + (static_cast<uint64>(byteData[3]) << 24));
			b += (byteData[4] + (static_cast<uint64>(byteData[5]) << 8) + (static_cast<uint64>(byteData[6]) << 16) + (static_cast<uint64>(byteData[7]) << 24));
			hash += (byteData[8] + (static_cast<uint64>(byteData[9]) << 8) + (static_cast<uint64>(byteData[10]) << 16) + (static_cast<uint64>(byteData[11]) << 24));

			mix(a, b, hash);

			byteData += 12;
			length -= 12;
		}

		hash += length;

		switch (length)
		{
			case 11: hash += (static_cast<uint64>(byteData[10]) << 24);
			case 10: hash += (static_cast<uint64>(byteData[9]) << 16);
			case 9: hash += (static_cast<uint64>(byteData[8]) << 8);
			case 8: b += (static_cast<uint64>(byteData[7]) << 24);
			case 7: b += (static_cast<uint64>(byteData[6]) << 16);
			case 6: b += (static_cast<uint64>(byteData[5]) << 8);
			case 5: b += byteData[4];
			case 4: a += (static_cast<uint64>(byteData[3]) << 24);
			case 3: a += (static_cast<uint64>(byteData[2]) << 16);
			case 2: a += (static_cast<uint64>(byteData[1]) << 8);
			case 1: a += byteData[0];
		}

		mix(a, b, hash);

		return hash;
	}

}