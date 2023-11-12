#ifndef AES_ALLOCATOR_HPP
#define AES_ALLOCATOR_HPP
#include <cstdint>

namespace aes
{
	constexpr unsigned long long operator "" _kb(unsigned long long p)
	{
		return p * 1024;
	}

	constexpr unsigned long long operator "" _mb(unsigned long long p)
	{
		return p * 1024_kb;
	}

	constexpr unsigned long long operator "" _gb(unsigned long long p)
	{
		return p * 1024_mb;
	}

	class IAllocator
	{
	public:
		template<typename T>
		[[nodiscard]] void* allocate(size_t count = 1)
		{
			return allocate(count * sizeof(T), alignof(T));
		}

		[[nodiscard]] virtual void* allocate(size_t size, size_t align = 1) = 0;
		virtual void deallocate(void* ptr) = 0;

		virtual ~IAllocator() {}
	};

	// implementation of IAllocator that is equivalent of just calling malloc
	class Mallocator final : public IAllocator
	{
	public:
		[[nodiscard]] void* allocate(size_t size, size_t align) override;
		void deallocate(void* ptr) override;
	};

	class AllocatorProfiler final : public IAllocator
	{
	public:
		AllocatorProfiler(IAllocator& base);
		[[nodiscard]] void* allocate(size_t size, size_t align) override;
		void deallocate(void* ptr) override;

	private:
		IAllocator* baseAllocator;
	};

	class StackAllocator final : public IAllocator
	{
	public:
		
		StackAllocator(IAllocator& base, size_t size);
		~StackAllocator() override;
		[[nodiscard]] void* allocate(size_t size, size_t align) override;
		void deallocate(void* ptr) override;

		void deallocateFromMarker(size_t marker);
		size_t getMarker() const;
		
	private:
		IAllocator* baseAllocator;
		uint8_t* start;
		size_t totalSize, offset;
	};

	extern Mallocator mallocator;
	
	// used to avoid cicular dependencies in some core headers
	[[nodiscard]] IAllocator* getContextAllocator() noexcept;
}

#endif