#ifndef AES_ALLOCATOR_HPP
#define AES_ALLOCATOR_HPP

#include "core/aes.hpp"
#include <memory_resource>

namespace aes
{
	inline unsigned operator "" _ko(unsigned long long p)
	{
		return p * 1024;
	}

	inline unsigned operator "" _mo(unsigned long long p)
	{
		return p * 1024_ko;
	}

	inline unsigned operator "" _go(unsigned long long p)
	{
		return p * 1024_mo;
	}

	class MemoryProfiler
	{
		
	public:
		static MemoryProfiler& instance()
		{
			static MemoryProfiler prof;
			return prof;
		}

		void profileAlloc(void* ptr, size_t size);
		void profileDealloc(void* ptr){};
		
	private:
		size_t memoryAllocated = 0;
		size_t nbAllocations = 0;
	};

	class IAllocator : public std::pmr::memory_resource
	{
	public:
		[[nodiscard]] virtual void* allocate(size_t size, size_t align = 1) = 0;
		virtual void deallocate(void* ptr) = 0;

		void* do_allocate(size_t size, size_t align) override;
		void do_deallocate(void* ptr, size_t size, size_t align) override;
		bool do_is_equal(const memory_resource& rhs) const noexcept override;

		virtual ~IAllocator() {}
	};

	class Mallocator final : public IAllocator
	{
	public:
		[[nodiscard]] void* allocate(size_t size, size_t align) override;
		void deallocate(void* ptr) override;
	};

	class StackAllocator final : public IAllocator
	{
	public:
		
		StackAllocator(IAllocator& base, size_t size);
		~StackAllocator() override;
		[[nodiscard]] void* allocate(size_t size, size_t align) override;
		void deallocate(void* ptr) override { AES_UNUSED(ptr); }

		void deallocateFromMarker(size_t marker);
		size_t getMarker() const;
		
	private:
		IAllocator* baseAllocator;
		uint8_t* start;
		size_t totalSize, offset;
	};

	extern Mallocator globalAllocator;
}

#endif