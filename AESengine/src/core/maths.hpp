#ifndef MATH_HPP
#define MATH_HPP

#include <Eigen/Dense>

namespace aes {
	Eigen::Matrix4f makeOrthographicMatrix(float left, float right, float up, float down, float near, float far);
	Eigen::Matrix4f makePerspectiveMatrix(float fov, float near, float far);
	Eigen::Matrix4f makeLookAtMatrix(Eigen::Vector3f const& pos, Eigen::Vector3f const& up, Eigen::Vector3f const& at);
}


#endif // !MATH_HPP
