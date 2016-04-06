#pragma once
#pragma comment(linker, "/subsystem:windows")
#include <windows.h>
#include <fcntl.h>
#include <io.h>

#include <iostream>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <string>
#include <array>

#include "vulkan/vulkan.h"

#include "base/vulkantools.h"
#include "base/vulkandebug.h"
#include "base/vulkanTextureLoader.hpp"

#include "base/vulkanswapchain.hpp"

class VulkanBase {
private:
	bool enableValidation = false;
	float fpsTimer = 0.0f;
	VkResult createInstance(bool enableValidation);
	VkResult createDevice(VkDeviceQueueCreateInfo requestedQueues, bool enableValidation);
	std::string getWindowTitle();
protected:
	float frameTimer = 1.0f;
	uint32_t frameCounter = 0;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceMemoryProperties devicememProperties;
	VkDevice device;
	VkQueue queue;
	VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkFormat depthFormat;
	VkCommandPool cmdPool;
	VkCommandBuffer setupCmdBuffer = VK_NULL_HANDLE;
	VkCommandBuffer postPresentCmdBuffer = VK_NULL_HANDLE;
	VkCommandBuffer prePresentCmdBuffer = VK_NULL_HANDLE;
	VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	VkSubmitInfo submitInfo;
	std::vector<VkCommandBuffer> drawCmdBuffers;
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> frameBuffers;
	uint32_t currentBuffer = 0;
	VkDescriptorPool descriptorPool;
	std::vector<VkShaderModule> shaderModules;
	VkPipelineCache pipelineCache;
	VulkanSwapChain swapChain;
	struct {
		VkSemaphore presentComplete;
		VkSemaphore renderComplete;
	} semaphores;
public:
	bool prepared = false;
	uint32_t width = 1280;
	uint32_t height = 720;

	VkClearColorValue defaultClearColor = {0.0f, 0.0f, 0.0f, 1.0f};

	float timer = 0.0f;
	float timerSpeed = 0.25f;
	
	std::string title = "Vulkan Base";
	std::string name = "vulkanBase";

	struct {
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
	} depthStencil;

	HWND window;
	HINSTANCE windowInstance;

	VulkanBase(bool enableValidation);
	VulkanBase() : VulkanBase(false) {};
	~VulkanBase();

	void initVulkan(bool enableValidation);

	void setupConsole(std::string title);
	HWND setupWindow(HINSTANCE hinstance, WNDPROC wndproc);
	virtual void handleMessages(
		HWND hWnd, 
		UINT uMsg, 
		WPARAM wParam, 
		LPARAM lParam);

	virtual void render() = 0;

	virtual void viewChanged();

	void createCommandPool();
	void setupDepthStencil();
	void setupFrameBuffer();
	void setupRenderPass();
	void initSwapChain();
	void setupSwapChain();
	bool checkCommandBuffers();
	void createCommandBuffers();
	void destroyCommandBuffers();
	void createSetupCommandBuffer();
	void flushSetupCommandBuffer();
	void createPipelineCache();
	virtual void prepare();

	VkPipelineShaderStageCreateInfo loadShader(
		std::string fileName, 
		VkShaderStageFlagBits stage);

	VkBool32 createBuffer(
		VkBufferUsageFlags usage,
		VkDeviceSize size,
		void *data,
		VkBuffer *buffer,
		VkDeviceMemory *memory);

	VkBool32 createBuffer(
		VkBufferUsageFlags usage, 
		VkDeviceSize size,
		void *data,
		VkBuffer *buffer,
		VkDeviceMemory *memory,
		VkDescriptorBufferInfo *descriptor);

	void renderLoop();

	void submitPrePresentBarrient(VkImage image);
	void submitPostPresentBarrier(VkImage image);

	VkSubmitInfo prepareSubmitInfo(
		std::vector<VkCommandBuffer> commandBuffers,
		VkPipelineStageFlags *pipelineStages);
};