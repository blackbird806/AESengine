#ifndef WOB_REFCOUNTED_HPP
#define WOB_REFCOUNTED_HPP

#include "core/wob.hpp"
#include "core/allocator.hpp"
#include "core/uniquePtr.hpp"
#include "RHIDevice.hpp"

namespace wob
{
	template<typename T>
	struct InternalRHIDeviceDeleter
	{
		template <class>
		friend struct InternalRHIDeviceDeleter;

		InternalRHIDeviceDeleter() noexcept = default;
		InternalRHIDeviceDeleter(RHIDevice* a) noexcept : device(a) {}

		template<typename T2>
		InternalRHIDeviceDeleter(InternalRHIDeviceDeleter<T2> const& rhs) noexcept : device(rhs.device) {}

		RHIDevice* device = nullptr;
		void operator()(T* res) noexcept
		{
			device->destroyRessource<T>(*res);
		}
	};

	template<typename T>
	using RHIUnique = UniquePtr<T, InternalRHIDeviceDeleter<T>>;

	template<typename T, typename D>
	auto RHIMakeUnique(RHIDevice& device, D const& desc)
	{

	}

	template<typename T>
	struct RHIRefCounted
	{

	private:

		struct Payload
		{
			T* rhiRessource;
			RHIDevice* device;
			uint32_t counter = 0;
			IAllocator* allocator;
		};

		void decCounter()
		{
			payload->counter--;
			if (payload->counter == 0)
			{
				device->destroyRessource<T>(*payload->rhiRessource);
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