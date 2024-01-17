#include <iostream>
#include <type_traits>

template <typename T>
concept Integral = std::is_integral<T>::value;

template <Integral T>
T Add(T a, T b) {
  return a + b;
}

struct A {
  using one = int;
};

struct B {
  using two = float;
};

template <typename T>
concept IsA = {}

template <typename T, typename U = std::void_t<>>
struct Foo {
    void operator()(T&&) {
        std::cout << "foo" << std::endl;
    }
};

template <typename T>
struct Foo<T, std::void_t<typename T::one>> {
    void operator()(T&&) {
        std::cout << "one" << std::endl;
    }
};

template <typename T>
struct Foo<T, std::void_t<typename T::two>> {
    void operator()(T&&) {
        std::cout << "two" << std::endl;
    }
};

template <typename T>
void foo(T&&) requires typename T::one {
    std::cout << "one" << std::endl;
}

int main() {
  std::cout << Add(1, 2) << std::endl;
  Foo<A>()(A{});
  Foo<B>()(B{});
  return 0;
}