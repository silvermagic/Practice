#include <boost/mpl/equal.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/multiplies.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_same.hpp>
#include <iostream>
#include <vector>

using namespace boost;
using namespace mpl::placeholders;

namespace chapter_3 {

typedef mpl::vector_c<int, 1, 0, 0, 0, 0, 0, 0> mass;
typedef mpl::vector_c<int, 0, 1, 0, 0, 0, 0, 0> length;
typedef mpl::vector_c<int, 0, 0, 1, 0, 0, 0, 0> time;
typedef mpl::vector_c<int, 0, 0, 0, 1, 0, 0, 0> charge;
typedef mpl::vector_c<int, 0, 0, 0, 0, 1, 0, 0> temperature;
typedef mpl::vector_c<int, 0, 0, 0, 0, 0, 1, 0> intensity;
typedef mpl::vector_c<int, 0, 0, 0, 0, 0, 0, 1> amount_of_substance;

typedef mpl::vector_c<int, 0, 1, -1, 0, 0, 0, 1> velocity;     // l/t
typedef mpl::vector_c<int, 0, 1, -2, 0, 0, 0, 1> acceleration; // l/t^2
typedef mpl::vector_c<int, 1, 1, -1, 0, 0, 0, 1> momentum;     // ml/t
typedef mpl::vector_c<int, 1, 1, -2, 0, 0, 0, 1> force;        // ml/t^2
typedef mpl::vector_c<int, 0, 0, 0, 0, 0, 0, 1> scalar;

template <class T, class D> struct quantity {
  explicit quantity(T x) : m_value(x) {}
  T value() const { return m_value; }

  template <typename U> quantity(const quantity<T, U> &x) : m_value(x.value()) {
    BOOST_STATIC_ASSERT((mpl::equal<D, U>::type::value));
  }

private:
  T m_value;
};

template <typename T, typename D>
quantity<T, D> operator+(quantity<T, D> x, quantity<T, D> y) {
  return quantity<T, D>(x.value() + y.value());
}

template <typename T, typename D>
quantity<T, D> operator-(quantity<T, D> x, quantity<T, D> y) {
  return quantity<T, D>(x.value() - y.value());
}

struct plus_f {
  template <typename T1, typename T2> struct apply {
    typedef typename mpl::plus<T1, T2>::type type;
  };
};

template <typename T, typename D1, typename D2>
quantity<T, typename mpl::transform<D1, D2, plus_f>::type>
operator*(quantity<T, D1> x, quantity<T, D2> y) {
  return quantity<T, typename mpl::transform<D1, D2, plus_f>::type>(x.value() *
                                                                    y.value());
}

template <typename T, typename U>
struct divide_dimensions : mpl::transform<T, U, mpl::minus<_1, _2>> {};

template <typename T, typename D1, typename D2>
quantity<T, typename divide_dimensions<D1, D2>::type>
operator/(quantity<T, D1> x, quantity<T, D2> y) {
  return quantity<T, typename divide_dimensions<D1, D2>::type>(x.value() /
                                                               y.value());
}

void test() {
  quantity<float, length> dist1(1.0f), dist2(2.0);
  quantity<float, mass> m(2.0f);
  quantity<float, acceleration> a(9.8f);

  auto v1 = dist1 + dist2;
  // auto v2 = dist1 + m;
  std::cout << "force = " << (m * a).value() << std::endl;
  quantity<float, force> f = m * a;
  quantity<float, mass> m2 = f / a;
}

} // namespace chapter_3

namespace chapter_3_0 {

template <unsigned long N> struct binary {
  BOOST_STATIC_ASSERT((N % 10 < 2));
  static const unsigned value = binary<N / 10>::value * 2 + N % 10;
};

template <> struct binary<0> { static const unsigned value = 0; };

void test() {
  std::cout << binary<101>::value << std::endl;
  // std::cout << binary<5>::value << std::endl;
}
} // namespace chapter_3_0

namespace chapter_3_1 {
void test() {
  std::cout << mpl::equal<mpl::transform<mpl::vector_c<int, 1, 2, 3>,
                                         mpl::vector_c<int, 1, 1, 1>,
                                         mpl::plus<_1, _2>>::type,
                          mpl::vector_c<int, 2, 3, 4>>::value
            << std::endl;
}
} // namespace chapter_3_1

namespace chapter_3_2 {
void test() {
  std::cout << mpl::equal<mpl::transform<mpl::vector_c<int, 1, 2, 3>,
                                         mpl::vector_c<int, 1, 2, 3>,
                                         mpl::multiplies<_1, _2>>::type,
                          mpl::vector_c<int, 1, 4, 9>>::value
            << std::endl;
}
} // namespace chapter_3_2

namespace chapter_3_3 {
template <typename F, typename U>
struct twice
    : mpl::apply<F,
                 typename mpl::apply<
                     F, typename mpl::apply<
                            F, typename mpl::apply<F, U>::type>::type>::type> {
};
void test() {
  std::cout
      << boost::is_same<twice<add_pointer<_1>, int>::type, int ****>::value
      << std::endl;
}
} // namespace chapter_3_3

namespace chapter_3_4 {
template <typename F, typename U>
struct twice : mpl::apply<F, typename mpl::apply<F, U>::type> {};

void test() {
  std::cout
      << boost::is_same<twice<add_pointer<_1>,
                              typename twice<add_pointer<_1>, int>::type>::type,
                        int ****>::value
      << std::endl;
}
} // namespace chapter_3_4

namespace chapter_3_5 {
using namespace chapter_3;

template <typename T, typename D, typename U>
quantity<T, D> operator+(quantity<T, D> x, quantity<T, U> y) {
  quantity<T, D> tmp = y;
  return quantity<T, D>(x.value() + tmp.value());
}

void test() {
  quantity<float, mass> m(2.0f);
  quantity<float, acceleration> a(9.8f);
  quantity<float, force> f = m * a;
  f = f + m * a;
  std::cout << "force = " << (m * a).value() << std::endl;
}
} // namespace chapter_3_5

namespace chapter_3_6 {
void test() {
  std::cout
      << boost::is_same<mpl::apply<add_pointer<_1>, int>::type, int *>::value
      << std::endl;

  std::cout << boost::is_same<
                   mpl::apply<add_pointer<_1>,
                              mpl::apply<add_pointer<_1>, int>::type>::type,
                   int **>::value
            << std::endl;

  std::cout << boost::is_same<mpl::apply<mpl::apply<_1, mpl::apply<_1, _2>>,
                                         add_pointer<_1>, int>::type,
                              int **>::value
            << std::endl;
}
} // namespace chapter_3_6

namespace chapter_3_7 {
void test() {
  // 元函数类
  typedef mpl::lambda<mpl::lambda<_1>>::type t1;
  typedef mpl::apply<t1, add_pointer<_1>>::type fpp;
  std::cout << "t1 => " << boost::is_same<fpp::apply<int>::type, int *>::value
            << std::endl;

  // Lambda表达式
  typedef mpl::apply<_1, mpl::plus<_1, _2>>::type t2;
  std::cout << "t2 => "
            << mpl::apply<t2, mpl::int_<1>, mpl::int_<2>>::type::value
            << std::endl;

  // std::vector<int>
  typedef mpl::apply<_1, std::vector<int>>::type t3;
  t3 v3{1};
  std::cout << "t3 => " << v3[0] << std::endl;

  // std::vector
  typedef mpl::apply<_1, std::vector<_1>>::type t4;
  mpl::apply<t4, float>::type v4{1.1f};
  std::cout << "t4 => " << v4[0] << std::endl;

  // std::vector<int>
  typedef mpl::apply<mpl::lambda<_1>, std::vector<int>>::type t5;
  t5 v5{1};
  std::cout << "t5 => " << v5[0] << std::endl;

  // std::vector
  typedef mpl::apply<mpl::lambda<_1>, std::vector<_1>>::type t6;
  mpl::apply<t6, float>::type v6{1.1f};
  std::cout << "t6 => " << v6[0] << std::endl;

  // 元函数类
  typedef mpl::apply<mpl::lambda<_1>, mpl::plus<_1, _2>>::type t7;
  std::cout << "t7 => " << t7::apply<mpl::int_<1>, mpl::int_<2>>::type::value
            << std::endl;

  // Labmbda表达式
  typedef mpl::apply<_1, mpl::lambda<mpl::plus<_1, _2>>>::type t8;
  std::cout << "t8 => "
            << mpl::apply<t8, mpl::int_<1>, mpl::int_<2>>::type::value
            << std::endl;
}
} // namespace chapter_3_7

void test3() {
  std::cout << "chapter 3 testing....." << std::endl;
  chapter_3::test();
  std::cout << "chapter 3-0 testing....." << std::endl;
  chapter_3_0::test();
  std::cout << "chapter 3-1 testing....." << std::endl;
  chapter_3_1::test();
  std::cout << "chapter 3-2 testing....." << std::endl;
  chapter_3_2::test();
  std::cout << "chapter 3-3 testing....." << std::endl;
  chapter_3_3::test();
  std::cout << "chapter 3-4 testing....." << std::endl;
  chapter_3_4::test();
  std::cout << "chapter 3-5 testing....." << std::endl;
  chapter_3_5::test();
  std::cout << "chapter 3-6 testing....." << std::endl;
  chapter_3_6::test();
  std::cout << "chapter 3-7 testing....." << std::endl;
  chapter_3_7::test();
}