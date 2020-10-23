#include "camera.hpp"
#include "core/maths.hpp"

using namespace aes;

void Camera::lookAt(Eigen::Vector3f const& at)
{
	viewMatrix = makeLookAtMatrix(pos, { 0.0, 1.0, 0.0 }, at);
}
