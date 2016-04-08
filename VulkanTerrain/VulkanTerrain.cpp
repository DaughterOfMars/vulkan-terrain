#include "VulkanTerrain.h"

VulkanTerrain::VulkanTerrain() {

}

VulkanTerrain::~VulkanTerrain() {

}

void VulkanTerrain::buildComputeCommandBuffer() {
	// Only need to define one command buffer for compute pass, 
	// as there are no framebuffers
	VkCommandBufferBeginInfo cmdBufInfo = vkTools::initializers::commandBufferBeginInfo();

	vkBeginCommandBuffer(computeCmdBuffer, &cmdBufInfo);

	vkCmdBindPipeline(computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelines.compute);
	vkCmdBindDescriptorSets(computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeDescriptorSet, 0, 0);

	vkCmdDispatch(computeCmdBuffer, Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE);

	vkEndCommandBuffer(computeCmdBuffer);
}

void VulkanTerrain::draw() {
	VkSubmitInfo computeSubmitInfo = vkTools::initializers::submitInfo();
	computeSubmitInfo.commandBufferCount = 1;
	computeSubmitInfo.pCommandBuffers = &computeCmdBuffer;

	vkTools::checkResult(vkQueueSubmit(queue, 1, &computeSubmitInfo, VK_NULL_HANDLE));

	vkTools::checkResult(vkQueueWaitIdle(queue));
}

void VulkanTerrain::prepareStorageBuffers() {

}

void VulkanTerrain::setupDescriptorPool() {
	std::vector<VkDescriptorPoolSize> poolSizes = {
		vkTools::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
		vkTools::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2),
		vkTools::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
	};

	VkDescriptorPoolCreateInfo descriptorPoolInfo =
		vkTools::initializers::descriptorPoolCreateInfo(
			poolSizes.size(),
			poolSizes.data(),
			1);

	vkTools::checkResult(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool));
}

void VulkanTerrain::setupDescriptorSetLayout() {
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
		vkTools::initializers::descriptorSetLayoutBinding(
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			0),
		vkTools::initializers::descriptorSetLayoutBinding(
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1),
		vkTools::initializers::descriptorSetLayoutBinding(
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			2),
		vkTools::initializers::descriptorSetLayoutBinding(
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			3)
	};

	VkDescriptorSetLayoutCreateInfo descriptorLayout =
		vkTools::initializers::descriptorSetLayoutCreateInfo(
			setLayoutBindings.data(),
			setLayoutBindings.size());

	vkTools::checkResult(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &computeDescriptorSetLayout));

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
		vkTools::initializers::pipelineLayoutCreateInfo(
			&computeDescriptorSetLayout,
			1);

	vkTools::checkResult(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &computePipelineLayout));
}

void VulkanTerrain::setupDescriptorSet() {
	VkDescriptorSetAllocateInfo allocInfo =
		vkTools::initializers::descriptorSetAllocateInfo(
			descriptorPool,
			&computeDescriptorSetLayout,
			1);

	vkTools::checkResult(vkAllocateDescriptorSets(device, &allocInfo, &computeDescriptorSet));

	VkDescriptorImageInfo texDescriptor =
		vkTools::initializers::descriptorImageInfo(
			textures.triTable.sampler,
			textures.triTable.view,
			VK_IMAGE_LAYOUT_GENERAL);

	std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
		vkTools::initializers::writeDescriptorSet(
			computeDescriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&uniformData.compute.descriptor),
		vkTools::initializers::writeDescriptorSet(
			computeDescriptorSet,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			1,
			&storageBuffers.vertex_buffer.descriptor),
		vkTools::initializers::writeDescriptorSet(
			computeDescriptorSet,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			2,
			&storageBuffers.index_buffer.descriptor),
		vkTools::initializers::writeDescriptorSet(
			computeDescriptorSet,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			3,
			&texDescriptor)
	};

	vkUpdateDescriptorSets(device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, NULL);
}

void VulkanTerrain::createComputeCommandBuffer() {
	VkCommandBufferAllocateInfo cmdBufAllocateInfo =
		vkTools::initializers::commandBufferAllocateInfo(
			cmdPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1);

	vkTools::checkResult(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &computeCmdBuffer));
}

void VulkanTerrain::preparePipeline() {
	VkComputePipelineCreateInfo computePipelineCreateInfo =
		vkTools::initializers::computePipelineCreateInfo(
			computePipelineLayout,
			0);
	computePipelineCreateInfo.stage = loadShader("./../data/shaders/BuildMesh.comp.spv", VK_SHADER_STAGE_COMPUTE_BIT);
	vkTools::checkResult(vkCreateComputePipelines(device, pipelineCache, 1, &computePipelineCreateInfo, nullptr, &pipelines.compute));
}

void VulkanTerrain::prepareUniformBuffers() {
	createBuffer(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		sizeof(uboCompute),
		&uboCompute,
		&uniformData.compute.buffer,
		&uniformData.compute.memory,
		&uniformData.compute.descriptor);

	updateUniformBuffers();
}

void VulkanTerrain::updateUniformBuffers() {
	uboCompute.worldPos = currentChunk->worldPosition;
	uint8_t *pData;
	vkTools::checkResult(vkMapMemory(device, uniformData.compute.memory, 0, sizeof(uboCompute), 0, (void**)&pData));
	memcpy(pData, &uboCompute, sizeof(uboCompute));
	vkUnmapMemory(device, uniformData.compute.memory);
}

void VulkanTerrain::getComputeQueue() {
	uint32_t queueIndex = 0;
	uint32_t queueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
	assert(queueCount > 0);

	std::vector<VkQueueFamilyProperties> queueProperties;
	queueProperties.resize(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProperties.data());

	for (queueIndex = 0; queueIndex < queueCount; ++queueIndex)
		if (queueProperties[queueIndex].queueFlags & VK_QUEUE_COMPUTE_BIT)
			break;
	assert(queueIndex < queueCount);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.queueFamilyIndex = queueIndex;
	queueCreateInfo.queueCount = 1;
	vkGetDeviceQueue(device, queueIndex, 0, &queue);
}

void VulkanTerrain::prepare() {
	if (enableValidation)
		vkDebug::setupDebugging(instance, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, NULL);
	createCommandPool();
	createSetupCommandBuffer();
	setupSwapChain();
	setupRenderPass();
	createPipelineCache();
	flushSetupCommandBuffer();
	createSetupCommandBuffer();
	textureLoader = new vkTools::VulkanTextureLoader(physicalDevice, device, queue, cmdPool);

	loadTextures();
	getComputeQueue();
	createComputeCommandBuffer();
	prepareStorageBuffers();
	prepareUniformBuffers();
	setupDescriptorSetLayout();
	preparePipeline();
	setupDescriptorPool();
	setupDescriptorSet();
	buildComputeCommandBuffer();
	prepared = true;
}

void VulkanTerrain::render() {
	if (!prepared)
		return;
	vkDeviceWaitIdle(device);
	draw();
	vkDeviceWaitIdle(device);
}