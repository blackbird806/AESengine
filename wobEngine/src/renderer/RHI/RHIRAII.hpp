#ifndef WOB_RHIRAII_HPP
#define WOB_RHIRAII_HPP

#include "core/wob.hpp"
#include "core/allocator.hpp"
#include "core/uniquePtr.hpp"
#include "RHIDevice.hpp"

namespace wob
{
	template<typename T>
	struct RHIRefCounted
	{

	private:

		struct Payload
		{
			T* rhiRessource;
			uint32_t counter = 0;
			IAllocator* allocator;
		};

		void decCounter()
		{
			payload->counter--;
			if (payload->counter == 0)
			{
				payload->rhiRessource->destroy();
				payload->allocator.destroy(payload);
				payload = nullptr;
			}
		}

		void incCounter()
		{
			payload->counter++;
		}

	public:

		RHIRefCounted()
		{
			payload = getContextAllocator()->create<Payload>();
			payload->allocator = getContextAllocator();
			incCounter();
		}

		~RHIRefCounted()
		{
			decCounter();
		}

		T* get() const noexcept
		{
			return &payload->rhiRessource;
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