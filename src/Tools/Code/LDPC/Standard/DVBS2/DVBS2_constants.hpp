#ifndef DVBS2_CONSTANTS_HPP_
#define DVBS2_CONSTANTS_HPP_

#include <vector>

#include "Tools/Algo/Sparse_matrix/Sparse_matrix.hpp"

namespace aff3ct
{
namespace tools
{

struct dvbs2_values
{
	std::vector<int> EncValues;
	int N;
	int K;
	int NmK;
	int M;
	int Q;
	int N_LINES;
};

dvbs2_values* build_dvbs2(const int K, const int N);

tools::Sparse_matrix build_H(const dvbs2_values& dvbs2);

}
}

#endif // DVBS2_CONSTANTS_HPP_
