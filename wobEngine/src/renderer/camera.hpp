#ifndef AES_CAMERA_HPP
#define AES_CAMERA_HPP

#include <glm/glm.hpp>

// Sample camera class, should be deprecated when the new renderer will be advanced enough
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