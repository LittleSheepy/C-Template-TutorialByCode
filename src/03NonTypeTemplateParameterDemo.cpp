#include <iostream>
#include<stdlib.h>
#include <cassert>
#include <cstddef>
#include <tuple>
#include <string>
#include <type_traits>
#include <set>
#include <vector>
#include <deque>

using namespace std;
#define WRONG_CODE_ENABLED 0
#include <bitset>
#include <cassert>

// 非类型模板参数表示在编译期或链接期可以确定的常量值
namespace jc3_1 {
    // 这个N是8 常量
    template <bool IsSet = true, std::size_t N>
    std::size_t find_next(const std::bitset<N>& b, std::size_t cur) {
        for (std::size_t i = cur + 1; i < N; ++i) {
            if (!(b.test(i) ^ IsSet)) {
                return i;
            }
        }
        return N;
    }

    void foo() {
        std::bitset<8> b{ "10010111" };
        static constexpr int _next_set[] =   { 1, 2, 4, 4, 7, 7, 7, 8,     8, 8 };
        static constexpr int _next_unset[] = { 3, 3, 3, 5, 5, 6, 8, 8,     8, 8 };

        for (std::size_t i = 0; i < std::size(_next_set); ++i) {
            assert(find_next<true>(b, i) == _next_set[i]);
            assert(find_next<false>(b, i) == _next_unset[i]);
        }
    }
}  // namespace jc

// 模板参数可以由之前的参数推断类型，C++17 允许将非类型模板参数定义为 auto 或 decltype(auto)
namespace jc3_2 {

    template <typename>
    struct get_class;

    template <typename ClassType, typename MemberType>
    struct get_class<MemberType ClassType::*> {
        using type = ClassType;
    };

    template <typename T>
    using get_class_t = typename get_class<T>::type;

    template <auto ClassMember>
    class Wrapper {
    public:
        Wrapper(get_class_t<decltype(ClassMember)>& obj) : obj_(obj) {}

        void increase() { ++(obj_.*ClassMember); }

    private:
        get_class_t<decltype(ClassMember)>& obj_;
    };

    struct A {
        int i = 0;
    };

    void foo() {
        A a;
        Wrapper<&A::i>{a}.increase();
        assert(a.i == 1);
    }
}  // namespace jc


void NonTypeTemplateParameterDemo(void) {
    jc3_2::foo();
}




