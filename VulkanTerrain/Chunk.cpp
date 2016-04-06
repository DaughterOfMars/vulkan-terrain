#include "Chunk.h"

Chunk::Chunk() {

}

Chunk::~Chunk() {

}

void Chunk::loadTextures() {

}

void Chunk::buildCommandBuffers() {

}

void Chunk::buildComputeCommandBuffers() {
	VkCommandBufferBeginInfo cmdBufInfo = vkTools::initializers::commandBufferBeginInfo();

	vkBeginCommandBuffer(computeCmdBuffer, &cmdBufInfo);

	vkCmdBindPipeline(computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelines.compute);
	vkCmdBindDescriptorSets(computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeDescriptorSet, 0, 0);

	vkCmdDispatch(computeCmdBuffer, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);

	vkEndCommandBuffer(computeCmdBuffer);
}

void Chunk::draw() {
	VkSubmitInfo computeSubmitInfo = vkTools::initializers::submitInfo();
	computeSubmitInfo.commandBufferCount = 1;
	computeSubmitInfo.pCommandBuffers = &computeCmdBuffer;

	vkTools::checkResult(vkQueueSubmit(computeQueue, 1, &computeSubmitInfo, VK_NULL_HANDLE));

	vkTools::checkResult(vkQueueWaitIdle(computeQueue));
}

void Chunk::prepareStorageBuffers() {

}