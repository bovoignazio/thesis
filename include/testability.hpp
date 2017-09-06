#ifndef TESTABILITY_HPP
#define TESTABILITY_HPP

#include "config.hpp"

#include <vector>
#include <string>
#include <functional>
#include <cmath>
#include <utility>
#include <type_traits>

#include <boost/math/tools/roots.hpp>
#include <boost/assert.hpp>

#include <range/v3/view/transform.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/adjacent_remove_if.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/numeric/inner_product.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/find_if_not.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/size.hpp>

using Frequency = unsigned;

namespace thesis
{
  // In: n1 = D1.size, n2 = D2.size, f = frequency
  decltype(auto) min_p_(const unsigned n1, const unsigned n2, const Frequency f);

  template<typename Algorithm>
  decltype(auto)
  one_pass_(Algorithm alg, const unsigned n1, const unsigned n2, const double alpha);

  template<typename Algorithm>
  decltype(auto)
  lamp_dec_(Algorithm m, const unsigned n1, const unsigned n2, const double alpha);

  template<typename Algorithm>
  decltype(auto)
  early_term_(Algorithm m_et, const unsigned n1, unsigned n2, const double alpha);

  template<typename Algorithm>
  decltype(auto)
  bis_leap_(Algorithm m_et, const unsigned n1, const unsigned n2, const double alpha);


} // namespace thesis


/**************************************************************************************/

namespace th = thesis;
// In: n1 = D1.size, n2 = D2.size, f = frequency. Requires[f <= n1 + n2]
// Out: (double) phi(f) 
decltype(auto) th::min_p_(const unsigned n1, const unsigned n2, const Frequency f)
{
  using namespace ranges;

  auto N = n1 + n2;
  BOOST_ASSERT(f < N); // precondition
  auto op2 = [](const auto a, const auto b) {
    return static_cast<double>(a) / static_cast<double>(b);};

  auto init_v = static_cast<double>(n1) / static_cast<double>(N);

  auto base1 = f > n1 ? 1u : n1 - f + 1;
  auto base2 = f > n1 ? N - n1 + 1 : N - f + 1;

  auto r1 = view::iota(base1,n1);
  auto r2 = view::iota(base2, N);
  return inner_product(r1, r2, init_v, std::multiplies<double>{}, op2);
}


template<typename Algorithm>
decltype(auto)
th::one_pass_(Algorithm alg, const unsigned n1, const unsigned n2, const double alpha) 
{
  using namespace ranges;

  auto phi = [=](const auto f){return th::min_p_(n1, n2, f);};
  auto freq_rng = view::iota(1u,n1) | view::remove_if([=](const auto f){
      return phi(f) > alpha;});
  auto min_freq = *begin(freq_rng);

  auto freqs = alg(min_freq); BOOST_ASSERT(size(freqs) > 0);

  auto map_ = view::zip(view::iota(1ul, size(freqs)), freqs)
    | view::adjacent_remove_if([](const auto& a, const auto& b){
	return std::get<1>(a) == std::get<1>(b);});

  auto pred = [=](const auto& p){
    return phi(std::get<1>(p)) * std::get<0>(p) <= alpha;};

  return std::get<1>(*find_if(map_, pred));
}

template<typename Algorithm>
decltype(auto)
th::lamp_dec_(Algorithm m, const unsigned n1, const unsigned n2, const double alpha) 
{
  using namespace ranges;
  auto freq_rng = view::iota(1u,n1) | view::reverse;
  auto phi = [=](const auto f){return th::min_p_(n1, n2, f);};
  auto pred = [=](const auto f){
    const auto m_ = m(f); BOOST_ASSERT(m > 0); // <-- very important!
    return m_ * phi(f) > alpha;};

  return *find_if(freq_rng, pred) - 1;
}

template<typename Algorithm>
decltype(auto)
th::early_term_(Algorithm m_et, const unsigned n1, const unsigned n2, const double alpha){
  using namespace ranges;
  auto phi = [=](const auto f){return th::min_p_(n1, n2, f);};
  auto pred = [=](const auto f){
    const auto pv = phi(f);
    auto m =  m_et(f, pv); BOOST_ASSERT(m > 0); // <-- very important!
    return m * pv <= alpha;
  };

  auto freq_rng = view::iota(1,n1)
    | view::remove_if([=](const auto f){return phi(f) > alpha;});

  return *find_if(freq_rng, pred);
}

template<typename Algorithm>
decltype(auto)
th::bis_leap_(Algorithm m_et, const unsigned n1, const unsigned n2, const double alpha) 
{
  using namespace ranges;
  using namespace boost::math;
    
  auto phi = [=](const auto f){return th::min_p_(n1, n2, f);};
  auto g = [=](const auto f){
    const auto pv = phi(f);
    const auto m = m_et(f, pv); BOOST_ASSERT(m > 0); // <-- very important!
    return m * pv - alpha;};

  auto freq_rng = view::iota(1u,n1)| view::remove_if([=](const auto f){
      return phi(f) > alpha;});
    
  auto min = static_cast<double>(*begin(freq_rng));
  auto max = static_cast<double>(n1);
    
  auto tol = [](const auto a, const auto b){
    return b - a <= 1;};

  auto res =  tools::bisect(g, min, max, tol);
  return std::floor(res.first);
}
// }   

#endif
