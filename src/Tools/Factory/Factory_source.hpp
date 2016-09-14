#ifndef FACTORY_SOURCE_HPP
#define FACTORY_SOURCE_HPP

#include "Module/Source/Source.hpp"

#include "Tools/params.h"

template <typename B>
struct Factory_source
{
	static Source<B>* build(const parameters &params, const int seed = 0);
};

#endif /* FACTORY_SOURCE_HPP */
