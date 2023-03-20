#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
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
#include <array>
#include <bitset>
#include <utility>
using namespace std;
#define WRONG_CODE_ENABLED 0
#define STD_CPP 17

/*
     移动语义
*/
/*
右值引用即只能绑定到右值的引用，字面值（纯右值）和临时变量（亡值）就是常见的右值。
如果把左值传递给右值引动参数，则需要强制类型转换，std::move 就是不需要显式指定类型的到右值引用的强制类型转换
*/
namespace jc5_1 {

    template <typename T>
    constexpr std::remove_reference_t<T>&& move(T&& x) noexcept {
        return static_cast<std::remove_reference_t<T>&&>(x);
    }
    /*
    std::move,和上面样。它就是std::string 的实现
    template <class _Ty>
    _NODISCARD constexpr remove_reference_t<_Ty>&& move(_Ty&& _Arg) noexcept { // forward _Arg as movable
        return static_cast<remove_reference_t<_Ty>&&>(_Arg);
    }
    */
    constexpr int f(const std::string&) { return 1; }
    constexpr int f(std::string&&) { return 2; }

    void foo() {
        std::string s;
        static_assert(f(s) == 1);
        assert(f(std::string{}) == 2);
        static_assert(f(static_cast<std::string&&>(s)) == 2);
        static_assert(f(jc5_1::move(s)) == 2);
        static_assert(f(std::move(s)) == 2);
    }
}  // namespace jc

// 完美转发（Perfect Forwarding）
namespace jc5_2 {

    template <typename T>
    constexpr T&& forward(std::remove_reference_t<T>& t) noexcept {
        return static_cast<T&&>(t);
    }

    constexpr int f(std::string&) { return 1; }
    constexpr int f(const std::string&) { return 2; }
    constexpr int f(std::string&&) { return 3; }

    template <typename T>
    constexpr int g(T&& s) {
        return f(jc5_2::forward<T>(s));  // 等价于 std::forward
    }

    void foo() {
        std::string s;
        const std::string& s2 = s;
        static_assert(g(s) == 1);             // T = T&& = std::string&
        assert(g(s2) == 2);                   // T = T&& = const std::string&
        static_assert(g(std::move(s)) == 3);  // T = std::string, T&& = std::string&&
        assert(g(std::string{}) == 3);        // T = T&& = std::string&
        assert(g("downdemo") == 3);           // T = T&& = const char (&)[9]
    }

}  // namespace jc

// 结合变参模板完美转发转发任意数量的实参
namespace jc5_3 {

    template <typename F, typename... Args>
    constexpr void constexpr_for(F&& f, Args&&... args) {
        (std::invoke(std::forward<F>(f), std::forward<Args>(args)), ...);
    }

    template <typename... Args>
    void print(Args&&... args) {
        constexpr_for([](const auto& x) { std::cout << x << std::endl; },
            std::forward<Args>(args)...);
    }

    void foo() { print(3.14, 42, std::string{ "hello" }, "world"); }
}  // namespace jc

// Lambda 中使用完美转发需要借助 decltype 推断类型
namespace jc5_4 {

    constexpr auto constexpr_for = [](auto&& f, auto&&... args) {
        (std::invoke(std::forward<decltype(f)>(f),
            std::forward<decltype(args)>(args)),
            ...);
    };

    auto print = [](auto&&... args) {
        constexpr_for([](const auto& x) { std::cout << x << std::endl; },
            std::forward<decltype(args)>(args)...);
    };

    void foo() { print(3.14, 42, std::string{ "hello" }, "world"); }
}  // namespace jc

// C++20 可以为 lambda 指定模板参数
namespace jc5_5 {
#if STD_CPP >= 20
    constexpr auto constexpr_for =
        []<typename F, typename... Args>(F&& f, Args&&... args) {
        (std::invoke(std::forward<F>(f), std::forward<Args>(args)), ...);
    };

    auto print = []<typename... Args>(Args&& ... args) {
        constexpr_for([](const auto& x) { std::cout << x << std::endl; },
            std::forward<Args>(args)...);
    };

    void foo() { print(3.14, 42, std::string{ "hello" }, "world"); }
#endif
}  // namespace jc

// C++20 的 lambda 可以捕获参数包
namespace jc5_6 {
#if STD_CPP >= 20
    template <typename... Args>
    void print(Args&&... args) {
        [... args = std::forward<Args>(args)] <typename F>(F && f) {
            (std::invoke(std::forward<F>(f), args), ...);
        }([](const auto& x) { std::cout << x << std::endl; });
    }
    void foo() { print(3.14, 42, std::string{ "hello" }, "world"); }
#endif
}  // namespace jc

/*
模板也能用于构造函数，但它不是真正的构造函数，从函数模板实例化而来的函数不和普通函数等价，
由成员函数模板实例化的函数不会重写虚函数，
由构造函数模板实例化的构造函数不是拷贝或移动构造函数，
但对一个 non-const 对象调用构造函数时，万能引用是更优先的匹配

为此可以用 std::enable_if 约束模板参数，在条件满足的情况下才会匹配模板
*/ 
namespace jc5_7 {

    struct A {
        template <typename T>
        explicit A(T&& t) : s(std::forward<T>(t)) {}

        A(const A& rhs) : s(rhs.s) {}
        A(A&& rhs) noexcept : s(std::move(rhs.s)) {}

        std::string s;
    };
    struct B {
        template <typename T,  // 要求 T 能转为 std::string
                  typename = std::enable_if_t<std::is_convertible_v<T, std::string>>>
        explicit B(T&& t) : s(std::forward<T>(t)) {}

        B(const A& rhs) : s(rhs.s) {}
        B(A&& rhs) noexcept : s(std::move(rhs.s)) {}

        std::string s;
    };
    void foo() {
        const A a1{ "downdemo" };
        A a2{ a1 };  // OK，匹配拷贝构造函数
        //A a3{ a1 };  // 错误，匹配模板构造函数.
        const B b1{ "downdemo" };
        B b2{ b1 };  // OK，匹配拷贝构造函数
        B b3{ b2 };  // OK，匹配拷贝构造函数
    }
}  // namespace jc

// C++20 可以用 concepts 约束模板参数
namespace jc5_8 {
#if STD_CPP >= 20
    struct A {
        template <typename T>
        requires std::convertible_to<T, std::string>
            explicit A(T&& t) : s(std::forward<T>(t)) {}

        A(const A& rhs) : s(rhs.s) {}
        A(A&& rhs) noexcept : s(std::move(rhs.s)) {}

        std::string s;
    };

    void foo() {
        const A a{ "downdemo" };
        A b{ a };  // OK，匹配拷贝构造函数
        A c{ b };  // OK，匹配拷贝构造函数
    }
#endif
}  // namespace jc


void MoveSemantics(void) {
    jc5_1::foo();
}
