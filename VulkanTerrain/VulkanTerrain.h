#pragma once

#include "VulkanBase.h"

struct Vertex {
	float pos[3];
	float norm[2];
};

struct CompressedPt {
	uint32_t pt;
};

class VulkanTerrain : VulkanBase {
public:
	VulkanTerrain(bool enableValidation);
	~VulkanTerrain();
private:
	struct MeshBufferInfo
	{
		VkBuffer buf = VK_NULL_HANDLE;
		VkDeviceMemory mem = VK_NULL_HANDLE;
	};

	struct MeshBuffer
	{
		MeshBufferInfo vertices;
		MeshBufferInfo indices;
		uint32_t indexCount;
	};

	struct {
		VkPipelineVertexInputStateCreateInfo inputState;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	} vertices;

	struct {
		MeshBuffer terrain;
	} meshes;

	struct {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	} uboMVP;

	struct {
		vkTools::VulkanTexture triTable;
		vkTools::VulkanTexture dirt;
		vkTools::VulkanTexture grass;
	} uboTextures;

	struct {
		glm::ivec3 worldPos;
	} uboCompute;

	struct {
		vkTools::UniformData mvp;
		vkTools::UniformData textures;
		vkTools::UniformData compute;
	} uniformData;

	struct {
		VkPipeline compute;
		VkPipeline render;
	} pipelines;

	void loadTextures();
	void buildCommandBuffers();
	void buildComputeCommandBuffers();
	void draw();
	void prepareStorageBuffers();
	//void setupVertexDescriptions();
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
	void handleMessages();
};