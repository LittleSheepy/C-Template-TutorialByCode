#include <iostream>
#include<stdlib.h>
#include <cassert>
#include <cstddef>
using namespace std;
#define WRONG_CODE_ENABLED 0

/*
    函数模板
*/
// Overload 重载
int MaxValue1(int a, int b) {
    std::cout << " use: int MaxValue1(int a, int b) " << std::endl;
    return a > b ? a : b;
}
float MaxValue1(float a, float b) {
    std::cout << " use: float MaxValue1(float a, float b) " << std::endl;
    return a > b ? a : b;
}

template<typename T>
T MaxValue1(T a, T b) {
    std::cout << " use: template<typename T> T MaxValue1(T a, T b) " << std::endl;
    return a > b ? a : b;
}


template <typename T, typename U>
T MaxValue2(const T a, const U b) {
    std::cout << " use: template <typename T, typename U> T MaxValue2(const T a, const U b) " << std::endl;
    return a < b ? b : a;
}
// 一重载
//template <typename RT, typename T, typename U>
//int MaxValue2(const T& a, const U& b) {
//    std::cout << " use: template <typename RT, typename T, typename U> int MaxValue1(const T & a, const U & b) " << std::endl;
//    return 1;
//}
// 二 重载
template <typename RT, typename T, typename U>
RT MaxValue2(const T& a, const U& b) {
    std::cout << " use: template <typename RT, typename T, typename U> RT MaxValue1(const T & a, const U & b) " << std::endl;
    return 2;
}
template <typename T, typename U>
T MaxValue2p(const T& a, const U& b) {
    return a < b ? b : a;
}

template <typename RT, typename T, typename U>
RT MaxValue3p(const T& a, const U& b) {
    return a < b ? b : a;
}

template <typename T, typename U>
auto MaxValue2pAuto(const T& a, const U& b) -> decltype(true ? a : b) {
    return a < b ? b : a;
}

template <typename T, typename U>
constexpr auto MaxValue2pAutoConstexpr(const T& a, const U& b) {
    return a < b ? b : a;
}

// 注意不能返回 C-style 字符串的引用
template <typename T>
const T& ReturnCharReference(const char* s) {
    return s;
}

template <typename T>
const T& ReturnCharReference2(const T& a, const T& b) {
    std::cout << " use: template" << std::endl;
    return std::strcmp(a, b) < 0 ? b : a;
    //return b < a ? a : b;
}

// 新增函数来支持 C-style 参数
const char* ReturnCharReference2(const char* a, const char* b) {
    std::cout << " use: function." << a << ", "<< b << std::endl;
    return std::strcmp(a, b) < 0 ? b : a;
}

template <typename T>
const T& ReturnCharReference2(const T& a, const T& b, const T& c) {
    return ReturnCharReference2(ReturnCharReference2(a, b), c);  // max("down", "de") 返回临时对象的引用
}

//只有在函数调用前声明的重载才会被匹配，即使后续有更优先的匹配，由于不可见也会被忽略
template <typename T>
int f(T) {
    return 1;
}

template <typename T>
int g(T a) {
    return f(a);
}

int f(int) { return 2; }

// type traits 类型萃取
// 用 std::common_type 来计算不同类型中最通用的类型
template <typename T, typename U, typename RT = std::common_type_t<T, U>>
RT MaxValue3pT(const T& a, const U& b) {
    return a < b ? b : a;
}
// Specialization 特化

void foo();
void TemplateDemo1() {
    int a = 3, b = 9;
    float af = 3.1, bf = 9.1;
    char ac = 'a', bc = 'b';
    std::string as = "ab", bs = "bc";
    /******* Overload 重载  */
    // 当类型同时匹配普通函数和模板时，优先匹配普通函数
    cout << "MaxValue1(a,b):" << MaxValue1(a, b) << endl;     // 9
    cout << "MaxValue1(ac,bc):" << MaxValue1(ac, bc) << endl; // b
    cout << "MaxValue1(af,bf):" << MaxValue1(af, bf) << endl; // 9.1
    cout << "MaxValue1(af,bf):" << MaxValue1(as, bs) << endl; // bc
    // 如果对于给定的实参能同时匹配两个模板，重载解析会优先匹配更特殊的模板，
    // 不重载 template <typename T, typename U> T MaxValue2(const T a, const U b)  int MaxValue2<int, double>(int a, double b)
    //   重载 一样
    cout << "Overload MaxValue1(1, 3.14):" << MaxValue2(1, 3.14) << endl;     // 3
    // 不重载 template <typename T, typename U> T MaxValue2(const T a, const U b)  double MaxValue2<double, int>(double a, int b)
    //   重载 一样
    cout << "Overload MaxValue1(1, 3.14):" << MaxValue2(3.14, 1) << endl;     // 3.14
    // 不重载 template <typename T, typename U> T MaxValue2(const T a, const U b)                      double MaxValue2<double, double>(double a, double b)
    // 1 一重载 template <typename RT, typename T, typename U> int MaxValue1(const T & a, const U & b)   int MaxValue2<double, int, double>(const int &a, const double &b)
    // 2 二重载 template <typename RT, typename T, typename U> RT MaxValue1(const T & a, const U & b)   double MaxValue2<double, int, double>(const int &a, const double &b)
    // 一二重载 有多个 重载函数 实例与参数列表匹配.对重载函数的调用不明确
    cout << "Overload MaxValue1<double>(1, 3.14):" << MaxValue2<double>(1, 3.14) << endl;     // 3
    // 不重载 template <typename T, typename U> T MaxValue2(const T a, const U b)                      double MaxValue2<double, double>(double a, double b)
    // 1 一重载 template <typename RT, typename T, typename U> int MaxValue1(const T & a, const U & b)   int MaxValue2<double, double, int>(const double &a, const int &b)
    // 2 二重载 template <typename RT, typename T, typename U> RT MaxValue1(const T & a, const U & b)   double MaxValue2<double, double, int>(const double &a, const int &b)
    // 一二重载 有多个 重载函数 实例与参数列表匹配.对重载函数的调用不明确
    cout << "Overload MaxValue1<double>(1, 3.14):" << MaxValue2<double, double>(3.14, 1) << endl;     // 有多个 重载函数 实例与参数列表匹配.对重载函数的调用不明确

#if WRONG_CODE_ENABLED
    // 不重载 template <typename T, typename U> T MaxValue2(const T a, const U b)                      int MaxValue2<int, double>(int a, double b)
    // 一重载 有多个 重载函数 实例与参数列表匹配.对重载函数的调用不明确
    // 二重载 有多个 重载函数 实例与参数列表匹配.对重载函数的调用不明确
    cout << "Overload MaxValue1<double>(1, 3.14):" << MaxValue2<int>(1, 3.14) << endl;     // 3
#endif

#if WRONG_CODE_ENABLED
    MaxValue1("adown", bs);  // 错误，T 推断为 char[5] 和 std::string
#endif

#if WRONG_CODE_ENABLED
    MaxValue2("adown", bs);  // 无法从“const U”转换为“T”。  后面可以使用auto作为返回值
#endif
    // 对于推断不一致的情况，可以显式指定类型而不使用推断机制，或者强制转换实参为希望的类型使得推断结果一致
    // 显式指定类型
    cout << "MaxValue1<std::string>(''down'', ''demo''):" << MaxValue1<std::string>("down", "demo") << endl; // down
    // 强制转换
    cout << "MaxValue1(std::string{ ''down'' }, bs):" << MaxValue1(std::string{ "down" }, bs) << endl; // down
#if WRONG_CODE_ENABLED
    cout << "MaxValue1(a,bf):" << MaxValue1(a, bf) << endl;     // 没有匹配
#endif
    // 也可以增加一个模板参数，这样每个实参的推断都是独立的，不会出现矛盾
    cout << "MaxValue2(a,bf):" << MaxValue2(a, bf) << endl;     // 9  int MaxValue2<int, float>(int a, float b)
    cout << "MaxValue2(a,bf):" << MaxValue2<float>(a, bf) << endl;     // 9.1  float MaxValue2<float, float>(float a, float b)
    cout << "MaxValue1(a,bf):" << MaxValue1<float>(a, bf) << endl;     // 9.1  float MaxValue2<float>(float a, float b)
    // 模板实参不能推断返回类型，必须显式指定
    cout << "MaxValue3p<double>(1, 3.14):" << MaxValue3p<double>(1, 3.14) << endl;      // 3.14   double MaxValue3p<double, int, double>(const int &a, const double &b)
#if WRONG_CODE_ENABLED
    cout << "MaxValue3p<double>(1, 3.14):" << MaxValue3p(1, 3.14) << endl;      // 未能为“RT”推导 模板 参数
#endif
    //  auto 作为返回类型
    cout << "MaxValue2pAuto(1, 3.14):" << MaxValue2pAuto(1, 3.14) << endl;      // 3.14  auto MaxValue2pAuto<int, double>(const int &a, const double &b)->double
    cout << "MaxValue2pAuto('adown', bs):" << MaxValue2pAuto("adown", bs) << endl;      // bc  auto MaxValue2pAuto<char [6], std::string>(const char (&a)[6], const std::string &b)->const std::string
    // 用 constexpr 函数可以生成编译期值
    cout << "MaxValue2pAutoConstexpr(1, 3.14):" << MaxValue2pAutoConstexpr(1, 3.14) << endl;      // 3.14  constexpr double MaxValue2pAutoConstexpr<int, double>(const int &a, const double &b)
    //static_assert(MaxValue2pAutoConstexpr(1, 3.14) == 3.14);        // 属性->常规->C++语言标准->std:c++17

    /******* type traits 类型萃取  */
    cout << "MaxValue3pT(1, 3.14):" << MaxValue3pT(1, 3.14) << endl;    // 3.14 double MaxValue3pT<int, double, double>(const int &a, const double &b)
    /******* 注意不能返回 C-style 字符串的引用  */
    const char* s = "downdemo";
    //cout << "ReturnCharReference<const char*>(s):" << ReturnCharReference<const char*>(s) << endl; //
    cout << "ReturnCharReference<const char*>(s):" << (std::string)ReturnCharReference<const char*>(s) << endl; //
    //cout << "ReturnCharReference<const char*>(s):" << ReturnCharReference<const char*>() << endl; //
    //ReturnCharReference<const char*>(s);  // 错误：返回临时对象的引用

    const char* rac = "down";
    const char* rbc = "de";
    const char* rcc = "mo";
    //cout << "ReturnCharReference2<const char*>(rac, rbc, rcc):" << ReturnCharReference2<const char*>(rac, rbc, rcc) << endl; //
    //cout << "ReturnCharReference2<const char*>(rac, rbc, rcc):" << (std::string)ReturnCharReference2<const char*>(rac, rbc, rcc) << endl; //
    ReturnCharReference2<const char*>(rac, rbc, rcc);
    //ReturnCharReference2<const char*>(rac, rbc, rcc);  // 错误：返回临时对象的引用
    // 只有在函数调用前声明的重载才会被匹配，即使后续有更优先的匹配，由于不可见也会被忽略
    cout << "g(0):    "<< g(0)  << endl;
    //assert(g(0) == 1);
}




namespace jc9999 {
    /*
    字符串字面值传引用会推断为字符数组，为此需要为原始数组和字符串字面值提供特定处理的模板
    字符串字面值是一串常量字符 如： "down"
    */
    template <typename T, typename U>
    bool less(const T& a, const U& b) {
        cout << "1111111111111111 "<< endl;
        return a < b;
    }

    template <typename T, std::size_t M, std::size_t N>
    bool less(T(&a)[M], T(&b)[N]) {
        cout << "22222222222222 " << endl;
        for (std::size_t i = 0; i < M && i < N; ++i) {
            if (a[i] < b[i]) {
                return true;
            }
            if (b[i] < a[i]) {
                return false;
            }
        }
        return M < N;
    }

    void foo() {
        assert(less(0, 42));            // 调用第一个
        assert(less("demo", "down"));   // 调用第二个
        cout << "jc::less(0, 42):   " << less(0, 42) << endl; // 1
        cout << "jc::less('down', 'demo'):   " << less("down", "demo") << endl; // 0

    }
}  // namespace jc

namespace jc299 {

    template <typename T>
    int f(T) {
        return 1;
    }

    template <typename T>
    int g(T a) {
        return f(a);
    }

    void foo() {
        cout << "g(0):    " << g(0) << endl;   // 2
        //assert(jc2::g(0) == 1);
    }
    int f(int) { return 2; }

}  // namespace jc
namespace jc999 {
    template <typename T>
    const T& max(const T& a, const T& b) {
        return b < a ? a : b;
    }

    //const char* max(const char* a, const char* b) {
    //    return std::strcmp(a, b) < 0 ? b : a;
    //}

    template <typename T>
    const T& max(const T& a, const T& b, const T& c) {
        return max(max(a, b), c);  // 如果传值调用 max(a, b) 则出错
    }

    void foo() {
        const char* a = "frederic";
        const char* b = "anica";
        const char* c = "lucas";
        jc999::max(a, b, c);  // run-time ERROR // n instantiation of 'const T& max(const T&, const T&, const T&) [with T = const char*]':
    }
}
void TemplateDemo() {
    //jc3::foo();
    TemplateDemo1();
}
