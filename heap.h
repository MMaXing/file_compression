#pragma once
#include <vector>
#include <assert.h>

template <class T>
struct Less
{ 
	bool operator() (const T& left,const T& right)
	{
		return left < right;
	} 
};

template <class T>
struct Greater
{ 
	bool operator() (const T& left,const T& right)
	{
		return left > right;
	} 
};

template <class T, class Compare = Less<T>>
class Heap
{
public:
	Heap()
	{}
	Heap(const T* a,size_t size)
	{
		for(size_t i=0;i<size;i++)
		{
			_arrays.push_back(a[i]);
		}
		//建堆
		for(int i=(size-2)/2;i >= 0;i--)
			 Heapify(i);
	}


	void Push(const T& x)
	{
		_arrays.push_back(x);
		AdjustUp(_arrays.size()-1);
	}

	void Pop()
	{
		swap(_arrays[0],_arrays[_arrays.size()-1]);
		_arrays.pop_back();
		Heapify(0);
	}
	T& Top()
	{
		assert(_arrays.size());
		return _arrays[0];
	}
	bool Empty()
	{
		return _arrays.empty ();
	}

	int Size()
	{
		return _arrays.size ();
	}
	void  Heapify(int root) //堆化
	{
		size_t child = root*2+1;
		Compare com;
		while(child < _arrays.size())
		{
			if(child+1 < _arrays.size() && com(_arrays[child+1],_arrays[child]))
				child++;
			if(com(_arrays[child],_arrays[root]))
			{
				swap(_arrays[child],_arrays[root]);
				root = child;
				child = root*2 +1;
			}
			else
				break;
		}

	}
	void AdjustUp(int child) //向上调整
	{
		int parent = (child-1)/2;
		while(child > 0)
		{
			if(Compare()(_arrays[child],_arrays[parent]))
			{
				swap(_arrays[child],_arrays[parent]);
				child = parent;
				parent = (child-1)/2;
			}
			else
				break;
		}
	}
	
	void Print()
	{
		for(size_t i=0;i<_arrays.size();i++)
			cout<<_arrays[i]<<" ";
		cout<<endl;
	}
public:
	vector <T> _arrays ;
};
