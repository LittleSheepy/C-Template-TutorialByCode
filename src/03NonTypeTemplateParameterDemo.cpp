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

using namespace std;
#define WRONG_CODE_ENABLED 0
#define STD_CPP 14

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
#if STD_CPP >= 17
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
#endif
}  // namespace jc

// C++14 允许 auto 作返回类型
namespace jc3_3 {
#if STD_CPP >= 14
    template <typename T, typename U>
    constexpr auto add(const T& a, const U& b) {
        return a + b;
    }

    template <typename T, typename U>
    auto add2(const T& a, const U& b) {
        return a + b;
    }

    void foo() {
        //static_assert(add('A', 2) == 'C');
        char c = add('A', 2);   // C
        cout << c << endl;
        char c2 = add2('A', 2); // C
        cout << c2 << endl;
        int c3 = add2('A', 2);  // 67
        cout << c3 << endl;
    }
#endif
}  // namespace jc

// C++20 之前，非类型模板参数不能是浮点数
namespace jc3_4 {

#if WRONG_CODE_ENABLED
    template <auto N>
    struct A {
        static constexpr auto f() { return N; }
    };

    static_assert(A<42>::f() == 42);
    static_assert(A<3.14>::f() == 3.14);  // C++20 属性->常规->C++语言标准->std:c++latest
#endif
}  // namespace jc

/*
* 不能用字符串字面值常量、临时对象、数据成员或其他子对象作模板实参。
* C++ 标准演进过程中逐渐放宽了对字符数组作为模板实参的限制，
C++11 仅允许外链接（external linkage，不定义于单一的文件作用域，链接到全局符号表），
C++14 允许外链接或内链接（internal linkage，只能在单个文件内部看到，不能被其他文件访问，不暴露给链接器），
C++17 不要求链接
*/
namespace jc3_5 {

    template <const char* s>
    struct A {};

    constexpr const char* s1 = "hello";  // 内链接对象的指针
    extern const char s2[] = "world";    // 外链接
    const char s3[] = "down";            // 内链接

    void foo() {
        static const char s4[] = "demo";  // 无链接
#if WRONG_CODE_ENABLED
        // 不能用字符串字面值常量作模板实参。
        A<"downdemo">{};              // 错误
        A<s1>{};                      // 错误
#endif
        A<s2>{};                      // C++11 允许
        A<s3>{};                      // C++14 允许
        A<s4>{};                      // C++17 允许 c++14 没报错
    }
}  // namespace jc

// 非类型模板参数可以是左值引用，此时实参必须是静态常量
namespace jc3_6 {

    template <int& N>
    struct A {
        A() { ++N; }
        ~A() { --N; }
    };

    void foo() {
        static int n = 0;
        {
            A<n> a;
            assert(n == 1);
        }
        assert(n == 0);
    }
}  // namespace jc

// 函数和数组类型作为非类型模板参数会退化为指针类型
namespace jc3_7 {

    template <int buf[5]>
    struct Lexer {};

     //template <int* buf>
     //struct Lexer {};  // 错误：重定义

    template <int fun()>
    struct FuncWrap {};

    // template <int (*)()>
    // struct FuncWrap {};  // 错误：重定义

}  // namespace jc

// 如果模板实参的表达式有大于号，必须用小括号包裹表达式，否则大于号会被编译器视为表示参数列表终止的右尖括号，导致编译错误
namespace jc3_8 {
#if STD_CPP >= 17
    template <bool b>
    struct A {
        inline static constexpr bool value = b;
    };

    void foo() {
        static_assert(A<(sizeof(int) > 0)>::value);
        static_assert(!A< sizeof(int)<0 >::value);      // < 不加(),不报错
        //static_assert(A<  sizeof(int)>0  >::value);       // 错误
    }
#endif
}  // namespace jc

// C++14 提供了变量模板
namespace jc3_9 {
    // constexpr修饰的需要进行初始化,3.24对应类型只能是float、double等数字类型
    // std::string等其他类型，不能转换，会编译错误
    // double 为默认模板参数
    template <typename T = double>
    constexpr T pi{ static_cast<T>(3.1415926535897932385) };

    template <typename T = double>
    constexpr T pi2{ 3.1415926535897932385};

    // 变量模板，这种定义最好不要出现在函数内部或块作用域内部
    template<typename T>
    T var{}; // 可以进行初始化，也可以不初始化

    // 函数模板、类模板有默认模板参数，变量模板当然也有默认模板参数
    template<typename T = int>
    T defaultVar{};

    void foo() {
        static_assert(pi<bool> == true);
        static_assert(pi<int> == 3);
        static_assert(pi<double> == 3.1415926535897932385);
        static_assert(pi<> == 3.1415926535897932385);
#if WRONG_CODE_ENABLED
        pi2<std::string>;       // 非法初始化了含非常量表达式的“constexpr”实体
#endif
        var<int> = 3;
        std::cout << defaultVar<> << std::endl;// use default template para type

        defaultVar<std::string> = "world";
        defaultVar<std::vector<int>> = { 1,2,3,4 };
        std::cout << defaultVar<std::string> << std::endl;

        for (auto it : defaultVar<std::vector<int>>) {
            std::cout << it << std::endl;
        }   // 1234

        for (auto it : defaultVar<std::string>) {
            std::cout << it << std::endl;
        }   // world
    }

}  // namespace jc

// 变量模板可以由非类型参数 参数化
namespace jc3_10 {

    template <int N>
    std::array<int, N> arr{};

    template <auto N>
    constexpr decltype(N) dval = N;

    static_assert(dval<'c'> == 'c');

    void foo() {
        arr<3> = { 1,2,3 };
        for (auto it : arr<3>) {
            std::cout << it << std::endl;
        }
        arr<3>[0] = 42;
        assert(arr<3>[0] == 42);

        std::cout << dval<10> << std::endl;
        std::cout << dval<'a'> << std::endl;
    }
}  // namespace jc

// 成员变量模板化
namespace jc3_11 {
    template<typename T>
    struct Member {
        static const int max{ 10 };
        std::string name{ "ts" };
    };

    // 定义变量模板替换类成员变量
    template<typename T>
    int maxType = Member<T>::max;

    void foo() {
        auto intTypeVar = Member<int>::max;
        auto strTypeVar = Member<std::string>::max;

        std::cout << intTypeVar << std::endl;       // 10
        std::cout << strTypeVar << std::endl;       // 10


        std::cout << maxType<int> << std::endl;
        std::cout << maxType<std::string> << std::endl;
        //static_assert(Member<std::string>::max == "10");
    }
}

// 继承true/false_type进行偏特化判断
namespace jc3_12 {
    template<typename T1, typename T2>
    struct IsSameT : std::false_type {};

    template<typename T>
    struct IsSameT<T, T> : std::true_type {};


    // 变量模板，使用偏特化模板进行初始化，由于value是值而不是type，不要和using 别名混淆
    template<typename T1, typename T2>
    constexpr bool IsSame = IsSameT<T1, T2>::value;

    void foo() {
        // 相当于IsSameT<T1, T2>::value简化为IsSame<T1,T2>

        std::cout << std::boolalpha;
        std::cout << IsSame<int, int> << std::endl; // true
        std::cout << IsSame<int, long> << std::endl;// false
    }
}
void NonTypeTemplateParameterDemo(void) {
    jc3_12::foo();
}




