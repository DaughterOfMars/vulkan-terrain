#pragma once

#include <glm/glm.hpp>
#include "VulkanBase.h"

class Chunk : VulkanBase {
public:
	static const uint32_t CHUNK_SIZE = 32;

	glm::ivec3 worldPosition;

	Chunk(glm::ivec3 worldPosition) : worldPosition(worldPosition) {};
	Chunk(int worldPosition[3]) : worldPosition(glm::ivec3(worldPosition[0], worldPosition[1], worldPosition[2])) {};
	Chunk(int xpos, int ypos, int zpos) : worldPosition(glm::ivec3(xpos, ypos, zpos)) {};
	~Chunk() {};

	bool IsEmpty();
};