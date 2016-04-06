#pragma once

#include <glm/glm.hpp>
#include "VulkanBase.h"

class Chunk : VulkanBase {
public:
	static const uint32_t CHUNK_SIZE = 32;

	Chunk();
	~Chunk();

	bool IsEmpty();
};