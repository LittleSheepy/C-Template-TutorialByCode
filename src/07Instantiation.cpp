//#include <cassert>
//#include <cstdarg>
//#include <cstdio>
//#include <cstring>
//#include <string>
//#include <iostream>
//#include<stdlib.h>
//#include <cassert>
//#include <cstddef>
//#include <tuple>
//#include <string>
//#include <type_traits>
//#include <set>
////#include <vector>
//#include <deque>
//#include <array>
//#include <bitset>
//#include <utility>
//#include <list>

#define vector_hpp 1
#if vector_hpp
#include "type_traits.hpp"
#include "vector.hpp"
#include "list.hpp"
#else
#include <vector>
#include <list>
#endif // vector_hpp
//#include "vector.hpp"


////#include <vector>
using namespace std;
#define WRONG_CODE_ENABLED 0
#define STD_CPP 17

/*
编译器遇到 模板特化 时会用所给的 模板实参 替换对应的模板参数，从而产生特化。
如果声明类模板的指针或引用，不需要看到类模板定义，但如果要访问特化的成员或想知道模板特化的大小，就要先看到定义
*/ 
namespace jc7_1 {

    template <typename T>
    struct A;

    A<int>* p = 0;  // OK：不需要类模板定义

    template <typename T>
    struct A {
        void f();
    };

    void g(A<int>& a) {     // 只使用类模板声明
        a.f();              // 使用了类模板定义，需要 A::f() 的定义
    }

    template <typename T>
    void A<T>::f() {}

}  // namespace jc
namespace jc7_2 {
    //vector<int> mvector;
    list<int> mlist;

    void foo(){
        mlist.push_back(2);
        //mvector.push_back(100);


    }
}

void Instantiation(void) {
    //jc7_2::foo();
}