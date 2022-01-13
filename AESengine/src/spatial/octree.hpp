#ifndef OCTREE_HPP
#define OCTREE_HPP
#include <vector>

namespace aes
{
	template<template<typename, typename> typename Container, template<typename> typename Allocator = std::allocator>
	class Octree
	{
		struct Node
		{
			Node* next;
			Node* prev;
			void* userData;
		};

		Container<Node, Allocator<Node>> data;
	};
}

#endif