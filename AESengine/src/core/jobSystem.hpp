#ifndef AES_JOBSYSTEM_HPP
#define AES_JOBSYSTEM_HPP

#include "aes.hpp"
#include "error.hpp"
#include "array.hpp"
#include "allocator.hpp"
#include "utility.hpp"
#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <ranges>
#include <latch>

namespace aes
{
	template<typename T>
	class LfreeQueue
	{
	public:

		LfreeQueue(IAllocator& alloc, uint32_t size) : allocator(&alloc)
		{
			data = allocator->allocate<T>(size);
			end = data + size;
			head = 0;
			tail = 1;
		}

		uint32_t capacity() const noexcept
		{
			return end - data;
		}

		uint32_t adjacentNext(uint32_t i) const
		{
			return (i + 1) % capacity();
		}

		Result<void> enqueue(T const& value)
		{
			if (!isIndexEmpty(adjacentNext(tail)))
				return { AESError::Undefined };

		}

	private:

		bool isIndexEmpty(uint32_t i) const
		{
			AES_ASSERT(i < capacity());
			if (tail > head)
			{
				return i > tail || i < head;
			}
			return i > tail && i < head;
		}

		IAllocator* allocator;
		std::atomic<T*> data;
		std::atomic<T*> end;
		std::atomic<uint32_t> head, tail;
	};

	struct Job
	{
		Job(std::function<void()>&& fn) noexcept : task(fn)
		{
			
		}

		Job(Job const& rhs) noexcept : task(rhs.task), done(rhs.done.load(std::memory_order::relaxed))
		{}

		Job& operator=(Job const& rhs)
		{
			task = rhs.task;
			done = rhs.done.load(std::memory_order::relaxed);
			return *this;
		}

		std::function<void()> task;
		std::atomic<bool> done = false;
	};

	class JobSystem
	{
	public:

		static constexpr uint32_t nbWorkers = 4;

		JobSystem(IAllocator& allocator) : jobs(allocator), threads(allocator)
		{
			threads.resize(nbWorkers);
			for (uint32_t i = 0; i < nbWorkers; i++)
			{
				threads[i] = std::jthread([this]()
				{
					while (!stop.load(std::memory_order::memory_order_relaxed))
					{
						std::shared_ptr<Job> job;
						{
							std::unique_lock l(sync);
							if (!jobs.empty())
							{
								job = jobs.back();
								jobs.pop();
							}
						}
						if (job)
						{
							job->task();
							job->done.store(true, std::memory_order::memory_order_relaxed);
						}
					}
				});
			}
		}

		template<typename F>
		std::shared_ptr<Job> run(F&& f)
		{
			std::unique_lock l(sync);
			jobs.push(std::make_shared<Job>(std::move(f)));
			return jobs.back();
		}

		void wait()
		{
			while (true)
			{
				if (sync.try_lock())
				{
					AES_SCOPE(sync.unlock());
					if (jobs.empty())
						return;
				}
			}
		}

		~JobSystem()
		{
			stop.store(true, std::memory_order::memory_order_relaxed);
		}

	private:
		std::atomic<bool> stop = false;
		std::mutex sync;
		Array<std::shared_ptr<Job>> jobs;
		Array<std::jthread> threads;
	};

	template<typename F, std::ranges::random_access_range R>
	void parallelForEach(JobSystem& js, R&& range, F const& f)
	{
		size_t const size = std::ranges::size(range);
		size_t const sizePerWorker = size / JobSystem::nbWorkers;
		std::latch latch(JobSystem::nbWorkers);
		for (size_t i = 0; i < JobSystem::nbWorkers; i++)
		{
			js.run([=, &latch]() mutable 
				{
					auto const begin = std::ranges::begin(range) + sizePerWorker * i;
					auto const end = [&]()
					{
						if (i == JobSystem::nbWorkers-1)
							return std::ranges::end(range);
						return begin + sizePerWorker;
					}();

					for (auto e = begin; e != end; ++e)
					{
						f(*e);
					}
					latch.count_down();
				});
		}
		latch.wait();
	}

}

#endif