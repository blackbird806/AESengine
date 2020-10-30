#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

namespace aes {

	class Camera
	{
	public:
		
		void lookAt(glm::vec3 const& at);

		glm::vec3 pos;
		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;
	};

}

#endif