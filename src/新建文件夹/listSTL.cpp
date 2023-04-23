#include <iostream>
#define vector_hpp 1
#if vector_hpp
//#include "type_traits.hpp"
#include "myxmemory.hpp"
#include "mylist.hpp"
#else
#include <list>
#endif // vector_hpp
using namespace std;

class String {
public:
	String(const char* str = "") {
		m_data = new char[strlen(str)+1];
		strcpy(m_data, str);
	}
	String(const String& s) {
		m_data = new char[strlen(s.m_data) + 1];
		strcpy(m_data, s.m_data);
	}
	~String() {
		delete []m_data;
		m_data = NULL;
	}
private:
	char* m_data;
};
void listSTL() {
	list<String> mylist;
	String s("abc");
	mylist.push_back(s);
	mylist.erase(mylist.begin(), mylist.end());
	//list<String>::iterator it = mylist.begin();
	//while (it != mylist.end())
	//{
	//	cout << *it << " ";
	//	++it;
	//}
	//cout << endl;
}
/*
void listSTL2() {
    int ar[4] = { 1, 2, 3, 4};
	cout << "start list" << endl;
    list<int> mylist;
	list<int> mylist2(5, 1);
	for (int i = 0; i < 4; i++)
		//mylist.insert(mylist.end(), ar[i]);
		mylist.push_back(ar[i]);

	//mylist.insert(mylist.end(), 2, 5);
	//mylist.insert(mylist.end(), &ar[1], &ar[2]);
	//mylist.insert(mylist.end(), ar, ar+3);
	cout << mylist.front() << endl;
	cout << mylist.back() << endl;
	//mylist.pop_back();
	//mylist.clear();
	//mylist.pop_front();
	//mylist.erase(mylist.begin());
	list<int>::iterator it = mylist.begin();
	while (it != mylist.end())
	{
		cout << *it << " ";
		++it;
	}
	cout << endl;

	list<int>::iterator it2 = mylist2.begin();
	while (it2 != mylist2.end())
	{
		cout << *it2 << " ";
		++it2;
	}
	cout << endl;
}
*/
void listSTL3() {
	int ar[4] = { 1, 2, 3, 4 };
	cout << "start list" << endl;
	list<int> mylist;
	mylist.insert(mylist.begin(), 1);
	list<int>::iterator it = mylist.begin();
	while (it != mylist.end())
	{
		cout << *it << " ";
		++it;
	}
	cout << endl;
}