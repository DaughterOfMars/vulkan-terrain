#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 up;

	Camera(float width, float height) {
		Camera(width, height, glm::vec3(), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	}
	Camera(float width, float height, glm::vec3 pos, glm::vec3 dir, glm::vec3 up) : pos(pos), dir(dir), up(up) {
		view = glm::lookAt(pos, pos + dir, up);
		projection = glm::perspective(glm::radians(50.0f), width / height, 0.1f, 256.0f);
	};
	~Camera() {

	}

	glm::mat4 GetProjection() {
		return projection;
	}

	glm::mat4 GetView() {
		return view;
	}

	void update() {
		
	}
};