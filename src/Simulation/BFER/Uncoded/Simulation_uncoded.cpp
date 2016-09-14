#include <string>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <algorithm>

#include "Tools/bash_tools.h"
#include "Tools/simu_tools.h"

#include "Module/Encoder/NO/Encoder_NO.hpp"
#include "Module/Decoder/NO/Decoder_NO.hpp"

#include "Simulation_uncoded.hpp"

template <typename B, typename R, typename Q>
Simulation_uncoded<B,R,Q>
::Simulation_uncoded(const parameters& params)
: Simulation_BFER<B,R,Q>(params)
{
	assert(params.code.N == params.code.K);
}

template <typename B, typename R, typename Q>
Simulation_uncoded<B,R,Q>
::~Simulation_uncoded()
{
}

template <typename B, typename R, typename Q>
void Simulation_uncoded<B,R,Q>
::launch_precompute()
{
}

template <typename B, typename R, typename Q>
void Simulation_uncoded<B,R,Q>
::snr_precompute()
{
}

template <typename B, typename R, typename Q>
Encoder<B>* Simulation_uncoded<B,R,Q>
::build_encoder(const int tid)
{
	return new Encoder_NO<B>(this->params.code.K, this->params.code.N);
}

template <typename B, typename R, typename Q>
Decoder<B,Q>* Simulation_uncoded<B,R,Q>
::build_decoder(const int tid)
{
	return new Decoder_NO<B,Q>(this->params.code.K, this->params.code.N);
}

// ==================================================================================== explicit template instantiation 
#include "Tools/types.h"
#ifdef MULTI_PREC
template class Simulation_uncoded<B_8,R_8,Q_8>;
template class Simulation_uncoded<B_16,R_16,Q_16>;
template class Simulation_uncoded<B_32,R_32,Q_32>;
template class Simulation_uncoded<B_64,R_64,Q_64>;
#else
template class Simulation_uncoded<B,R,Q>;
#endif
// ==================================================================================== explicit template instantiation
