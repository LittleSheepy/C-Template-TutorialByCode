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
namespace jc2_1 {
    /*
    对于不同类型模板参数的类模板，会为每个类型实例化出不同的类，类的函数被调用时才实例化，
    类模板的 static 数据成员会分别在每个不同的类中实例化，static 数据成员和成员函数只被同一个类共享。
    */
    template <typename T>
    class A {
    public:
        static int value();
        static void set_value(int i);

    private:
        static int n;
    };

    template <typename T>
    inline int A<T>::value() {
        return n;
    }

    template <typename T>
    inline void A<T>::set_value(int i) {
        n = i;
    }

    template <typename T>
    int A<T>::n = 0;

    void foo() {
        // 对于不同类型模板参数的类模板，会为每个类型实例化出不同的类，类的函数被调用时才实例化，
        A<void> a;    // 实例化 A<void>::n  class jc1::A<void>
        cout << "a.value():   " << a.value()<<endl;     // 0
        A<int> b, c;  // 实例化 A<int>::n，bc 共享 A<int>::value() 和 A<int>::n
        cout << "b.value():   " << b.value() << endl;     // 0
        cout << "c.value():   " << c.value() << endl;     // 0
        int n = A<double>::value();  // 实例化 A<double>::value()
        cout << "A<double>::value() n:   " << n << endl;     // 0
        n = b.value();               // 使用 A<int>::value()
        cout << "b.value() n:   " << n << endl;     // 0
        n = A<int>::value();  // 必须指定模板参数以确定实例化版本
        cout << "A<int>::value() n:   " << n << endl;     // 0
        // 类模板的 static 数据成员会分别在每个不同的类中实例化，static 数据成员和成员函数只被同一个类共享。
        // b、c共享一个实例化。其static共享
        a.set_value(1);
        cout << "a.set_value(1);" << endl;     // 0
        cout << "a.value():   " << a.value() << endl;     // 1
        cout << "b.value():   " << b.value() << endl;     // 0
        cout << "c.value():   " << c.value() << endl;     // 0
        b.set_value(2);
        cout << "b.set_value(2);" << endl;     // 0
        cout << "a.value():   " << a.value() << endl;     // 1
        cout << "b.value():   " << b.value() << endl;     // 2
        cout << "c.value():   " << c.value() << endl;     // 2
    }

}  // namespace jc
namespace jc2_2 {
    /*
    * 由于函数被调用时才实例化，如果不调用实例化时会出错的函数，代码也能通过编译
    */
    template <typename T>
    class A {
    public:
        static int value();

    private:
        static int n;
    };

    template <typename T>
    inline int A<T>::value() {
        return f(n);
    }

    template <typename T>
    int A<T>::n = 0;

    void foo() {
        A<void> a;  // OK，实例化 A<void>::n
        // a.value();         // 实例化错误，f 未声明
        // A<void>::value();  // 实例化错误，f 未声明
    }

}  // namespace jc


/// <summary>
/// 友元
/// </summary>
namespace jc2_3 {
    /*
    * 类内定义友元可以省略模板参数，但友元函数在类模板实例化后才会实例化，如果类模板中的友元函数不包含模板参数，则会出现重定义的错误 f
    * 类外定义友元不会有重定义的问题，f2
    * 需要在类内声明处为类模板额外指定不同的模板参数 f11
    */
    template <typename T>
    class A {
        // 类作用域内的 A 是注入类名，等价于 A<T>.  const A& rhs 等价于 const A<T>& rhs
        friend std::ostream& operator<<(std::ostream& os, const A& rhs) {
            return os << "A<" << typeid(T).name() << "> = " << rhs.n;
        }

        friend string f1(A a) { return typeid(T).name(); }
        friend string f11(A a);
        friend void f2();
        //friend void f() {}

    private:
        int n = 0;
    };
    // 
    void f2() {}
#if WRONG_CODE_ENABLED
    string f11(A a) { return typeid(T).name(); }     // “jc2_3::A”: 使用 类 模板 需要 模板 参数列表. “T”: 未声明的标识符
#endif
    template <typename T>
    string f11(A<T> a) { return typeid(T).name(); }

    void foo() {
        A<float> a;  // 实例化 operator<<(std::ostream&, const A<void>&) 和 f()
        std::cout << a << endl;  // A<void> = 0
        A<int> b;    // 错误：第二次实例化 f()，重定义
        std::cout << b << endl;  // A<void> = 0
    }
}  // namespace jc
#include <iostream>
#include <typeinfo>

namespace jc2_4 {
    // 如果要在类外定义友元，又不想在类内声明额外指定模板参数，则可以将友元声明为函数模板，在类内使用模板实例作为友元
    template <typename T>
    class A;
    template <typename T>
    int f1(A<T> a) { return 1; }

    template <typename T>
    class A {
        friend std::ostream& operator << <T>(std::ostream& os, const A<T>& rhs);

        friend int f1<T>(A<T> a);
    private:
        int n = 0;
    };

    template <typename R>
    std::ostream& operator<<(std::ostream& os, const A<R>& rhs);

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const A<T>& rhs) {
        return os << "A<" << typeid(T).name() << "> = " << rhs.n;
    }

    void foo() {
        A<void> a;
        A<int> b;
        std::cout << a << endl;  // A<void> = 0
        std::cout << f1(a) << endl;  // 1
    }

}   // namespace jc

namespace jc2_5 {

    template <typename T>
    struct Node;

    template <typename T>
    struct Tree {
        friend class Node<T>;  // 友元类模板必须已声明并可见
        friend class A;        // 友元类可以未声明
    };

}  // namespace jc
namespace jc2_6 {
    // 模板参数可以是友元
    template <typename T>
    class A {
        friend T;  // 如果 T 不是 class 则忽略
    public:
        int get_n() { return n; }
    private:
        int n = 0;
    };

    class B {
    public:
        static int f(A<B>& a) {
            a.n++;
            return a.n;
        }
    };
    void foo() {
        // 模板参数可以是友元
        A<B> a;
        B b;
        int n = b.f(a);
        cout << n << endl;
        cout << a.get_n() << endl;
    }
}  // namespace jc

/*
* 特化（Specialization）
* 特化一般指的是全特化，即为一种特定类型指定一个特定实现，该类型将不使用模板的实例化版本
*/
namespace jc2_7 {

    template <typename T>
    class A {
    public:
        int f() { return 1; }
    };
    // 对于T 为int 类型特化
    template <>
    class A<int> {
    public:
        int f() { return 2; }
        int g() { return 3; }
    };

    void foo() {
        A<void> a;
        assert(a.f() == 1);
        A<int> b;
        assert(b.f() == 2);
        assert(b.g() == 3);
    }
}  // namespace jc
/*
* 偏特化是为一类类型指定特定实现，是一种更通用的特化
*/
namespace jc2_8 {

    template <typename T>
    class A {
    public:
        int f() { return 1; }
    };

    // // 对于T 为指针 类型特化
    template <typename T>
    class A<T*> {
    public:
        int f() { return 2; }
        int g() { return 3; }
    };
    void foo() {
        A<int> a;
        assert(a.f() == 1);
        A<int*> b;
        assert(b.f() == 2);
        assert(b.g() == 3);
        A<A<int>*> c;
        assert(c.f() == 2);
        assert(c.g() == 3);
    }
}  // namespace jc
/*
偏特化可以指定多个模板参数之间的关系，
如果多个偏特化匹配程度相同，将产生二义性错误。
如果多个特化中，有一个匹配程度最高，则不会有二义性错误。
如果模板声明是一个普通声明（没有在模板名称后添加尖括号），这个声明就是一个主模板（primary template），
编写偏特化通常会有一个主模板和其他偏特化模板
*/
namespace jc2_9 {
    // 主模板
    template <typename T, typename U>
    struct A {
        static constexpr int i = 0;
    };

    template <typename T>
    struct A<T, T> {
        static constexpr int i = 1;
    };

    template <typename T>
    struct A<T, int> {
        static constexpr int i = 2;
    };

    template <typename T, typename U>
    struct A<T*, U*> {
        static constexpr int i = 3;
    };

    template <typename T>
    struct A<T*, T*> {
        static constexpr int i = 4;
    };

    void foo() {
        static_assert(A<double, double>::i == 1);   // 匹配顺序：1-0
        static_assert(A<double, int>::i == 2);      // 匹配顺序：2-0
        static_assert(A<int*, double*>::i == 3);    // 匹配顺序：3-0
        static_assert(A<double*, int*>::i == 3);    // 匹配顺序：3-0
        //A<int*, int*>{};
        static_assert(A<int*, int*>::i == 4);       // 匹配顺序：4-1/3-0
#if WRONG_CODE_ENABLED
        // 如果多个偏特化匹配程度相同，将产生二义性错误。
        static_assert(A<int, int>::i == 1);             // 匹配顺序：1/2-0 // 错误，匹配 A<T, T> 和 A<T, int> 
#endif
    }
}  // namespace jc

// 偏特化常用于元编程
namespace jc2_10 {
    template <typename T, typename Tuple>
    struct is_among;

    template <typename T, template <typename...> class Tuple, typename... List>
    struct is_among<T, Tuple<List...>>
        : std::disjunction<std::is_same<T, List>...> {};

    template <typename T, typename Tuple>
    inline constexpr bool is_among_v = is_among<T, Tuple>::value;


    static_assert(is_among_v<int, std::tuple<char, int, double> >);
    static_assert(!is_among_v<float, std::tuple<char, int, double>>);

    void foo() {}
}  // namespace jc

// 偏特化遍历 std::tuple
namespace jc2_11 {

    template <std::size_t Index, std::size_t N, typename... List>
    struct PrintImpl {
        static void impl(const std::tuple<List...>& t) {
            std::cout << std::get<Index>(t) << " ";
            PrintImpl<Index + 1, N, List...>::impl(t);
        }
    };

    template <std::size_t N, typename... List>
    struct PrintImpl<N, N, List...> {
        static void impl(const std::tuple<List...>& t) {}
    };

    template <typename... List>
    void Print(const std::tuple<List...>& t) {
        PrintImpl<0, sizeof...(List), List...>::impl(t);
    }

    void foo() {
        auto t = std::make_tuple(3.14, 42, "hello world", 25);
        Print(t);  // 3.14 42 hello world
    }
}  // namespace jc

// 成员模板也能特化或偏特化
namespace jc2_12 {

    struct A {
        template <typename T = std::string>
        T as() const {
            cout << "TTTT" << endl;
            return s;
        }

        std::string s;
    };

    template <>
    inline bool A::as<bool>() const {
        cout << "bool" << endl;
        return s == "true";
    }

    void foo() {
        A a{ "hello" };
        assert(a.as() == "hello");      // 调用TTT
        assert(!a.as<bool>());          // 调用bool 返回0
        A b{ "true" };
        assert(b.as<bool>());           // 调用bool 返回1
    }
}  // namespace jc

// 成员函数模板不能为虚函数，因为虚函数表的大小是固定的，而成员函数模板的实例化个数要编译完成后才能确定
namespace jc2_13 {
    template <typename T>
    class Dynamic {
    public:
        virtual ~Dynamic() {}  // OK，每个 Dynamic<T> 对应一个析构函数

#if WRONG_CODE_ENABLED
        template <typename U>
        virtual void f(const U&) {}  // 错误，编译器不知道一个 Dynamic<T> 中 f() 个数
#endif
    };

}  // namespace jc

/*
* 模板的模板参数（Template Template Parameter）
*/
/*
如果模板参数的类型是类模板(一个模板的参数是模板类型-类模板)，则需要使用模板的模板参数。
对于模板的模板参数，C++11 之前只能用 class 关键字修饰，C++11 及其之后可以用别名模板的名称来替代，C++17 可以用 typename 修饰
*/
namespace jc2_14 {

    template <typename T, template <typename...> class Container>
    void f(const Container<T>&) {}

    void foo() {
        f(std::vector<int>{});
        f(std::vector<double>{});
        f(std::set<int>{});
    }
}  // namespace jc

// 实际上容器还有一个模板参数，即内存分配器 allocator
namespace jc2_15 {

    template <typename T, 
              // 模板作为 模板参数
              template <typename Elem, typename = std::allocator<Elem>> class Container = std::deque>
    class Stack {
    public:
        using reference = T&;
        using const_reference = const T&;

        template <typename, template <typename, typename> class>
        friend class Stack;

        template <typename U,
            template <typename Elem2, typename = std::allocator<Elem2>> class Container2>
        Stack<T, Container>& operator=(const Stack<U, Container2>&);

        void push(const T&);

        void pop();

        reference top();

        const_reference top() const;

        std::size_t size() const;

        bool empty() const;

    private:
        Container<T> container_;
    };

    template <typename T, template <typename, typename> class Container>
    template <typename U, template <typename, typename> class Container2>
    inline Stack<T, Container>& Stack<T, Container>::operator=(
        const Stack<U, Container2>& rhs) {
        container_.assign(rhs.container_.begin(), rhs.container_.end());
        return *this;
    }

    template <typename T, template <typename, typename> class Container>
    inline void Stack<T, Container>::push(const T& x) {
        container_.emplace_back(x);
    }

    template <typename T, template <typename, typename> class Container>
    inline void Stack<T, Container>::pop() {
        assert(!empty());
        container_.pop_back();
    }

    template <typename T, template <typename, typename> class Container>
    inline typename Stack<T, Container>::reference Stack<T, Container>::top() {
        assert(!empty());
        return container_.back();
    }

    template <typename T, template <typename, typename> class Container>
    inline typename Stack<T, Container>::const_reference Stack<T, Container>::top()
        const {
        assert(!empty());
        return container_.back();
    }

    template <typename T, template <typename, typename> class Container>
    inline std::size_t Stack<T, Container>::size() const {
        return container_.size();
    }

    template <typename T, template <typename, typename> class Container>
    inline bool Stack<T, Container>::empty() const {
        return container_.empty();
    }

    void foo() {
        Stack<std::string> s;
        s.push("hello");
        s.push("world");
        assert(s.size() == 2);
        assert(s.top() == "world");
        s.pop();
        assert(s.size() == 1);
        assert(s.top() == "hello");
        s.pop();
        assert(s.empty());
    }
}  // namespace jc

void ClassTemplateDemo(void) {
    jc2_15::foo();
}




