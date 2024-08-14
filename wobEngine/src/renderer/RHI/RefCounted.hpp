#ifndef AES_REFCOUNTED_HPP
#define AES_REFCOUNTED_HPP

#include "core/aes.hpp"
#include "core/allocator.hpp"
#include "scoped.hpp"

namespace aes
{
	template<Destructible T>
	struct RefCounted
	{

	private:

		struct Payload
		{
			T element;
			uint32_t counter = 0;
			IAllocator* allocator;
		};

		void decCounter()
		{
			payload->counter--;
			if (payload->counter == 0)
			{
				payload->element.destroy(); // explicit call to destroy in case if destructors do not release resources in the future
				payload->allocator.destroy(payload);
				payload = nullptr;
			}
		}

		void incCounter()
		{
			payload->counter++;
		}

	public:

		RefCounted()
		{
			payload = getContextAllocator()->create<Payload>();
			payload->allocator = getContextAllocator();
			incCounter();
		}

		~RefCounted()
		{
			decCounter();
		}

		T* get() const noexcept
		{
			return &payload->element;
		}

		T& operator*() const noexcept
		{
			return *get();
		}

		T* operator->() const noexcept
		{
			return get();
		}

	private:
		Payload* payload;
	};
}

#endif