#pragma once

#include "VulkanBase.h"
#include "Camera.hpp"

struct Vertex {
	float pos[3];
	float norm[3];
};

class Mesh : public VulkanBase {
public:
	Mesh(bool enableValidation);
	~Mesh();
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

public:
	struct {
		MeshBuffer terrain;
	} meshes;

	Camera *cam;
	HWND winHandle;

private:
	struct {
		VkPipelineVertexInputStateCreateInfo inputState;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	} vertices;

	struct {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	} uboMVP;

	struct {
		vkTools::VulkanTexture dirt;
		vkTools::VulkanTexture grass;
	} textures;

	struct {
		vkTools::UniformData mvp;
	} uniformData;

	struct {
		VkPipeline render;
	} pipelines;

	VkPipelineLayout pipelineLayout;
	VkDescriptorSet descriptorSetPostCompute;
	VkDescriptorSetLayout descriptorSetLayout;

	float moveSpeed;
	float sprintSpeed;

	bool keyboardState[256] = { false };

	void loadTextures();
	void buildCommandBuffers();
	void draw();
	void setupVertexDescriptions();
	void setupDescriptorPool();
	void setupDescriptorSetLayout();
	void setupDescriptorSet();
	void preparePipelines();
	void prepareUniformBuffers();
	void updateUniformBuffers();
	void prepare();
	virtual void render();
public:
	void handleMessages(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);
};