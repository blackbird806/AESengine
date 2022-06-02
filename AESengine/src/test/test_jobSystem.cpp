#include <iostream>
#include <glm/gtx/matrix_transform_2d.hpp>

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

	auto startupSession = AES_STOP_PROFILE_SESSION();
}
