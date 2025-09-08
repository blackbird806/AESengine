#ifndef AES_HASHMAP_HPP
#define AES_HASHMAP_HPP

#include "aes.hpp"
#include "context.hpp"
#include "array.hpp"
#include "hash.hpp"

namespace aes
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
				AES_ASSERT(n);
				return n->data;
			}

			Iterator& operator++() noexcept
			{
				AES_ASSERT(n);
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

		constexpr void add(T&& e) noexcept
		{
			if (first == nullptr)
			{
				first = createNode(std::forward<T>(e));
				return;
			}

			Node* c = first;
			for (; c != nullptr; c = c->next)
			{ }
			c->next = createNode(std::forward<T>(e));
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
			AES_BOUNDS_CHECK(it.n != nullptr);
			auto const* tmp = it.n->next->next;
			deleteNode(it.n->next);
			it.n->next = tmp;
		}

		template<typename Pred>
		constexpr void removeFirst(Pred&& pred) noexcept
		{
			Node** prev = nullptr;
			for (Node** c = &first; *c != nullptr; c = &(*c)->next)
			{
				if (pred(*c))
				{
					if (prev)
						(*prev)->next = (*c)->next;
					deleteNode(*c);
					*c = nullptr;
					return;
				}
				prev = c;
			}
		}

		constexpr uint32_t size() const noexcept
		{
			uint32_t s = 0;
			for (Node* c = first; c != nullptr; c = c->next)
				s++;
			return s;
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
			n->data = std::forward<T>(e);
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

	template<typename T1, typename T2>
	struct Pair
	{
		T1 first;
		T2 second;
	};

	template<typename K, typename V, typename H = Hash<K>>
	class HashMap
	{
	public:
		using Key_t = K;
		using Value_t = V;
		using Hash_t = H;
		using BucketList_t = Array<SList<Pair<K, V>>>;

		// this is pure shit
		// TODO restart from scratch
		struct Iterator
		{
			Pair<K, V>& operator*() const noexcept
			{
				return *currentBucketIt;
			}

			Iterator& operator++() noexcept
			{
				start:
				if (currentBucketIt == typename SList<Pair<K, V>>::Iterator{nullptr })
				{
					while (currentBucketIt == typename SList<Pair<K, V>>::Iterator{ nullptr })
					{
						bucketListIndex++;
						if (bucketListIndex < map->buckets.size())
						{
							currentBucketIt = map->buckets[bucketListIndex].begin();
						}
						else
						{
							currentBucketIt = typename SList<Pair<K, V>>::Iterator{ nullptr };
						}
					}
				}
				else
				{
					++currentBucketIt;
					goto start; // bruh
				}

				return *this;
			}

			bool operator==(Iterator const&) const noexcept = default;

			HashMap* map;
			int32_t bucketListIndex;
			SList<Pair<K, V>>::Iterator currentBucketIt;
		};

		constexpr HashMap(uint32_t nBuckets) : allocator(context.allocator), size_(0)
		{
			buckets.resize(nBuckets);
		}

		constexpr HashMap(IAllocator* alloc, uint32_t nBuckets) : allocator(alloc), size_(0)
		{
			buckets.resize(nBuckets);
		}

		constexpr uint32_t size() const noexcept
		{
			return size_;
		}

		constexpr void rehash(uint32_t newBucketCount)
		{
			BucketList_t newBuckets;
			newBuckets.resize(newBucketCount);
		}

		constexpr void add(K&& key, V&& value)
		{
			auto pair = Pair<K, V>{ std::forward<K>(key), std::forward<V>(value) };
			auto const hash = Hash_t{}(pair.first);
			uint32_t const index = hash % buckets.size();
			buckets[index].add(std::move(pair));
			size_++;
		}

		constexpr void remove(K const& key)
		{
			auto const hash = Hash_t{}(key);
			uint32_t const index = hash % buckets.size();

			buckets[index].removeFirst([key](auto const& node) {
				return node->data.first == key;
				});
			size_--;
		}

		bool tryFind(K const& key, V& value) const
		{
			auto const hash = Hash_t{}(key);
			uint32_t const index = hash % buckets.size();

			auto const it = buckets[index].findIf([key](auto const& pair) {
				return pair.first == key;
				});

			if (it != buckets[index].end())
			{
				value = (*it).second;
				return true;
			}

			return false;
		}

		constexpr V& operator[](K const& key)
		{
			auto const hash = Hash_t{}(key);
			uint32_t const index = hash % buckets.size();
			
			auto const it = buckets[index].findIf([key](auto const& pair) {
				return pair.first == key;
			});

			if (it != buckets[index].end())
				return (*it).second;
			
			AES_FATAL_ERROR("Key not present in map");
		}

		constexpr V const& operator[](K const& key) const
		{
			return (*this)[key];
		}

		constexpr Iterator begin()
		{
			Iterator it = Iterator{ this, -1, buckets.begin()->begin() };
			return ++it;
		}

		constexpr Iterator end()
		{
			return Iterator{ this, (int32_t)buckets.size(), typename SList<Pair<K, V>>::Iterator{ nullptr } };
		}

	private:

		IAllocator* allocator;
		BucketList_t buckets;
		uint32_t size_;
	};
}

#endif