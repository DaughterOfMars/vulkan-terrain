#pragma once

#include "VulkanBase.h"
#include "Camera.hpp"

#define KEYBOARD_C 0x43
#define KEYBOARD_P 0x50
#define KEYBOARD_W 0x57
#define KEYBOARD_A 0x41
#define KEYBOARD_S 0x53
#define KEYBOARD_D 0x44

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
	RECT rect;
	glm::vec2 centerPos;

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
	float mouseDelta[2] = { 0.0f };

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
	void viewChanged();
	void updateCamera();
public:
	void handleMessages(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);
};