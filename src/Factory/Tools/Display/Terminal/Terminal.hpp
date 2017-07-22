#ifndef FACTORY_TERMINAL_HPP_
#define FACTORY_TERMINAL_HPP_

#include <string>
#include <chrono>

#include "Tools/Arguments_reader.hpp"

#include "../../../Factory.hpp"

namespace aff3ct
{
namespace factory
{
struct Terminal : Factory
{
	static const std::string name;
	static const std::string prefix;

	struct parameters
	{
		std::chrono::milliseconds frequency = std::chrono::milliseconds(500);
		bool                      disabled  = false;
	};

	static void build_args(arg_map &req_args, arg_map &opt_args, const std::string p = prefix);
	static void store_args(const tools::Arguments_reader& ar, parameters& params, const std::string p = prefix);
	static void header(params_list& head_ter, const parameters& params);
};
}
}

#endif /* FACTORY_TERMINAL_HPP_ */