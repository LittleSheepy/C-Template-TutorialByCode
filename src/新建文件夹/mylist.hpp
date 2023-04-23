#pragma once
#include "myxmemory.hpp"
#include <assert.h>
template<class _Ty, class _A=::allocator<_Ty> >
class list
{
public:
	struct _Node;
	typedef _Node* _Nodeptr;	//  typedef list<_Ty>::_Node*   list<_Ty>::_Nodeptr
	struct _Node
	{
		_Ty _Value;
		_Nodeptr _Next, _Prev;
	};
	struct _Acc {
		typedef _Nodeptr& _Nodepref;	// 节点的指针引用 typedef list<_Ty>::_Node *&  list<_Ty>::_Acc::_Nodepref
		typedef _Ty& _Vref;				// 值的引用
		static _Nodepref _Next(_Nodeptr _P) {
			return (_Nodepref)(*_P)._Next;		// _P->_Next
		}
		static _Nodepref _Prev(_Nodeptr _P) {
			return (_Nodepref)(*_P)._Prev;		// _P->_Prev
		}
		static _Vref _Value(_Nodeptr _P) {
			return (_Vref)(*_P)._Value;		// _P->_Value
		}
	};
	// 萃取
	typedef  _A::size_type           size_type;
	typedef  _A::difference_type     difference_type;
	typedef  _A::pointer             pointer;
	typedef  _A::const_pointer       const_pointer;
	typedef  _A::reference           reference;
	typedef  _A::const_reference     const_reference;
	typedef  _A::value_type          value_type;

	class iterator;
	class const_iterator;
	typedef const_iterator _It;

	class const_iterator
	{
	public:
		const_iterator()
		{}
		const_iterator(_Nodeptr _P) : _Ptr(_P)
		{}
		const_iterator(const_iterator& _X) : _Ptr(_X._Ptr)
		{}
		const_reference operator*()const
		{
			return _Acc::_Value(_Ptr);
		}

		//++it   it++
		const_iterator operator++()
		{
			_Ptr = _Acc::_Next(_Ptr); // _Ptr = _Ptr->_Next;
			return *this;
		}
		const_iterator operator++(int)
		{
			const_iterator _Tmp = *this;
			++* this;
			return _Tmp;
		}
		const_iterator operator--()
		{
			_Ptr = _Acc::_Prev(_Ptr);
			return *this;
		}
		const_iterator operator--(int)
		{
			const_iterator _Tmp = *this;
			--* this;
			return _Tmp;
		}
		bool operator==(const const_iterator& _X)const
		{
			return _Ptr == _X._Ptr;
		}
		bool operator!=(const const_iterator& _X)const
		{
			return !(*this == _X);
		}
		_Nodeptr _Mynode()const {
			return _Ptr;
		}
	protected:
		_Nodeptr _Ptr;
	};

	class iterator : public const_iterator
	{
	public:
		iterator()
		{}
		iterator(_Nodeptr _P) : const_iterator(_P)
		{}
		reference operator*()
		{
			return _Acc::_Value(_Ptr);
		}

		//++it   it++
		iterator operator++()
		{
			_Ptr = _Acc::_Next(_Ptr); // _Ptr = _Ptr->_Next;
			return *this;
		}
		iterator operator++(int)
		{
			iterator _Tmp = *this;
			++* this;
			return _Tmp;
		}
		iterator operator--()
		{
			_Ptr = _Acc::_Prev(_Ptr);
			return *this;
		}
		iterator operator--(int)
		{
			const_iterator _Tmp = *this;
			--* this;
			return _Tmp;
		}
		bool operator==(const iterator& _X)const
		{
			return _Ptr == _X._Ptr;
		}
		bool operator!=(const iterator& _X)const
		{
			return !(*this == _X);
		}
	};
public:
	explicit list() :_Head(_Buynode()), _Size(0) {}
	explicit list(size_type _N, const _Ty& _V = _Ty()) :_Head(_Buynode()), _Size(0) {
		insert(begin(), _N, _V);
	}
	explicit list(const _Ty* _F, const _Ty* _L) :_Head(_Buynode()), _Size(0) {
		insert(begin(), _F, _L);
	}
	explicit list(_It _F, _It _L) :_Head(_Buynode()), _Size(0) {
		insert(begin(), _F, _L);
	}
public:
	iterator begin()
	{
		return iterator(_Acc::_Next(_Head));
	}
	const_iterator begin()const
	{
		return const_iterator(_Acc::_Next(_Head));
	}
	iterator end()
	{
		return iterator(_Head);
	}
	const_iterator end()const
	{
		return const_iterator(_Head);
	}
public:
	size_type size() const {
		return _Size;
	}
	bool empty() const {
		return (size() == 0);
	}
	reference front() {
		return *begin();
	}
	reference back() {
		return *(--end());
	}
	void push_back(const_reference v) {
		//_Nodeptr _S = _Buynode(_Head, _Acc::_Prev(_Head));
		//_S->_Value = v;
		//_Acc::_Next(_Acc::_Prev(_S)) = _S;
		//_Acc::_Prev(_Head) = _S;
		//_Size++;
		insert(end(), v);
	}
	void push_front(const_reference v) {
		insert(begin(), v);
	}
	void pop_back() {
		erase(--end());
	}
	void pop_front() {
		erase(begin());
	}
	~list() {
		erase(begin(), end());
		_Freenode(_Head);
		_Head = 0;
		_Size = 0;
	}
public:
	void assign(_It _F, _It _L) {
		erase(begin(), end());
		insert(_F, _L);
	}
	void assign(size_type _N, const _Ty& _X = _Ty()) {
		erase(begin(), end());
		insert(begin(), _N, _X);
	}
	iterator insert_old(iterator _P, const _Ty& _X = _Ty()) {
		_Nodeptr _Ptr = _P._Mynode();
		_Nodeptr _S = _Buynode(_Ptr, _Acc::_Prev(_Ptr));
		_Acc::_Prev(_Ptr) = _S;
		_Acc::_Next(_Acc::_Prev(_S)) = _S;

		_Acc::_Value(_S) = _X;
		_Size++;
		return iterator(_S);
	}
	iterator insert(iterator _P, const _Ty& _X = _Ty()) {
		_Nodeptr _S = _P._Mynode();
		_Acc::_Prev(_S) = _Buynode(_S, _Acc::_Prev(_S));
		_S = _Acc::_Prev(_S);
		_Acc::_Next(_Acc::_Prev(_S)) = _S;

		/////////////////////////
		//_Acc::_Value(_S) = _X;
		allocator.construct(&_Acc::_Value(_S), _X);
		/////////////////////////
		_Size++;
		return iterator(_S);
	}
	void insert(iterator _P, size_type _M, const _Ty& _X) {
		for (; 0 < _M; --_M)
			insert(_P, _X);
	}
	// 插入指针之间的内容
	void insert(iterator _P, const _Ty* _F, const _Ty* _L) {
		for (; _F != _L; ++_F) {
			insert(_P, *_F);
		}
	}
	void insert(iterator _P, _It _F, _It _L) {
		for (; _F != _L; ++_F)
			insert(_P, *_F);
	}
	iterator erase(iterator _P) {
		_Nodeptr _S = (_P++)._Mynode();
		_Acc::_Next(_Acc::_Prev(_S)) = _Acc::_Next(_S);
		_Acc::_Prev(_Acc::_Next(_S)) = _Acc::_Prev(_S);

		/////////////////////////
		// free(_S);  // 只释放节点, 没有释放对象空间
		allocator.destroy(&_Acc::_Value(_S));	// 释放空间，_S->_Value
		_Freenode(_S);
		/////////////////////////
		--_Size;
		return _P;
	}
	iterator erase(iterator _F, iterator _L) {
		while (_F != _L)
			erase(_F++);
		return _F;
	}
	void clear() {
		erase(begin(), end());
	}
protected:
	// _Narg后继参数  _Parg前继参数
	_Nodeptr _Buynode(_Nodeptr _Narg=0, _Nodeptr _Parg=0) {
		//_Nodeptr _S = (_Nodeptr)malloc(sizeof(_Node));		// C
		//_Nodeptr _S = (_Nodeptr)operator new(sizeof(_Node));	// C++
		//cout << sizeof(_Node::_Next) << sizeof(_Node::_Value) << sizeof(_Node) << endl;
		_Nodeptr _S = (_Nodeptr)allocator._Charalloc(sizeof(_Node));
		assert(_S != NULL);
		//_S->_Next = _Narg != 0 ? _Narg : _S;
		//_S->_Prev = _Parg != 0 ? _Parg : _S;
		_Acc::_Next(_S) = _Narg != 0 ? _Narg : _S;
		_Acc::_Prev(_S) = _Parg != 0 ? _Parg : _S;
		return _S;
	}
	void _Freenode(_Nodeptr _S)
	{
		allocator.deallocate(_S, 1);
		//operatror delete(_S);  // 同上
	}
private:
	_A allocator;
	_Nodeptr _Head;
	size_type _Size;
};

//list::list()
//{
//}
//
//list::~list()
//{
//}