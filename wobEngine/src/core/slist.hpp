#ifndef WOB_SLIST_HPP
#define WOB_SLIST_HPP

namespace wob
{
	template<typename T>
	class SList
	{
		struct Node
		{
			T data;
			Node* next = nullptr;
		};

	public:

		struct Iterator
		{
			struct Node* n;

			T& operator*() const noexcept
			{
				WOB_BOUNDS_CHECK(n);
				return n->data;
			}

			Iterator& operator++() noexcept
			{
				WOB_BOUNDS_CHECK(n);
				n = n->next;
				return *this;
			}

			bool operator==(Iterator const&) const noexcept = default;
		};

		constexpr SList() noexcept : allocator(context.allocator), first(nullptr)
		{
		}

		constexpr SList(IAllocator* alloc) noexcept : allocator(alloc), first(nullptr)
		{
		}

		constexpr ~SList()
		{
			clear();
		}

		constexpr void clear()
		{
			while (first)
				removeFront();
		}

		constexpr void add(T&& e) noexcept
		{
			if (first == nullptr)
			{
				first = createNode(wob::forward<T>(e));
				return;
			}

			Node* c = first;
			for (; c->next != nullptr; c = c->next)
			{
			}
			c->next = createNode(wob::forward<T>(e));
		}

		constexpr Iterator find(T const& e) const noexcept
		{
			for (Node* c = first; c != nullptr; c = c->next)
			{
				if (c->data == e)
				{
					return Iterator{ c };
				}
			}

			return end();
		}

		template<typename P>
		constexpr Iterator findIf(P&& pred) const noexcept
		{
			for (Node* c = first; c != nullptr; c = c->next)
			{
				if (pred(c->data))
				{
					return Iterator{ c };
				}
			}

			return end();
		}

		constexpr void removeAfter(Iterator const& it) noexcept
		{
			WOB_BOUNDS_CHECK(it.n != nullptr);
			WOB_BOUNDS_CHECK(it.n->next != nullptr);

			auto const* tmp = it.n->next->next;
			deleteNode(it.n->next);
			it.n->next = tmp;
		}

		constexpr void removeFront() noexcept
		{
			WOB_CHECK(first != nullptr);
			Node* second = first->next;
			deleteNode(first);
			if (second)
			{
				first = second;
			}
			else
			{
				first = nullptr;
			}
		}

		template<typename Pred>
		constexpr void removeFirst(Pred&& pred) noexcept
		{
			for (Node** c = &first; *c != nullptr; c = &(*c)->next)
			{
				if (pred(*c))
				{
					Node* nodeToDelete = *c;
					*c = nodeToDelete->next;

					deleteNode(nodeToDelete);
					return;
				}
			}
		}

		constexpr uint32_t size() const noexcept
		{
			uint32_t s = 0;
			for (Node* c = first; c != nullptr; c = c->next)
				s++;
			return s;
		}

		constexpr bool isEmpty() const noexcept
		{
			return first == nullptr;
		}

		constexpr Iterator begin() const noexcept
		{
			return Iterator{ first };
		}

		constexpr Iterator end() const noexcept
		{
			return Iterator{ nullptr };
		}

	private:

		Node* createNode(T&& e)
		{
			Node* n = new(allocator->allocate<Node>()) Node;
			n->data = wob::forward<T>(e);
			n->next = nullptr;
			return n;
		}

		void deleteNode(Node* n)
		{
			n->~Node();
			allocator->deallocate(n);
		}

		IAllocator* allocator;
		Node* first;
	};

}

#endif