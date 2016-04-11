#include "VulkanTerrain.h"

VulkanTerrain::VulkanTerrain(bool enableValidation) {
	meshRenderer = new Mesh(enableValidation);
}

VulkanTerrain::~VulkanTerrain() {

}

void VulkanTerrain::loadMesh() {
	std::vector<Vertex> vertexBuffer;
	std::vector<uint32_t> indexBuffer;

	std::vector<Chunk> chunks;
	int pos[3] = { (int)meshRenderer->cam->pos.x%Chunk::CHUNK_SIZE, (int)meshRenderer->cam->pos.y%Chunk::CHUNK_SIZE, (int)meshRenderer->cam->pos.z%Chunk::CHUNK_SIZE };
	for(int x = pos[0] - 10*Chunk::CHUNK_SIZE, 
			y = pos[1] - 10*Chunk::CHUNK_SIZE, 
			z = pos[2] - 10*Chunk::CHUNK_SIZE;
			x < pos[0] + 10*Chunk::CHUNK_SIZE && 
			y < pos[1] + 10*Chunk::CHUNK_SIZE && 
			z < pos[2] + 10*Chunk::CHUNK_SIZE;){ 
		chunks.push_back(Chunk(x, y, z));
	}
	for (Chunk c : chunks) {
		// TODO: run compute shader here
		readStorageBuffers(vertexBuffer, indexBuffer);
	}

	createBuffer(
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		vertexBuffer.size() * sizeof(Vertex),
		vertexBuffer.data(),
		&meshRenderer->meshes.terrain.vertices.buf,
		&meshRenderer->meshes.terrain.vertices.mem);

	createBuffer(
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		indexBuffer.size() * sizeof(uint32_t),
		indexBuffer.data(),
		&meshRenderer->meshes.terrain.indices.buf,
		&meshRenderer->meshes.terrain.indices.mem);

	meshRenderer->meshes.terrain.indexCount = indexBuffer.size();

	vertexBuffer.clear();
	indexBuffer.clear();
}

void VulkanTerrain::loadTextures() {

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
	uint32_t vertexBufferMaxSize = 32*32*32*3 * sizeof(Vertex);
	uint32_t indexBufferMaxSize = 32*32*32*5*3 * sizeof(uint32_t);
	VkMemoryAllocateInfo memAlloc = vkTools::initializers::memoryAllocateInfo();
	VkMemoryRequirements memReqs;

	VkBufferCreateInfo vBufferInfo =
		vkTools::initializers::bufferCreateInfo(
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			vertexBufferMaxSize);
	VkBufferCreateInfo iBufferInfo =
		vkTools::initializers::bufferCreateInfo(
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			indexBufferMaxSize);

	// Create a buffer on the GPU to hold the data
	vkTools::checkResult(vkCreateBuffer(device, &vBufferInfo, nullptr, &storageBuffers.vertex_buffer.buffer));
	// Get the memory needed by this buffer
	vkGetBufferMemoryRequirements(device, storageBuffers.vertex_buffer.buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAlloc.memoryTypeIndex);
	// Allocate space for buffer on GPU and store the address in stagingBuffer.memory
	vkTools::checkResult(vkAllocateMemory(device, &memAlloc, nullptr, &storageBuffers.vertex_buffer.memory));
	// Bind the buffer
	vkTools::checkResult(vkBindBufferMemory(device, storageBuffers.vertex_buffer.memory, storageBuffers.vertex_buffer.memory, 0));

	// Repeat for index buffer
	vkTools::checkResult(vkCreateBuffer(device, &iBufferInfo, nullptr, &storageBuffers.index_buffer.buffer));
	vkGetBufferMemoryRequirements(device, storageBuffers.index_buffer.buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAlloc.memoryTypeIndex);
	vkTools::checkResult(vkAllocateMemory(device, &memAlloc, nullptr, &storageBuffers.index_buffer.memory));
	vkTools::checkResult(vkBindBufferMemory(device, storageBuffers.index_buffer.memory, storageBuffers.index_buffer.memory, 0));
}

void VulkanTerrain::readStorageBuffers(std::vector<Vertex> & vertexBuffer_complete, std::vector<uint32_t> & indexBuffer_complete) {
	std::vector<Vertex> vertexBuffer;
	std::vector<uint32_t> indexBuffer;
	uint32_t vertexBufferMaxSize = 32 * 32 * 32 * 3 * sizeof(Vertex);
	uint32_t indexBufferMaxSize = 32 * 32 * 32 * 5 * 3 * sizeof(uint32_t);
	VkMemoryAllocateInfo memAlloc = vkTools::initializers::memoryAllocateInfo();
	VkMemoryRequirements memReqs;

	void *data;

	struct StagingBuffer {
		VkDeviceMemory memory;
		VkBuffer buffer;
	} vertexReadBuffer, indexReadBuffer;

	VkBufferCreateInfo vBufferInfo =
		vkTools::initializers::bufferCreateInfo(
			VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			vertexBufferMaxSize);
	VkBufferCreateInfo iBufferInfo =
		vkTools::initializers::bufferCreateInfo(
			VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			indexBufferMaxSize);

	// Create local buffers to copy data from the GPU into
	vkTools::checkResult(vkCreateBuffer(device, &vBufferInfo, nullptr, &vertexReadBuffer.buffer));
	vkGetBufferMemoryRequirements(device, vertexReadBuffer.buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAlloc.memoryTypeIndex);
	vkTools::checkResult(vkAllocateMemory(device, &memAlloc, nullptr, &vertexReadBuffer.memory));
	vkTools::checkResult(vkBindBufferMemory(device, vertexReadBuffer.buffer, vertexReadBuffer.memory, 0));

	vkTools::checkResult(vkCreateBuffer(device, &iBufferInfo, nullptr, &indexReadBuffer.buffer));
	vkGetBufferMemoryRequirements(device, indexReadBuffer.buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAlloc.memoryTypeIndex);
	vkTools::checkResult(vkAllocateMemory(device, &memAlloc, nullptr, &indexReadBuffer.memory));
	vkTools::checkResult(vkBindBufferMemory(device, indexReadBuffer.buffer, indexReadBuffer.memory, 0));

	createSetupCommandBuffer();

	VkBufferCopy copyRegion = {};
	copyRegion.size = vertexBufferMaxSize;
	vkCmdCopyBuffer(
		setupCmdBuffer,
		storageBuffers.vertex_buffer.buffer,
		vertexReadBuffer.buffer,
		1,
		&copyRegion);

	copyRegion.size = indexBufferMaxSize;
	vkCmdCopyBuffer(
		setupCmdBuffer,
		storageBuffers.index_buffer.buffer,
		indexReadBuffer.buffer,
		1,
		&copyRegion);

	flushSetupCommandBuffer();

	// Copy data from the GPU buffer to local vectors
	vkTools::checkResult(vkMapMemory(device, vertexReadBuffer.memory, 0, vertexBufferMaxSize, 0, &data));
	vertexBuffer.resize(vertexBufferMaxSize);
	memcpy(&vertexBuffer[0], data, vertexBufferMaxSize);
	vkUnmapMemory(device, vertexReadBuffer.memory);

	vkTools::checkResult(vkMapMemory(device, indexReadBuffer.memory, 0, indexBufferMaxSize, 0, &data));
	vertexBuffer.resize(indexBufferMaxSize);
	memcpy(&vertexBuffer[0], data, indexBufferMaxSize);
	vkUnmapMemory(device, indexReadBuffer.memory);

	for (auto v : vertexBuffer) {
		if (&v != nullptr)
			vertexBuffer_complete.push_back(v);
	}
	for (auto i : indexBuffer) {
		if (&i != nullptr)
			indexBuffer_complete.push_back(i + vertexBuffer_complete.size());
	}

	// Cleanup buffers
	vkDestroyBuffer(device, vertexReadBuffer.buffer, nullptr);
	vkFreeMemory(device, vertexReadBuffer.memory, nullptr);
	vkDestroyBuffer(device, indexReadBuffer.buffer, nullptr);
	vkFreeMemory(device, indexReadBuffer.memory, nullptr);
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
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1),
		vkTools::initializers::descriptorSetLayoutBinding(
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
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