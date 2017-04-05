#include <chrono>
#include <algorithm>
#include <stdexcept>

#include "Tools/Perf/Reorderer/Reorderer.hpp"

#include "Decoder_polar_SC_naive.hpp"

namespace aff3ct
{
namespace module
{
template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
Decoder_polar_SC_naive<B,R,F,G,H>
::Decoder_polar_SC_naive(const int& K, const int& N, const mipp::vector<B>& frozen_bits, const int n_frames,
                         const std::string name)
: Decoder<B,R>(K, N, n_frames, 1, name), m((int)std::log2(N)), frozen_bits(frozen_bits), polar_tree(m +1)
{
	if (!tools::is_power_of_2(this->N))
		throw std::invalid_argument("aff3ct::module::Decoder_polar_SC_naive: \"N\" has to be positive a power "
		                            "of 2.");

	if (this->N != (int)frozen_bits.size())
		throw std::length_error("aff3ct::module::Decoder_polar_SC_naive: \"frozen_bits.size()\" has to be equal to "
		                        "\"N\".");

	this->recursive_allocate_nodes_contents(this->polar_tree.get_root(), this->N);
	this->recursive_initialize_frozen_bits(this->polar_tree.get_root(), frozen_bits);
}

template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
Decoder_polar_SC_naive<B,R,F,G,H>
::~Decoder_polar_SC_naive()
{
	this->recursive_deallocate_nodes_contents(this->polar_tree.get_root());
}

template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
void Decoder_polar_SC_naive<B,R,F,G,H>
::_load(const R *Y_N)
{
	auto *contents = this->polar_tree.get_root()->get_contents();

	for (auto i = 0; i < this->N; i++)
		contents->lambda[i] = Y_N[i];
}

template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
void Decoder_polar_SC_naive<B,R,F,G,H>
::_hard_decode_fbf(const R *Y_N, B *V_K)
{
	auto t_load = std::chrono::steady_clock::now(); // ----------------------------------------------------------- LOAD
	this->_load(Y_N);
	auto d_load = std::chrono::steady_clock::now() - t_load;

	auto t_decod = std::chrono::steady_clock::now(); // -------------------------------------------------------- DECODE
	this->recursive_decode(this->polar_tree.get_root());
	auto d_decod = std::chrono::steady_clock::now() - t_decod;

	auto t_store = std::chrono::steady_clock::now(); // --------------------------------------------------------- STORE
	this->_store(V_K);
	auto d_store = std::chrono::steady_clock::now() - t_store;

	this->d_load_total  += d_load;
	this->d_decod_total += d_decod;
	this->d_store_total += d_store;
}

template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
void Decoder_polar_SC_naive<B,R,F,G,H>
::_store(B *V_K) const
{
	auto k = 0;
	this->recursive_store(this->polar_tree.get_root(), V_K, k);

	if (k != this->K)
		throw std::runtime_error("aff3ct::module::Decoder_polar_SC_naive: \"k\" should be equal to \"K\".");
}

template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
void Decoder_polar_SC_naive<B,R,F,G,H>
::recursive_allocate_nodes_contents(tools::Binary_node<Contents_SC<B,R>>* node_curr,
                                    const int vector_size)
{
	if (node_curr != nullptr)
	{
		node_curr->set_contents(new Contents_SC<B,R>(vector_size));

		this->recursive_allocate_nodes_contents(node_curr->get_left() , vector_size / 2);
		this->recursive_allocate_nodes_contents(node_curr->get_right(), vector_size / 2);
	}
}

template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
void Decoder_polar_SC_naive<B,R,F,G,H>
::recursive_initialize_frozen_bits(const tools::Binary_node<Contents_SC<B,R>>* node_curr,
                                   const mipp::vector<B>& frozen_bits)
{
	auto *contents = node_curr->get_contents();

	if (!node_curr->is_leaf()) // stop condition
	{
		this->recursive_initialize_frozen_bits(node_curr->get_left(),  frozen_bits); // recursive call
		this->recursive_initialize_frozen_bits(node_curr->get_right(), frozen_bits); // recursive call
	}
	else
		contents->is_frozen_bit = frozen_bits[node_curr->get_lane_id()];
}

template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
void Decoder_polar_SC_naive<B,R,F,G,H>
::recursive_decode(const tools::Binary_node<Contents_SC<B,R>>* node_curr)
{
	if (!node_curr->is_leaf()) // stop condition
	{
		const auto size = (int)node_curr->get_c()->lambda.size();
		const auto size_2 = size / 2;

		const auto *node_left  = node_curr->get_left(); // get left node
		const auto *node_right = node_curr->get_right(); // get right node

		for (auto i = 0; i < size_2; i++)
			node_left->get_c()->lambda[i] = F(node_curr->get_c()->lambda[        i],  // apply f()
			                                  node_curr->get_c()->lambda[size_2 +i]);

		this->recursive_decode(node_left); // recursive call

		for (auto i = 0; i < size_2; i++)
			node_right->get_c()->lambda[i] = G(node_curr->get_c()->lambda[        i], // apply g()
			                                   node_curr->get_c()->lambda[size_2 +i],
			                                   node_left->get_c()->s     [        i]);

		this->recursive_decode(node_right); // recursive call

		for (auto i = 0; i < size_2; i++)
			node_curr->get_c()->s[i] = node_left ->get_c()->s[i] ^ node_right->get_c()->s[i]; // bit xor

		for (auto i = 0; i < size_2; i++)
			node_curr->get_c()->s[size_2 +i] = node_right->get_c()->s[i]; // bit eq
	}
	else // specific leaf treatment
	{
		node_curr->get_c()->s[0] = (!node_curr->get_c()->is_frozen_bit && // if this is a frozen bit then s == 0
		                            H(node_curr->get_c()->lambda[0])); // apply h()
	}
}

template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
void Decoder_polar_SC_naive<B,R,F,G,H>
::recursive_store(const tools::Binary_node<Contents_SC<B,R>>* node_curr, B *V_K, int &k) const
{
	auto *contents = node_curr->get_contents();

	if (!node_curr->is_leaf()) // stop condition
	{
		this->recursive_store(node_curr->get_left(),  V_K, k); // recursive call
		this->recursive_store(node_curr->get_right(), V_K, k); // recursive call
	}
	else
		if (!frozen_bits[node_curr->get_lane_id()])
			V_K[k++] = contents->s[0];
}

template <typename B, typename R, proto_f<R> F, proto_g<B,R> G, proto_h<B,R> H>
void Decoder_polar_SC_naive<B,R,F,G,H>
::recursive_deallocate_nodes_contents(tools::Binary_node<Contents_SC<B,R>>* node_curr)
{
	if (node_curr != nullptr)
	{
		this->recursive_deallocate_nodes_contents(node_curr->get_left()); // recursive call
		this->recursive_deallocate_nodes_contents(node_curr->get_right()); // recursive call

		auto *contents = node_curr->get_contents();
		delete contents;
		node_curr->set_contents(nullptr);
	}
}
}
}
