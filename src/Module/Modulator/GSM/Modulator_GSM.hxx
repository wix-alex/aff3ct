#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "Tools/Math/matrix.h"

#include "../../Encoder/RSC/Encoder_RSC3_CPE_sys.hpp"

#include "Modulator_GSM.hpp"

// translation of binary_symbols
template <typename B, typename R, typename Q, proto_max<Q> MAX>
const std::vector<int> Modulator_GSM<B,R,Q,MAX>::BCJR_binary_symbols = {
	0, 1
};

// translation of trellis
template <typename B, typename R, typename Q, proto_max<Q> MAX>
const std::vector<int> Modulator_GSM<B,R,Q,MAX>::BCJR_trellis = {
	0,  1,  1,  0,  2,  3,  3,  2,  4,  5,  5,  4,  6,  7,  7,  6, 
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
};

// translation of anti_trellis
template <typename B, typename R, typename Q, proto_max<Q> MAX>
const std::vector<int> Modulator_GSM<B,R,Q,MAX>::BCJR_anti_trellis = {
	0,  3,  1,  2,  4,  7,  5,  6,  0,  3,  1,  2,  4,  7,  5,  6, 
	0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1
};

template <typename B, typename R, typename Q, proto_max<Q> MAX>
Modulator_GSM<B,R,Q,MAX>
::Modulator_GSM(const int N, const R sigma, const std::string name, const bool tbless)
: Modulator<B,R,Q>(N, 
                   (tbless ? N : (N +6)) *  5 * 2, // up_sample_factor =  5
                   (tbless ? N : (N +6)) * 16,     // n_output_symbs   = 16
                   1,
                   name),
  sigma(sigma),
  parity_enc(N +6),
  BCJR((tbless ? N : N +6),
       BCJR_n_states,
       BCJR_m_order,
       BCJR_n_bits_per_symb,
       BCJR_n_prev_branches,
       BCJR_binary_symbols,
       BCJR_trellis,
       BCJR_anti_trellis)
{
}

template <typename B, typename R, typename Q, proto_max<Q> MAX>
Modulator_GSM<B,R,Q,MAX>
::~Modulator_GSM()
{
}

template <typename B, typename R, typename Q, proto_max<Q> MAX>
int Modulator_GSM<B,R,Q,MAX>
::get_buffer_size_after_modulation(const int N)
{
	// +6: tails bit
	// *up_sample_factor: because work with waveforms
	// *2: because of complex numbers
	return (N + this->n_frames * 6) * up_sample_factor * 2;
}

template <typename B, typename R, typename Q, proto_max<Q> MAX>
int Modulator_GSM<B,R,Q,MAX>
::get_buffer_size_after_filtering(const int N)
{
	return (N + this->n_frames * 6) * n_output_symbs;
}

// translation of base band vectors (80 complex elmts)
template <typename B, typename R, typename Q, proto_max<Q> MAX>
const mipp::vector<R> Modulator_GSM<B,R,Q,MAX>::baseband = {
	// real part:
	(R)-0.00564198672890523552497787918014, (R) 0.00564198672890511322697282281524, (R)-0.52091994211246372969981166534126,
	(R) 0.52091994211246384072211412785691, (R)-0.52091994211246317458829935276299, (R) 0.52091994211246339663290427779430,
	(R)-0.00564198672890548098834972989835, (R) 0.00564198672890560328635478626325, (R)-0.00564198672890523552497787918014,
	(R) 0.00564198672890511322697282281524, (R)-0.52091994211246372969981166534126, (R) 0.52091994211246384072211412785691,
	(R)-0.52091994211246317458829935276299, (R) 0.52091994211246339663290427779430, (R)-0.00564198672890548098834972989835,
	(R) 0.00564198672890560328635478626325, (R)-0.00776877723203855691941344119300, (R) 0.00776877723203887871061823489072,
	(R)-0.28757227949570324243566687982820, (R) 0.28757227949570335345796934234386, (R)-0.78729088687072712460235379694495,
	(R) 0.78729088687072712460235379694495, (R) 0.57035903678899979585281698746257, (R)-0.57035903678899968483051452494692,
	(R)-0.02137400816179174656284089905967, (R) 0.02137400816179184717680250571448, (R)-0.27451404512079868869278698184644,
	(R) 0.27451404512079879971508944436209, (R)-0.79560744046001319329519674283802, (R) 0.79560744046001286022828935529105,
	(R) 0.58148244165252382931186048153904, (R)-0.58148244165252371828955801902339, (R)-0.00834221760492034544731687617514,
	(R) 0.00834221760492066810588340786126, (R)-0.13452830303649640497631878588436, (R) 0.13452830303649651599862124840001,
	(R)-0.97383659537451217058645625002100, (R) 0.97383659537451217058645625002100, (R) 0.93140875618689789039450488417060,
	(R)-0.93140875618689789039450488417060, (R)-0.05833956743729826893707368640207, (R) 0.05833956743729836608158834110327,
	(R)-0.08480504399984668817413790975479, (R) 0.08480504399984681307422818008490, (R)-0.98398270811805732538601887426921,
	(R) 0.98398270811805732538601887426921, (R) 0.94844553546340304439610235931468, (R)-0.94844553546340293337379989679903,
	(R)-0.00858018935547101568861450004988, (R) 0.00858018935547133661245755575919, (R)-0.05270696031830793965022863289960,
	(R) 0.05270696031830806455031890322971, (R)-0.97253937780659382994485895324033, (R) 0.97253937780659382994485895324033,
	(R) 0.98497264802923345516916242559091, (R)-0.98497264802923345516916242559091, (R)-0.14011593487553636316889082991111,
	(R) 0.14011593487553603010198344236414, (R) 0.07918383214916917267611751185541, (R)-0.07918383214916904777602724152530,
	(R)-0.93344722234109755998332502713311, (R) 0.93344722234109778202792995216441, (R) 0.95367293175296929952367008809233,
	(R)-0.95367293175296929952367008809233, (R)-0.00915443117019493039776545373343, (R) 0.00915443117019525305633198541955,
	(R)-0.01573363938574873222009387063736, (R) 0.01573363938574885365073718901385, (R)-0.78380065831311762636346429644618,
	(R) 0.78380065831311740431885937141487, (R) 0.79901259618196074363538627949310, (R)-0.79901259618195974443466411685222,
	(R)-0.29297109943652965835525492366287, (R) 0.29297109943653021346676723624114, (R) 0.26908606242929095087035307187762,
	(R)-0.26908606242929083984805060936196, (R)-0.57498371378513679630373189866077, (R) 0.57498371378513690732603436117643,
	(R) 0.59516671290393219351244624704123, (R)-0.59516671290393230453474870955688,
	// imag part:
	(R) 0.99998408386621373544755897455616, (R)-0.99998408386621373544755897455616, (R)-0.85360553765164115525010402052430,
	(R) 0.85360553765164115525010402052430, (R) 0.85360553765164148831701140807127, (R)-0.85360553765164137729470894555561,
	(R)-0.99998408386621373544755897455616, (R) 0.99998408386621373544755897455616, (R) 0.99998408386621373544755897455616,
	(R)-0.99998408386621373544755897455616, (R)-0.85360553765164115525010402052430, (R) 0.85360553765164115525010402052430,
	(R) 0.85360553765164148831701140807127, (R)-0.85360553765164137729470894555561, (R)-0.99998408386621373544755897455616,
	(R) 0.99998408386621373544755897455616, (R) 0.99996982259482158905683490957017, (R)-0.99996982259482158905683490957017,
	(R)-0.95775893838984615946685607923428, (R) 0.95775893838984615946685607923428, (R) 0.61658175406859383471669389109593,
	(R)-0.61658175406859394573899635361158, (R)-0.82139550105483560837882350824657, (R) 0.82139550105483560837882350824657,
	(R) 0.99977154979280125068896722950740, (R)-0.99977154979280125068896722950740, (R)-0.96158309002988195945960114841000,
	(R) 0.96158309002988195945960114841000, (R) 0.60581251281619008519641056409455, (R)-0.60581251281619052928562041415717,
	(R)-0.81355895302665015567100681437296, (R) 0.81355895302665026669330927688861, (R) 0.99996520309730385633883997797966,
	(R)-0.99996520309730385633883997797966, (R)-0.99090975153245952622427239475655, (R) 0.99090975153245941520196993224090,
	(R) 0.22724939055887161121205508607090, (R)-0.22724939055887172223435754858656, (R)-0.36397490146763655793904490565183,
	(R) 0.36397490146763666896134736816748, (R) 0.99829679698535989107455179691897, (R)-0.99829679698535989107455179691897,
	(R)-0.99639756348165764432422975005466, (R) 0.99639756348165764432422975005466, (R) 0.17826393388639785730909181893367,
	(R)-0.17826393388639799608696989707823, (R)-0.31694016195417507031351078694570, (R) 0.31694016195417518133581324946135,
	(R) 0.99996318949780560458862055384088, (R)-0.99996318949780560458862055384088, (R)-0.99861002214778737950950926460791,
	(R) 0.99861002214778737950950926460791, (R)-0.23273839093618237106042556661123, (R) 0.23273839093618267637175733852928,
	(R) 0.17271040105992410684798699094245, (R)-0.17271040105992399582568452842679, (R) 0.99013510431352469431942608935060,
	(R)-0.99013510431352480534172855186625, (R)-0.99686003065935602229785672534490, (R) 0.99686003065935602229785672534490,
	(R)-0.35871476566164589572238696746354, (R) 0.35871476566164534061087465488527, (R) 0.30084537430663010892928355133336,
	(R)-0.30084537430662999790698108881770, (R) 0.99995809731705764811238168476848, (R)-0.99995809731705764811238168476848,
	(R)-0.99987621863492648355276060101460, (R) 0.99987621863492648355276060101460, (R)-0.62101250231208993124454309509019,
	(R) 0.62101250231209015328914802012150, (R) 0.60131428649464413460634659713833, (R)-0.60131428649464535585167368481052,
	(R) 0.95612129716629101761071751752752, (R)-0.95612129716629090658841505501186, (R)-0.96311613578337462726608464436140,
	(R) 0.96311613578337462726608464436140, (R)-0.81816485434284691447004433939583, (R) 0.81816485434284680344774187688017,
	(R) 0.80360225475736957179151431773789, (R)-0.80360225475736957179151431773789
};

template <typename B, typename R, typename Q, proto_max<Q> MAX>
void Modulator_GSM<B,R,Q,MAX>
::modulate(const mipp::vector<B>& X_N1, mipp::vector<R>& X_N2)
{
	assert(((X_N1.size() + this->n_frames * 6) * up_sample_factor * 2) == X_N2.size());

	if (this->n_frames == 1)
	{
		_modulate(X_N1, X_N2);
	}
	else // more than 1 frame
	{
		mipp::vector<B> X_N1_tmp(this->N);
		mipp::vector<R> X_N2_tmp((this->N +6) * up_sample_factor * 2);
		for (auto f = 0; f < this->n_frames; f++)
		{
			std::copy(X_N1.begin() + f * this->N, X_N1.begin() + (f +1) * this->N, X_N1_tmp.begin());

			_modulate(X_N1_tmp, X_N2_tmp);

			std::copy(X_N2_tmp.begin(), X_N2_tmp.end(), X_N2.begin() + f * ((this->N +6) * up_sample_factor * 2));
		}
	}
}

template <typename B, typename R, typename Q, proto_max<Q> MAX>
void Modulator_GSM<B,R,Q,MAX>
::_modulate(const mipp::vector<B>& X_N1, mipp::vector<R>& X_N2)
{
	assert(((X_N1.size() +6) * up_sample_factor * 2) == X_N2.size());

	// bit mapping -> not done here

	// Rimoldi phase tilting -> for GSM only consists in transforming -1/+1 in 0/1

	// CPE encoding
	Encoder_RSC3_CPE_sys<B> enc((int)X_N1.size(), (int)X_N1.size() * 2, 1, false);
	enc.encode_sys(X_N1, this->parity_enc);

	// memoryless modulation (attributing complex waveforms to symbols)
	const auto off_X_r  = 0;
	const auto off_X_i  = (X_N1.size() +6) * up_sample_factor;
	const auto off_BB_r = 0;
	const auto off_BB_i = (int)(baseband.size() / 2);

	const auto loop_size = X_N1.size() +6;
	for (auto i = 0; i < (int)loop_size; i++)
		for (auto j = 0; j < up_sample_factor; j++)
		{
			X_N2[off_X_r + i * up_sample_factor + j] = baseband[off_BB_r + j * n_output_symbs + this->parity_enc[i]];
			X_N2[off_X_i + i * up_sample_factor + j] = baseband[off_BB_i + j * n_output_symbs + this->parity_enc[i]];
		}
}

// translation of filtering generator familly (80 complex elmts)
template <typename B, typename R, typename Q, proto_max<Q> MAX>
const mipp::vector<R> Modulator_GSM<B,R,Q,MAX>::projection = {
	// real part:
	(R)-0.00132029697684193014425857448657, (R)-0.00058505123686783551927026447714, (R)-0.00339468397069032836593782676005,
	(R)-0.00040650650234961400140976106599, (R)-0.00219078471377524104310441366295, (R) 0.00132029697684190217184252436056,
	(R) 0.00058505123686791336498624893636, (R) 0.00339468397069037433610994014543, (R) 0.00040650650234968924504053155999,
	(R) 0.00219078471377530305946867983380, (R)-0.10401748554092599130527219131181, (R)-0.05837574642016117587584034254178,
	(R)-0.02534602518731315576316909243815, (R)-0.01173906302348584791694907636384, (R)-0.00281456991227095219484932719922,
	(R) 0.10401748554092603293863561475519, (R) 0.05837574642016116893694643863455, (R) 0.02534602518731318004929775611345,
	(R) 0.01173906302348587740724816796956, (R) 0.00281456991227097778202059785713, (R)-0.10419838646349323385553731213804,
	(R)-0.15737182038912753134773936380952, (R)-0.19493752797443592017501146074210, (R)-0.19436919790676290831221706412180,
	(R)-0.15680021941424224229955086684640, (R) 0.10419838646349324773332511995250, (R) 0.15737182038912755910331497943844,
	(R) 0.19493752797443592017501146074210, (R) 0.19436919790676288055664144849288, (R) 0.15680021941424218678839963558858,
	(R)-0.00110369058882891146397553505665, (R) 0.11406293921541321711998762111762, (R) 0.18615930726484100632411866627081,
	(R) 0.19716837111164342566382856603013, (R) 0.15973801222277886346923025939759, (R) 0.00110369058882893249749768127543,
	(R)-0.11406293921541320324219981330316, (R)-0.18615930726484103407969428189972, (R)-0.19716837111164342566382856603013,
	(R)-0.15973801222277864142462533436628, (R)-0.00141366279319429452576706296441, (R)-0.00285994240979238478761459418820,
	(R)-0.01415919085618501035983562985621, (R)-0.02615087290929190427801742657721, (R)-0.05910515553292910312377372861192,
	(R) 0.00141366279319428541846881408617, (R) 0.00285994240979233101118683890718, (R) 0.01415919085618514219881980409355,
	(R) 0.02615087290929176550013934843264, (R) 0.05910515553292924190165180675649, (R)-0.10392385970452529264651531093477,
	(R)-0.05622480885151811408340094544656, (R)-0.01459459897167380768023825510227, (R) 0.01403539744041460562984369175865,
	(R) 0.05431356462776571608630504783832, (R) 0.10392385970452532040209092656369, (R) 0.05622480885151812102229484935378,
	(R) 0.01459459897167383370109039475437, (R)-0.01403539744041458481316198003697, (R)-0.05431356462776569526962333611664,
	(R)-0.10381139199392126759580179395925, (R)-0.16067748314182234148361771985947, (R)-0.19441140682922075644256665327703,
	(R)-0.18827180589878331207209782860446, (R)-0.11460877603955207937325155853614, (R) 0.10381139199392130922916521740262,
	(R) 0.16067748314182225821689087297273, (R) 0.19441140682922078419814226890594, (R) 0.18827180589878333982767344423337,
	(R) 0.11460877603955207937325155853614, (R)-0.00148861390352549114585167799873, (R) 0.11790671198601079816548065082316,
	(R) 0.18706804997001283408231131488719, (R) 0.19257748293331786171300734622491, (R) 0.11855691706085519365743152775394,
	(R) 0.00148861390352552540664032854068, (R)-0.11790671198601074265432941956533, (R)-0.18706804997001277857116008362937,
	(R)-0.19257748293331783395743173059600, (R)-0.11855691706085520753521933556840,
	// imag part:
	(R)-0.20022994699815499508588345634053, (R)-0.19930212538608207961665641505533, (R)-0.20064714980915868136612800753937,
	(R)-0.19983533762996610883000414560229, (R)-0.19996481148887482182097130589682, (R) 0.20022994699815499508588345634053,
	(R) 0.19930212538608207961665641505533, (R) 0.20064714980915868136612800753937, (R) 0.19983533762996610883000414560229,
	(R) 0.19996481148887482182097130589682, (R) 0.17094374945445667268728584531345, (R) 0.19087396185449426178060150505189,
	(R) 0.19885754516415377057825253359624, (R) 0.19952120033548864652672705233272, (R) 0.19996601402686037252109940709488,
	(R)-0.17094374945445667268728584531345, (R)-0.19087396185449428953617712068080, (R)-0.19885754516415371506710130233841,
	(R)-0.19952120033548867428230266796163, (R)-0.19996601402686037252109940709488, (R)-0.17075199660761980480216948308225,
	(R)-0.12321342678018226690639380649372, (R)-0.04557004002377239987309209823252, (R) 0.04660179542834700511244605536376,
	(R) 0.12419528167265808660246761974122, (R) 0.17075199660761977704659386745334, (R) 0.12321342678018232241754503775155,
	(R) 0.04557004002377246926203113730480, (R)-0.04660179542834712307364242178664, (R)-0.12419528167265811435804323537013,
	(R) 0.19991334030230489826607254144619, (R) 0.16462218757988547612747254333954, (R) 0.07227703165261167850275114687975,
	(R)-0.03420387753798159041762616539017, (R)-0.12034449819846976259540838327666, (R)-0.19991334030230492602164815707511,
	(R)-0.16462218757988550388304815896845, (R)-0.07227703165261160911381210780746, (R) 0.03420387753798150715089931850343,
	(R) 0.12034449819847002627337673175134, (R)-0.20031873203795824700534922158113, (R)-0.19901969626463703044727537871950,
	(R)-0.20050097565137225963383116322802, (R)-0.19787101498833303825364282602095, (R)-0.19116743224502591447233612598211,
	(R) 0.20031873203795824700534922158113, (R) 0.19901969626463703044727537871950, (R) 0.20050097565137228738940677885694,
	(R) 0.19787101498833312152036967290769, (R) 0.19116743224502588671676051035320, (R) 0.17104582243181359890726866979094,
	(R) 0.19134568439589577648618501370947, (R) 0.20021232807485411786529994060402, (R) 0.19913131203074838082578423836821,
	(R) 0.19259287894233698135160182118852, (R)-0.17104582243181359890726866979094, (R)-0.19134568439589577648618501370947,
	(R)-0.20021232807485411786529994060402, (R)-0.19913131203074838082578423836821, (R)-0.19259287894233698135160182118852,
	(R)-0.17058670284035754027840425806062, (R)-0.12129395515095907354119475485277, (R)-0.03607130478167273196499209575450,
	(R) 0.07244269184042320675054327239195, (R) 0.16336166612556776711606687513267, (R) 0.17058670284035754027840425806062,
	(R) 0.12129395515095930946358748769853, (R) 0.03607130478167259318711401760993, (R)-0.07244269184042299858372615517510,
	(R)-0.16336166612556776711606687513267, (R) 0.19976310701392277491272864153871, (R) 0.16323402056012617511449036555859,
	(R) 0.06324205730973511685455434871983, (R)-0.06051781170779292617911693241695, (R)-0.16052965288872450555501814051240,
	(R)-0.19976310701392277491272864153871, (R)-0.16323402056012620287006598118751, (R)-0.06324205730973513073234215653429,
	(R) 0.06051781170779291230132912460249, (R) 0.16052965288872450555501814051240
};

template <typename B, typename R, typename Q, proto_max<Q> MAX>
void Modulator_GSM<B,R,Q,MAX>
::filter(const mipp::vector<R>& Y_N1, mipp::vector<R>& Y_N2)
{
	// /16 because 16 modulated symbols in GSM
	// *2  because we only keep real part here
	assert(Y_N1.size() / up_sample_factor == 2 * (Y_N2.size() / 16));

	if (this->n_frames == 1)
	{
		_filter(Y_N1, Y_N2);
	}
	else // mote than 1 frame
	{
		mipp::vector<R> Y_N1_tmp((this->N +6) * up_sample_factor * 2);
		mipp::vector<R> Y_N2_tmp((this->N +6) * n_output_symbs      );

		for (auto f = 0; f < this->n_frames; f++)
		{
			std::copy(Y_N1.begin() + (f +0) * (this->N +6) * up_sample_factor * 2, 
			          Y_N1.begin() + (f +1) * (this->N +6) * up_sample_factor * 2,
			          Y_N1_tmp.begin());

			_filter(Y_N1_tmp, Y_N2_tmp);

			std::copy(Y_N2_tmp.begin(), Y_N2_tmp.end(), Y_N2.begin() + f * (this->N +6) * n_output_symbs);
		}
	}
}

template <typename B, typename R, typename Q, proto_max<Q> MAX>
void Modulator_GSM<B,R,Q,MAX>
::_filter(const mipp::vector<R>& Y_N1, mipp::vector<R>& Y_N2)
{
	// /16 because 16 modulated symbols in GSM
	// *2  because we only keep real part here
	assert(Y_N1.size() / up_sample_factor == 2 * (Y_N2.size() / 16));

	const int M = (int)Y_N1.size() / (2 * up_sample_factor); // number of row    in "Y_N1"
	const int N = n_output_symbs;                            // number of column in "projection"
	const int K = up_sample_factor;                          // number of column in "Y_N1"

	// perform the matrix multiplication: Y_N2 = Y_N1 * projection (keep only the real part of the complexes in Y_N2)
	cgemm_r(M, N, K, Y_N1, projection, Y_N2);
}

template <typename B, typename R, typename Q, proto_max<Q> MAX>
void Modulator_GSM<B,R,Q,MAX>
::demodulate(const mipp::vector<Q>& Y_N1, mipp::vector<Q>& Y_N2)
{
	assert(Y_N1.size() == (Y_N2.size() + this->n_frames * 6) * n_output_symbs);

	if (this->n_frames == 1)
	{
		BCJR.decode(Y_N1, Y_N2);
	}
	else // more than 1 frame
	{
		mipp::vector<Q> Y_N1_tmp((this->N +6) * n_output_symbs);
		mipp::vector<Q> Y_N2_tmp(this->N);

		for (auto f = 0; f < this->n_frames; f++)
		{
			std::copy(Y_N1.begin() + (f +0) * (this->N +6) * n_output_symbs,
			          Y_N1.begin() + (f +1) * (this->N +6) * n_output_symbs,
			          Y_N1_tmp.begin());

			BCJR.decode(Y_N1_tmp, Y_N2_tmp);

			std::copy(Y_N2_tmp.begin(), Y_N2_tmp.end(), Y_N2.begin() + f * this->N);
		}
	}
}

template <typename B, typename R, typename Q, proto_max<Q> MAX>
void Modulator_GSM<B,R,Q,MAX>
::demodulate(const mipp::vector<Q>& Y_N1, const mipp::vector<Q>& Y_N2, mipp::vector<Q>& Y_N3)
{
	assert(Y_N1.size() == (Y_N3.size() + this->n_frames * 6) * n_output_symbs);
	assert(Y_N2.size() == Y_N3.size());

	if (this->n_frames == 1)
	{
		BCJR.decode(Y_N1, Y_N2, Y_N3);
	}
	else
	{
		mipp::vector<Q> Y_N1_tmp((this->N +6) * n_output_symbs);
		mipp::vector<Q> Y_N2_tmp(this->N);
		mipp::vector<Q> Y_N3_tmp(this->N);

		for (auto f = 0; f < this->n_frames; f++)
		{
			std::copy(Y_N1.begin() + (f +0) * (this->N +6) * n_output_symbs,
			          Y_N1.begin() + (f +1) * (this->N +6) * n_output_symbs,
			          Y_N1_tmp.begin());
			std::copy(Y_N2.begin() + (f +0) * this->N, Y_N2.begin() + (f +1) * this->N, Y_N2_tmp.begin());

			BCJR.decode(Y_N1_tmp, Y_N2_tmp, Y_N3_tmp);

			std::copy(Y_N3_tmp.begin(), Y_N3_tmp.end(), Y_N3.begin() + f * this->N);
		}
	}
}
