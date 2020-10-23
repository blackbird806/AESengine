#include "maths.hpp"
#include <cmath>

Eigen::Matrix4f aes::makeOrthographicMatrix(float left, float right, float up, float down, float near, float far)
{
	Eigen::Matrix4f ortho;
	ortho << 
		2.0f / (right - left), 0, 0, -(right + left) / (right - left),
		0, 2 / (up - down), 0, -(up + down) / (up - down),
		0, 0, -2 / (far - near), -(far + near) / (far - near),
		0, 0, 0, 1;
	return ortho;
}

Eigen::Matrix4f aes::makePerspectiveMatrix(float fov, float near, float far)
{
	Eigen::Matrix4f perspective;
	float const scale = 1 / tan(fov * 0.5 * M_PI / 180);
	perspective(0,0) = scale; // scale the x coordinates of the projected point 
	perspective(1,1) = scale; // scale the y coordinates of the projected point 
	perspective(2,2) = -far / (far - near); // used to remap z to [0,1] 
	perspective(3,2) = -far * near / (far - near); // used to remap z [0,1] 
	perspective(2,3) = -1; // set w = -z 
	perspective(3,3) = 0;
	return perspective;
}

Eigen::Matrix4f aes::makeLookAtMatrix(Eigen::Vector3f const& pos, Eigen::Vector3f const& up, Eigen::Vector3f const& at)
{
	Eigen::Matrix4f lookAt;
	Eigen::Matrix3f R;
	R.col(2) = (pos - at).normalized();
	R.col(0) = up.cross(R.col(2)).normalized();
	R.col(1) = R.col(2).cross(R.col(0));
	lookAt.topLeftCorner<3, 3>() = R.transpose();
	lookAt.topRightCorner<3, 1>() = -R.transpose() * pos;
	lookAt.row(3) << 0, 0, 0, 1;
    return lookAt;
}
