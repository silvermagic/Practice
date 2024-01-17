#include <boost/polymorphic_cast.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/add_pointer.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/conditional.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <iostream>
#include <type_traits>
#include <typeinfo>

// 2-0
namespace chapter_2_0 {
template <typename T> struct add_const_ref {
  typedef typename boost::conditional<
      boost::is_reference<T>::value, T,
      typename boost::add_reference<typename boost::add_const<T>::type>::type>::
      type type;
};

void test() {
  std::cout << std::is_same<add_const_ref<int>::type, const int &>::value
            << std::endl;
  std::cout << std::is_same<add_const_ref<int &>::type, int &>::value
            << std::endl;
  std::cout
      << std::is_same<add_const_ref<const int &>::type, const int &>::value
      << std::endl;
}
} // namespace chapter_2_0

// 2-1
namespace chapter_2_1 {
template <typename C, typename X, typename Y> struct replace_type {
  typedef C type;
};

template <typename C, typename Y> struct replace_type<C, C, Y> {
  typedef Y type;
};
template <typename C, typename Y> struct replace_type<C *, C, Y> {
  typedef Y *type;
};
template <typename C, typename Y> struct replace_type<C &, C, Y> {
  typedef Y &type;
};
template <typename C, typename Y> struct replace_type<C &&, C, Y> {
  typedef Y &&type;
};

template <typename C, typename X, typename Y> struct replace_type<C *, X, Y> {
  typedef typename replace_type<C, X, Y>::type *type;
};
template <typename C, typename X, typename Y> struct replace_type<C &, X, Y> {
  typedef typename replace_type<C, X, Y>::type &type;
};
template <typename C, typename X, typename Y>
struct replace_type<const C, X, Y> {
  typedef typename replace_type<C, X, Y>::type const type;
};
template <typename C, typename X, typename Y, size_t N>
struct replace_type<C[N], X, Y> {
  typedef typename replace_type<C, X, Y>::type type[N];
};

template <typename C, typename X, typename Y> struct replace_type<C(), X, Y> {
  typedef typename replace_type<C, X, Y>::type type();
};

template <typename C, typename X, typename Y, typename... Args>
struct replace_type<C(Args...), X, Y> {
  typedef typename replace_type<C, X, Y>::type
  type(typename replace_type<Args, X, Y>::type...);
};

void test() {
  std::cout
      << std::is_same<replace_type<int const *[10], int const, long>::type,
                      long *[10]>::value
      << std::endl;
  std::cout << std::is_same<replace_type<char (*)(int *), int, long>::type,
                            char (*)(long *)>::value
            << std::endl;
  std::cout << std::is_same<
                   replace_type<char (*)(const char *, int), char, int>::type,
                   int (*)(const int *, int)>::value
            << std::endl;
  std::cout << std::is_same<replace_type<char (*)(), char, int>::type,
                            int (*)()>::value
            << std::endl;
}
} // namespace chapter_2_1

// 2-2
namespace chapter_2_2 {
template <class Target, class Source>
inline Target polymorphic_downcast(Source *x) {
  return boost::polymorphic_downcast<Target>(x);
}
template <class Target, class Source>
inline Target &polymorphic_downcast(Source &x) {
  return *boost::polymorphic_downcast<boost::add_pointer_t<Target>>(&x);
}

struct A {
  virtual ~A() {}
};
struct B : A {};

void test() {
  B b;
  A *a_ptr = &b;
  B *b_ptr = polymorphic_downcast<B *>(a_ptr);

  A &a_ref = b;
  B &b_ref = polymorphic_downcast<B &>(a_ref);
}
} // namespace chapter_2_2

// 2-3
namespace chapter_2_3 {
template <typename... Args> struct type_descriptor {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<Args...> &) {
    return os;
  }
};

template <typename T, typename... Args> struct type_descriptor<T, Args...> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T, Args...> &) {
    os << type_descriptor<T>() << " ," << type_descriptor<Args...>();
    return os;
  }
};

template <> struct type_descriptor<char> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<char> &) {
    os << "char";
    return os;
  }
};

template <> struct type_descriptor<int> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<int> &) {
    os << "int";
    return os;
  }
};

template <typename T> struct type_descriptor<T *> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T *> &) {
    os << type_descriptor<T>() << " *";
    return os;
  }
};

template <typename T> struct type_descriptor<const T> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<const T> &) {
    os << "const " << type_descriptor<T>();
    return os;
  }
};

template <typename T> struct type_descriptor<T &> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T &> &) {
    os << type_descriptor<T>() << "&";
    return os;
  }
};

template <typename T, size_t N> struct type_descriptor<T[N]> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T[N]> &) {
    os << type_descriptor<T>() << "[" << N << "]";
    return os;
  }
};

template <typename T> struct type_descriptor<T()> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T()> &) {
    os << type_descriptor<T>() << "()";
    return os;
  }
};

template <typename T, typename... Args> struct type_descriptor<T(Args...)> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T(Args...)> &) {
    os << type_descriptor<T>() << "(" << type_descriptor<Args...>() << ")";
    return os;
  }
};

void test() {
  std::cout << type_descriptor<char>() << std::endl;
  std::cout << type_descriptor<const int *>() << std::endl;
  std::cout << type_descriptor<int const *&>() << std::endl;
  std::cout << type_descriptor<const char *[10]>() << std::endl;
  std::cout << type_descriptor<const char *(*)()>() << std::endl;
  std::cout << type_descriptor<char *(*)(int, const char *)>() << std::endl;
}
} // namespace chapter_2_3

// 2-4

// 2-5
namespace chapter_2_5 {
template <typename... Args> struct type_descriptor {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<Args...> &) {
    return os;
  }
};

template <typename T, typename... Args> struct type_descriptor<T, Args...> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T, Args...> &) {
    os << type_descriptor<T>() << " ," << type_descriptor<Args...>();
    return os;
  }
};

template <> struct type_descriptor<char> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<char> &) {
    os << "char";
    return os;
  }
};

template <> struct type_descriptor<int> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<int> &) {
    os << "int";
    return os;
  }
};

template <typename T> struct type_descriptor<T *> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T *> &) {
    os << "pointer to " << type_descriptor<T>();
    return os;
  }
};

template <typename T> struct type_descriptor<const T> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<const T> &) {
    os << "const " << type_descriptor<T>();
    return os;
  }
};

template <typename T> struct type_descriptor<T &> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T &> &) {
    os << "reference to " << type_descriptor<T>();
    return os;
  }
};

template <typename T, size_t N> struct type_descriptor<T[N]> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T[N]> &) {
    os << "array of " << type_descriptor<T>();
    return os;
  }
};

template <typename T> struct type_descriptor<T()> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T()> &) {
    os << "function returning " << type_descriptor<T>();
    return os;
  }
};

template <typename T, typename... Args> struct type_descriptor<T(Args...)> {
  friend std::ostream &operator<<(std::ostream &os,
                                  const type_descriptor<T(Args...)> &) {
    os << "function returning " << type_descriptor<T>() << "("
       << type_descriptor<Args...>() << ")";
    return os;
  }
};

void test() {
  std::cout << type_descriptor<char>() << std::endl;
  std::cout << type_descriptor<const int *>() << std::endl;
  std::cout << type_descriptor<int const *&>() << std::endl;
  std::cout << type_descriptor<const char *[10]>() << std::endl;
  std::cout << type_descriptor<const char *(*)()>() << std::endl;
  std::cout << type_descriptor<char *(*)(int, const char *)>() << std::endl;
}
} // namespace chapter_2_5

namespace chapter_2_6 {
template <typename T> struct is_unsigned : public boost::is_unsigned<T> {};
} // namespace chapter_2_6

void test2() {
  std::cout << "chapter 2-0 testing....." << std::endl;
  chapter_2_0::test();
  std::cout << "chapter 2-1 testing....." << std::endl;
  chapter_2_1::test();
  std::cout << "chapter 2-2 testing....." << std::endl;
  chapter_2_2::test();
  std::cout << "chapter 2-3 testing....." << std::endl;
  chapter_2_3::test();
  std::cout << "chapter 2-5 testing....." << std::endl;
  chapter_2_5::test();
}