#pragma once

#include "VulkanBase.h"
#include "Chunk.hpp"
#include "Mesh.h"
#include "MarchingCubesLookup.h"

class VulkanTerrain : public VulkanBase {
public:
	const uint32_t VISIBILITY_DISTANCE = 8;
	//                            chunks in x axis              chunks in y axis              chunks in z axis
	const uint32_t CHUNK_COUNT = (2 * VISIBILITY_DISTANCE + 1) * (2 * VISIBILITY_DISTANCE + 1) * (VISIBILITY_DISTANCE + 1);
	
	struct {
		glm::ivec3 worldPos;
	} uboCompute;

	typedef int table[256][16];
	struct {
		table * triTable;
	} uboLookup;

	struct {
		vkTools::UniformData compute;
		vkTools::UniformData lookup;
	} uniformData;

	struct {
		vkTools::UniformData vertex_buffer;
		vkTools::UniformData index_buffer;
	} storageBuffers;

	struct {
		VkPipeline compute;
	} pipelines;

	VkQueue computeQueue;
	VkCommandBuffer computeCmdBuffer;
	VkPipelineLayout computePipelineLayout;
	VkDescriptorSet computeDescriptorSet;
	VkDescriptorSetLayout computeDescriptorSetLayout;

	Mesh *meshRenderer;

	VulkanTerrain(bool enableValidation);
	~VulkanTerrain();

	void loadMesh();
	void buildComputeCommandBuffer();
	void draw();
	void prepareStorageBuffers();
	void readStorageBuffers(std::vector<Vertex> &vertexBuffer_complete, std::vector<uint32_t> &indexBuffer_complete);
	void setupDescriptorPool();
	void setupDescriptorSetLayout();
	void setupDescriptorSet();
	void preparePipeline();
	void createComputeCommandBuffer();
	void prepareUniformBuffers();
	void updateUniformBuffers(Chunk currentChunk);
	void getComputeQueue();
	void prepare();
	void render();
	void compute();
};

