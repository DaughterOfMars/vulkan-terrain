#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define FORWARD glm::normalize(dir)
#define BACKWARD glm::normalize(-dir)
#define LEFT glm::normalize(glm::cross(up, dir))
#define RIGHT glm::normalize(glm::cross(dir, up))

#define UAXIS glm::cross(dir, up)
#define VAXIS up
#define NAXIS dir

enum Direction {
	Forward,
	Backward,
	Left,
	Right
};

enum Axis {
	U, V, N
};

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
		view = glm::lookAt(pos, pos + dir, up);
	}

	void translate(Direction direction, float magnitude) {
		switch (direction) {
		case Forward:
			pos = glm::translate(glm::mat4(), magnitude*FORWARD)*glm::vec4(pos.x, pos.y, pos.z, 1.0f);
			break;
		case Backward:
			pos = glm::translate(glm::mat4(), magnitude*BACKWARD)*glm::vec4(pos.x, pos.y, pos.z, 1.0f);
			break;
		case Left:
			pos = glm::translate(glm::mat4(), magnitude*LEFT)*glm::vec4(pos.x, pos.y, pos.z, 1.0f);
			break;
		case Right:
			pos = glm::translate(glm::mat4(), magnitude*RIGHT)*glm::vec4(pos.x, pos.y, pos.z, 1.0f);
			break;
		}
		update();
	}

	void rotate(Axis axis, float angle) {
		// rotate dir about axis by angle
		switch (axis) {
		case U:
			dir = glm::rotate(glm::mat4(), angle, UAXIS) * glm::vec4(dir.x, dir.y, dir.z, 1.0f);
			break;
		case V:
			dir = glm::rotate(glm::mat4(), angle, VAXIS) * glm::vec4(dir.x, dir.y, dir.z, 1.0f);
			break;
		case N:
			dir = glm::rotate(glm::mat4(), angle, NAXIS) * glm::vec4(dir.x, dir.y, dir.z, 1.0f);
			break;
		}
		update();
	}
};