#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	Camera();
	~Camera();

	void SetProjection(glm::mat4 newProj);

	glm::mat4 GetProjection();

	glm::mat4 GetWorldViewFrustum();

	glm::mat4 GetViewFrustum();

private:
	glm::mat4 projection;
};