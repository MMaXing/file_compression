#pragma once
#include "heap.h"
#include<assert.h>

template <class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode<T>* _left;
	HuffmanTreeNode<T>* _right;
	HuffmanTreeNode<T>* _parent;

	T _weight;

	HuffmanTreeNode(const T& x)
		:_left(NULL)
		,_right(NULL)
		,_parent(NULL)
		,_weight(x)
	{}

};

template <class T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;

public:
	HuffmanTree()
		:_root(NULL)
	{}
	
	~HuffmanTree()
	{
		Destroy(_root);
		_root = NULL;
	}

	void Destroy(Node* root)
	{
		if(!root)
			return;
		Destroy(root->_left);
		Destroy(root->_right);
		delete root;
	}

	Node* GetRoot()
	{
		return _root;
	}

	template <class T>
	struct NodeCompare
	{
		bool operator()(Node* left,Node* right)
		{
			return left->_weight < right->_weight;
		}
	};

public:
	void CreateTree(const T* a,size_t size,const T& invalid)
	{
		assert(a);
		Heap<Node*,NodeCompare<T>> minheap;
		for (size_t i = 0; i < size; ++i)  
        {  
			if(a[i] != invalid)
			{
				Node* node = new Node(a[i]);  
				minheap.Push(node);
			}
        }  
		
		while(minheap.Size() > 1)
		{
			Node* left = minheap.Top();
			minheap.Pop();
			Node* right = minheap.Top();
			minheap.Pop();

			Node* parent = new Node(left->_weight + right->_weight);
			parent->_left = left;
			left->_parent = parent;
			parent->_right = right;
			right->_parent = parent;
			minheap.Push(parent);
		}
		_root = minheap.Top();
	}

	void Printf()
	{
		_Printf(_root);
		cout<<endl;
	}
	void _Printf(Node* root)
	{
		if(root)
		{
			if(!root->_left)
			cout <<root->_weight<< " ";
			_Printf(root->_left);
			_Printf(root->_right);
		}
	}


private:
	HuffmanTreeNode<T>*  _root;
};
