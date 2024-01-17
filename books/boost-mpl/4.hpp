#include <boost/mpl/bool_fwd.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_scalar.hpp>
#include <iostream>
#include <vector>

using namespace boost;

namespace chapter_4 {
template <typename T>
struct param_type : mpl::if_<boost::is_scalar<T>, mpl::identity<T>,
                             boost::add_reference<const T>>::type {};

struct A {};

void test() {
  std::cout << std::is_same<int, param_type<int>::type>::value << std::endl;
  std::cout << std::is_same<const A &, param_type<A>::type>::value << std::endl;
  std::cout << std::is_same<const std::vector<int> &,
                            param_type<std::vector<int>>::type>::value
            << std::endl;
}
} // namespace chapter_4

namespace chapter_4_0 {
void test() {
  std::cout
      << mpl::or_<boost::is_integral<int>, boost::is_integral<char>>::value
      << std::endl;
}
} // namespace chapter_4_0

namespace chapter_4_1 {
template <bool C_, typename T1> struct logical_or_impl : true_ {};

template <typename T1> struct logical_or_impl<true, T1> : true_ {};
template <typename T1>
struct logical_or_impl<false, T1> : logical_or_impl<T1::value, false_> {};

template <typename T1, typename T2>
struct logical_or : logical_or_impl<T1::value, T2> {};

void test() {
  if (logical_or<boost::is_integral<int>, boost::is_integral<char>>()) {
  }
}
} // namespace chapter_4_1

void test4() {
  chapter_4::test();
  chapter_4_1::test();
}