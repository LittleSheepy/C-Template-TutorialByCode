#include "xmemory.hpp"
#include <string>
using namespace std;
void allocatorTest(void) {

    ///* 代码示例1 */
    //::allocator<int> a1;
    //::allocator<int> a2;

    //int* p = a1.allocate(1); // 分配一个int所需要的空间
    //a2.deallocate(p, 1); // 释放由a1分配的空间

    ///* 代码示例2 */
    //std::allocator<std::string> a;
    //std::string* s = a.allocate(1); // 分配一个std::string所需要的空间
    //a.construct(s, "foo"); // 在之前分配的空间中构造一个std::string，其初始值为foo
    //a.destroy(s); // 析构之前构造的std::string，但空间并未释放
    //a.deallocate(s, 1); // 释放最初分配的空间

    //std::allocator<int>                      a1;
    //decltype(a1)::rebind<char>::other        a2; // 相当于std::allocator<char> a2
    //std::allocator<int>::rebind<char>::other a3; // 相当于std::allocator<char> a3


}