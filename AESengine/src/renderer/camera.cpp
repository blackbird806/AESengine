#include "camera.hpp"
#include "core/maths.hpp"

#include <glm/ext.hpp>

using namespace aes;

void Camera::lookAt(glm::vec3 const& at)
{
	viewMatrix = glm::lookAtLH(pos, at, { 0.0f, 1.0f, 0.0f });
}
