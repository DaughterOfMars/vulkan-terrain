#pragma once

#include <glm/glm.hpp>
#include "VulkanBase.h"

class Chunk : VulkanBase {
public:
	static const uint32_t CHUNK_SIZE = 32;

	struct {
		vkTools::VulkanTexture triTable;
	} uboTextures;

	struct {
		glm::ivec3 worldPos;
	} uboCompute;

	struct {
		vkTools::UniformData textures;
		vkTools::UniformData compute;
	} uniformData;

	struct {
		VkPipeline compute;
	} pipelines;

	struct {
		vkTools::UniformData vertex_buffer;
		vkTools::UniformData index_buffer;
	} storageBuffers;

	VkQueue computeQueue;
	VkCommandBuffer computeCmdBuffer;
	VkPipelineLayout computePipelineLayout;
	VkDescriptorSet computeDescriptorSet;
	VkDescriptorSetLayout computeDescriptorSetLayout;

	Chunk();
	~Chunk();

	bool IsEmpty();

	void loadTextures();
	void buildCommandBuffers();
	void buildComputeCommandBuffers();
	void draw();
	void prepareStorageBuffers();
	void setupDescriptorPool();
	void setupDescriptorSetLayout();
	void setupDescriptorSet();
	void createComputeCommandBuffer();
	void preparePipelines();
	void prepareCompute();
	void prepareUniformBuffers();
	void updateUniformBuffers();
	void getComputeQueue();
	void prepare();
	virtual void render();
};