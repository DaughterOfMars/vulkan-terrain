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
		vkTools::VulkanTexture dirt;
		vkTools::VulkanTexture grass;
	} uboTextures;

	struct {
		vkTools::UniformData mvp;
		vkTools::UniformData textures;
	} uniformData;

	struct {
		VkPipeline render;
	} pipelines;

	struct {
		glm::vec4 pos;
		glm::vec4 dir;
		glm::vec4 up;
	} cam;

	VkPipelineLayout pipelineLayout;
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout;

	float moveSpeed;
	float sprintSpeed;

	void loadTextures();
	void buildCommandBuffers();
	void draw();
	void setupDescriptorPool();
	void setupDescriptorSetLayout();
	void setupDescriptorSet();
	void preparePipelines();
	void prepareUniformBuffers();
	void updateUniformBuffers();
	void prepare();
	virtual void render();
	void handleMessages();
};