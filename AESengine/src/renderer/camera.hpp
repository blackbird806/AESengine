#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <Eigen/Dense>

namespace aes {

	class Camera
	{
	public:
		
		void lookAt(Eigen::Vector3f const& at);
		Eigen::Vector3f pos;
		Eigen::Matrix4f viewMatrix;
	};

}

#endif