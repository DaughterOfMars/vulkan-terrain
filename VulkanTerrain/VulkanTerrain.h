#pragma once

#include "VulkanBase.h"
#include "Chunk.hpp"
#include "Mesh.h"

class VulkanTerrain : VulkanBase {
public:
	struct {
		glm::ivec3 worldPos;
	} uboCompute;

	struct {
		vkTools::UniformData compute;
	} uniformData;

	struct {
		vkTools::VulkanTexture triTable;
	} textures;

	struct {
		vkTools::UniformData vertex_buffer;
		vkTools::UniformData index_buffer;
	} storageBuffers;

	struct {
		VkPipeline render;
		VkPipeline compute;
	} pipelines;

	VkQueue computeQueue;
	VkCommandBuffer computeCmdBuffer;
	VkPipelineLayout computePipelineLayout;
	VkDescriptorSet computeDescriptorSet;
	VkDescriptorSetLayout computeDescriptorSetLayout;

	Chunk *currentChunk;
	Mesh *meshRenderer;

	VulkanTerrain(bool enableValidation);
	~VulkanTerrain();

	void loadTextures();
	void buildComputeCommandBuffer();
	void draw();
	void prepareStorageBuffers();
	void setupDescriptorPool();
	void setupDescriptorSetLayout();
	void setupDescriptorSet();
	void preparePipeline();
	void createComputeCommandBuffer();
	void prepareUniformBuffers();
	void updateUniformBuffers();
	void getComputeQueue();
	void prepare();
	void render();
};

