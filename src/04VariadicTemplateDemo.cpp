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
#define STD_CPP 14

// 如果函数要接受任意数量任意类型的参数，没有模板时可以通过 std::va_list 实现
namespace jc4_1 {

    void test(int n, ...) {
        std::va_list args;
        va_start(args, n);
        assert(va_arg(args, double) == 3.14);
        assert(va_arg(args, int) == 42);
        assert(std::strcmp(va_arg(args, const char*), "hello") == 0);
        assert(std::strcmp(va_arg(args, const char*), "world") == 0);
        va_end(args);
    }

    void test(const char* fmt, ...) {
        char buf[256];
        std::va_list args;
        va_start(args, fmt);
        std::vsnprintf(buf, 256, fmt, args);
        va_end(args);
        assert(std::strcmp(buf, "3.14 42 hello world") == 0);
    }

    void foo() {
        test(4, 3.14, 42, std::string{ "hello" }.c_str(), "world");
        test("%.2f %d %s %s", 3.14, 42, std::string{ "hello" }.c_str(), "world");
    }
}  // namespace jc

/*
C++11 引入了变参模板，用省略号表示一个参数包，类型名后接省略号表示任意数量给定类型的参数。
在表达式后跟省略号，如果表达式中有参数包，就会把表达式应用到参数包中的每个参数。
如果表达式中出现两次参数包，对整个表达式扩展，而不会做笛卡尔积计算
*/
namespace jc4_2 {

    void print() {}  // 参数包展开到零参数时调用

    template <typename T, typename... Args>
    void print(const T& t, Args&&... args) {
        std::cout << t << ",";
        print(std::forward<Args>(args)...);
    }

    template <int... Index>
    struct A {};

    template <typename... List, int... Index>
    void test1(const std::tuple<List...>& t, A<Index...>) {
        print(std::get<Index>(t)...);  // print(std::get<2>(t), std::get<3>(t));
    }

    template <typename... List, int... Index>
    void test2(const std::tuple<List...>& t, A<Index...>) {
        print((std::get<Index>(t) + std::get<Index>(t))...);
    }

    // 注意参数包的省略号不能直接接在数值字面值后
    template <typename... Args>
    void f(const Args&... args) {
        //print(args + 1...);    // ERROR：1... 是带多个小数点的字面值，不合法
        print(args + 1 ...);   // OK
        print((args + 1)...);  // OK
    }
    void foo() {
        auto t = std::make_tuple(3.14, 42, std::string{ "hello" }, "world");
        test1(t, A<2, 3>{});     // hello,world
        test2(t, A<0, 1, 2>{});  // 6.28,84,hellohello,
    }
}  // namespace jc

/*
可以直接用逗号运算符做参数包扩展，
逗号左侧是对参数包每个元素做的操作，右侧是一个无关紧要的值，
这样展开后对每个元素都做了操作，并形成了一个以无关值为元素的数组，
这个数组无作用，只是为了满足扩展时省略号不能为表达式最后的 token 而引入
*/ 
namespace jc4_3 {

    template <typename... Args>
    void print(Args&&... args) {
        auto a = { (std::cout << std::forward<Args>(args) << std::endl, 2)... };
        for (auto aa : a) {
            cout << aa << endl;
        }
    }

    void foo() {
        print(3.14, 42, std::string{ "hello" }, "world");
    }
}  // namespace jc

// C++11 引入了 sizeof... 在编译期计算参数包中的元素数，
// C++17 引入了 if constexpr 判断编译期值，编译期结果为 true 才会实例化代码
namespace jc4_4 {

    template <typename T, typename... Args>
    void print(const T& t, Args&&... args) {
        std::cout << t << std::endl;
        if constexpr (sizeof...(args) > 0) {  // 不能用 if，因为零长包也会实例化代码
            print(std::forward<Args>(args)...);  // 当条件满足时才实例化
        }
    }

    void foo() { print(3.14, 42, std::string{ "hello" }, "world"); }
}  // namespace jc

// 在 C++11 中可以利用偏特化来达到 if constexpr 的效果
namespace jc4_5 {

    template <bool b>
    struct A;

    template <typename T, typename... Args>
    void print(const T& t, Args&&... args) {
        std::cout << t << std::endl;
        A<(sizeof...(args) > 0)>::f(std::forward<Args>(args)...);
    }

    template <bool b>
    struct A {
        template <typename... Args>
        static void f(Args&&... args) {
            print(std::forward<Args>(args)...);
        }
    };

    template <>
    struct A<false> {
        static void f(...) {}
    };

    void foo() { print(3.14, 42, std::string{ "hello" }, "world"); }
}  // namespace jc

/*折叠表达式（Fold Expression）
*/
// C++17 引入了折叠表达式，用于获取对所有 参数包实参 使用二元运算符的计算结果
namespace jc4_6 {

    template <typename... Args>
    auto sum(Args&&... args) {
        auto a = (... + std::forward<Args>(args));      // (((1 + 2) + 3) + 4)
        auto b = (std::forward<Args>(args) + ...);      // (1 + (2 + (3 + 4)))
        auto c = (5 + ... + std::forward<Args>(args));  // ((((5 + 1) + 2) + 3) + 4)
        auto d = (std::forward<Args>(args) + ... + 5);  // (1 + (2 + (3 + (4 + 5))))
        return std::make_tuple(a, b, c, d);
    }

    auto print1 = [](auto&&... args) {
        // operator<< 左折叠，std::cout 是初始值
        (std::cout << ... << std::forward<decltype(args)>(args));
    };

    auto print2 = [](auto&&... args) {
        // operator, 左折叠
        ((std::cout << std::forward<decltype(args)>(args) << ","), ...);
    };

    void foo() {
        auto [a, b, c, d] = sum(1, 2, 3, 4);
        print1(a, b, c, d);  // 10101515
        cout << endl;
        print2(a, b, c, d);  // 10,10,15,15,
    }
}  // namespace jc

// 实现与 Haskell 类似的左折叠和右折叠
namespace jc4_7 {

    template <typename F, typename T, typename... Args>
    void foldlList(F&& f, T&& zero, Args&&... x) {
        (   (   std::invoke(std::forward<F>(f), (std::string(sizeof...(Args), '(')) ),
                std::invoke(std::forward<F>(f), (std::forward<T>(zero)))
            ),
            ...,
            (   std::invoke(std::forward<F>(f), ('+')),
                std::invoke(std::forward<F>(f), (std::forward<Args>(x)) ),
                std::invoke(std::forward<F>(f), (')'))
            ) 
        );
    }

    template <typename F, typename T, typename... Args>
    void foldrList(F&& f, T&& zero, Args&&... x) {
        ((std::invoke(std::forward<F>(f), ('(')),
            std::invoke(std::forward<F>(f), (std::forward<Args>(x))),
            std::invoke(std::forward<F>(f), ('+'))),
            ...,
            (std::invoke(std::forward<F>(f), (std::forward<T>(zero))),
                std::invoke(std::forward<F>(f), (std::string(sizeof...(Args), ')')))));
    }

    void foo() {
        auto print = [](const auto& x) { std::cout << x; };     // lambda表达式
        foldlList(print, 0, 1, 2, 3, 4);  // ((((0+1)+2)+3)+4)
        cout << endl;
        foldrList(print, 0, 1, 2, 3, 4);  // (1+(2+(3+(4+0))))
    }
}  // namespace jc

void VariadicTemplateDemo(void) {
    jc4_3::foo();
}

