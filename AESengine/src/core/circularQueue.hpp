#ifndef AES_CIRCULAR_QUEUE
#define AES_CIRCULAR_QUEUE

#include "aes.hpp"
#include "error.hpp"
#include <atomic>

namespace aes
{
	template<typename T, uint staticSize>
	class CircularQueue
	{
	public:

		// assume enough size
		void push_back(T const& e) noexcept
		{
			AES_BOUNDS_CHECK((backIdx + 1) % maxSize != frontIdx);
			new (&elements[backIdx]) T(e);
			backIdx = (backIdx + 1) % maxSize;
		}

		void push_back(T&& e) noexcept
		{
			AES_BOUNDS_CHECK((backIdx + 1) % maxSize != frontIdx);
			new (&elements[backIdx]) T(std::move(e));
			backIdx = (backIdx + 1) % maxSize;
		}

		T pop_back() noexcept
		{
			AES_BOUNDS_CHECK(!empty());
			T e = elements[backIdx];
			backIdx = (backIdx - 1) > 0 ? backIdx - 1 : maxSize - 1;
			return e;
		}

		T pop_front() noexcept
		{
			AES_BOUNDS_CHECK(!empty());
			T e = elements[frontIdx];
			frontIdx = (frontIdx + 1) % maxSize;
			return e;
		}

		void push_front(T const& e) noexcept
		{
			AES_BOUNDS_CHECK(frontIdx == 0 ? maxSize - 1 : frontIdx - 1 != backIdx);
			new (&elements[frontIdx]) T(e);
			frontIdx = frontIdx == 0 ? maxSize - 1 : frontIdx - 1;
		}

		void push_front(T&& e) noexcept
		{
			AES_BOUNDS_CHECK(frontIdx == 0 ? maxSize - 1 : frontIdx - 1 != backIdx);
			new (&elements[frontIdx]) T(std::move(e));
			frontIdx = frontIdx == 0 ? maxSize - 1 : frontIdx - 1;
		}

		uint size() const noexcept
		{
			if (backIdx >= frontIdx)
				return backIdx - frontIdx;

			return backIdx + (maxSize - frontIdx);
		}

		bool empty() const noexcept
		{
			return backIdx == frontIdx;
		}

	private:
		// add 1 element (always empty) which is used to determine if the queue is full or empty
		static constexpr uint maxSize = staticSize + 1;

		std::atomic<uint> frontIdx = 0, backIdx = 0;
		T elements[maxSize]; 
	};
}

#endif