#ifndef WOB_HASHMAP_HPP
#define WOB_HASHMAP_HPP

#include <concepts>

#include "wob.hpp"
#include "context.hpp"
#include "array.hpp"
#include "hash.hpp"
#include "pair.hpp"
#include "slist.hpp"

namespace wob
{
	template<typename K, typename V, typename H = Hash<K>>
	class HashMap
	{
	public:
		using Key_t = K;
		using Value_t = V;
		using Hash_t = H;
		using Bucket_t = SList<Pair<K, V>>;

		static constexpr float defaultMaxLoadFactor = 0.75;

		struct Iterator
		{
			Pair<K, V>& operator*() const noexcept
			{
				return *currentBucketIt;
			}

			Iterator& operator++() noexcept
			{
				++currentBucketIt;

				// If we finished the current bucket, advance to the next
				if (currentBucketIt == BucketArrayIt->end())
				{
					advanceToNextNonEmptyBucket();
				}

				return *this;
			}

			bool operator==(Iterator const&) const noexcept = default;

			void advanceToNextNonEmptyBucket() noexcept
			{
				++BucketArrayIt;

				// Skip empty buckets
				while (BucketArrayIt != map->buckets.end() && BucketArrayIt->isEmpty())
				{
					++BucketArrayIt;
				}

				// Set position in new bucket (or end if we reached the end)
				if (BucketArrayIt != map->buckets.end())
				{
					currentBucketIt = BucketArrayIt->begin();
				}
				else
				{
					currentBucketIt = typename Bucket_t::Iterator{ nullptr };
				}
			}
			HashMap* map;
			Array<Bucket_t>::Iterator_t BucketArrayIt;
			Bucket_t::Iterator currentBucketIt;
		};

		constexpr HashMap(uint32_t nBuckets = 16) : allocator(context.allocator), size_(0)
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

		constexpr float getLoadFactor() const noexcept
		{
			return (float)size_ / buckets.size();
		}

		constexpr void rehash(uint32_t newBucketCount)
		{
			Array<Bucket_t> newBuckets;
			newBuckets.resize(newBucketCount);
			for (auto& [k, v] : *this)
			{
				add(newBuckets, wob::move(k), wob::move(v));
			}
			buckets = wob::move(newBuckets);
		}

		constexpr void rehash()
		{
			rehash(buckets.size() * 2);
		}

		constexpr void add(K const& key, V const& value) requires std::copy_constructible<V> && std::copy_constructible<K>
		{
			add(buckets, key, value);
			size_++;
			rehashIfNecessary();
		}

		constexpr void add(K&& key, V&& value)
		{
			add(buckets, wob::forward<K>(key), wob::forward<V>(value));
			size_++;
			rehashIfNecessary();
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

		bool tryFind(K const& key, V& value) const requires std::copy_constructible<V>&& std::copy_constructible<K>
		{
			Bucket_t bucket;
			auto const result = getKeyItAndBucketIndex(key);

			if (result.it != buckets[result.bucketIndex].end())
			{
				value = (*result.it).second;
				return true;
			}

			return false;
		}

		constexpr bool exist(K const& key) const
		{
			auto const result = getKeyItAndBucketIndex(key);
			return result.it != buckets[result.bucketIndex].end();
		}

		constexpr V& operator[](K const& key)
		{
			Bucket_t bucket;
			auto const result = getKeyItAndBucketIndex(key);
			if (result.it != buckets[result.bucketIndex].end())
			{
				return (*result.it).second;
			}

			WOB_FATAL_ERROR("Key not present in map");
		}

		constexpr V const& operator[](K const& key) const
		{
			return (*this)[key];
		}

		constexpr Iterator begin()
		{
			uint32_t const idx = getNextNonEmptyBucketIndex(0);
			if (idx == buckets.size())
				return end();

			return Iterator{ this, buckets.begin() + idx, buckets[idx].begin()};
		}

		constexpr Iterator end()
		{
			return Iterator{ this, buckets.end(), buckets[buckets.size()-1].end() };
		}

	private:

		static constexpr void add(Array<Bucket_t>& buckets, K&& key, V&& value)
		{
			auto pair = Pair<K, V>{ wob::forward<K>(key), wob::forward<V>(value) };
			auto const hash = Hash_t{}(pair.first);
			uint32_t const index = hash % buckets.size();
			buckets[index].add(wob::move(pair));
		}

		static constexpr void add(Array<Bucket_t>& buckets, K const& key, V const& value) requires std::copy_constructible<V> && std::copy_constructible<K>
		{
			Pair<K, V> pair{ key, value };
			auto const hash = Hash_t{}(pair.first);
			uint32_t const index = hash % buckets.size();
			buckets[index].add(wob::move(pair));
		}

		constexpr void rehashIfNecessary()
		{
			if (getLoadFactor() > defaultMaxLoadFactor)
			{
				rehash();
			}
		}

		constexpr uint32_t getNextNonEmptyBucketIndex(uint32_t startIndex) const
		{
			for (uint32_t i = startIndex; i < buckets.size(); i++)
			{
				if (!buckets[i].isEmpty())
					return i;
			}
			return buckets.size();
		}

		struct KeySearchResult
		{
			typename Bucket_t::Iterator it;
			uint32_t bucketIndex;
		};

		constexpr KeySearchResult getKeyItAndBucketIndex(K const& key) const
		{
			auto const hash = Hash_t{}(key);
			uint32_t const index = hash % buckets.size();

			auto const it = buckets[index].findIf([key](auto const& pair) {
				return pair.first == key;
				});
			return {it, index};
		}

		IAllocator* allocator;
		Array<Bucket_t> buckets;
		uint32_t size_;
	};
}

#endif