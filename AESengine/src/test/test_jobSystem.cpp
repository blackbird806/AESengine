#include <iostream>

#include "tests.hpp"

#include "core/allocator.hpp"
#include "core/utility.hpp"
#include "core/jobSystem.hpp"
#include "core/circularQueue.hpp"

using namespace aes;

void aes::test_jobSystem()
{
	AES_START_PROFILE_SESSION("test font startup");

	JobSystem js(globalAllocator);
	{
		std::atomic<int> sum = 0;
		parallelForEach(js, std::ranges::iota_view(0, 100), [&sum](auto const& e)
			{
				sum += e;
			});
		AES_LOG("sum = {}", sum);
	}
	{
		CircularQueue<int, 10> vals;
		js.run([&vals]()
			{
				int v = 1;
				while (v != 0)
				{
					if (!vals.empty())
					{
						v = vals.pop_front();
						AES_LOG("square {}", v * v);
					}
				}
				AES_LOG("exit");
			});

		int in = 1;
		while (in != 0)
		{
			std::cin >> in;
			vals.push_back(in);
		}
	}
	auto startupSession = AES_STOP_PROFILE_SESSION();
}
