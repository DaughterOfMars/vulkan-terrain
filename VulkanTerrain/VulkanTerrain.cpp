#include "VulkanTerrain.h"

VulkanTerrain::VulkanTerrain(bool enableValidation) : VulkanBase(enableValidation) {
	width = 800;
	height = 600;
	title = "3D Graph Visualizer";

	cam.pos = glm::vec4(-25.0f, -25.0f, 25.0f, 1.0f);
	cam.dir = glm::vec4(25.0f, 25.0f, 25.0f, 1.0f) - cam.pos;
	cam.up = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	moveSpeed = 50.0f;
	sprintSpeed = 100.0f;
}

VulkanTerrain::~VulkanTerrain() {
	// Clean up resources

}

void VulkanTerrain::loadTextures() {

}

void VulkanTerrain::buildCommandBuffers() {

}

void VulkanTerrain::draw() {

}

void VulkanTerrain::setupDescriptorPool() {

}

void VulkanTerrain::setupDescriptorSetLayout() {

}

void VulkanTerrain::setupDescriptorSet() {

}

void VulkanTerrain::preparePipelines() {

}

void VulkanTerrain::prepareUniformBuffers() {

}

void VulkanTerrain::updateUniformBuffers() {

}

void VulkanTerrain::prepare() {

}

void VulkanTerrain::render() {

}

void VulkanTerrain::handleMessages() {

}