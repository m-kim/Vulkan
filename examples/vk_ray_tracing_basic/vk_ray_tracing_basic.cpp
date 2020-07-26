/*
* Vulkan Example - Basic example for ray tracing using VK_NV_ray_tracing
*
* Copyright (C) by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <assert.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

#include "vulkanexamplebase.h"
#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"

//// Ray tracing acceleration structure
//struct AccelerationStructure {
//	VkDeviceMemory memory;
//	VkAccelerationStructureNV accelerationStructure;
//	uint64_t handle;
//};

// Ray tracing acceleration structure
struct AccelerationStructureKHR
{
	VkDeviceMemory            memory;
	VkAccelerationStructureKHR accelerationStructure;
	uint64_t                  handle;
};

// Ray tracing geometry instance
struct GeometryInstance {
	glm::mat3x4 transform;
	uint32_t instanceId : 24;
	uint32_t mask : 8;
	uint32_t instanceOffset : 24;
	uint32_t flags : 8;
	uint64_t accelerationStructureHandle;
};

// Indices for the different ray tracing shader types used in this example
#define INDEX_RAYGEN 0
#define INDEX_MISS 1
#define INDEX_CLOSEST_HIT 2

#define NUM_SHADER_GROUPS 3
static PFN_vkCreateAccelerationStructureKHR                  pfn_vkCreateAccelerationStructureKHR                  = 0;
static PFN_vkDestroyAccelerationStructureKHR                 pfn_vkDestroyAccelerationStructureKHR                 = 0;
static PFN_vkGetAccelerationStructureMemoryRequirementsKHR   pfn_vkGetAccelerationStructureMemoryRequirementsKHR   = 0;
static PFN_vkBindAccelerationStructureMemoryKHR              pfn_vkBindAccelerationStructureMemoryKHR              = 0;
static PFN_vkCmdBuildAccelerationStructureKHR                pfn_vkCmdBuildAccelerationStructureKHR                = 0;
static PFN_vkCmdBuildAccelerationStructureIndirectKHR        pfn_vkCmdBuildAccelerationStructureIndirectKHR        = 0;
static PFN_vkBuildAccelerationStructureKHR                   pfn_vkBuildAccelerationStructureKHR                   = 0;
static PFN_vkCopyAccelerationStructureKHR                    pfn_vkCopyAccelerationStructureKHR                    = 0;
static PFN_vkCopyAccelerationStructureToMemoryKHR            pfn_vkCopyAccelerationStructureToMemoryKHR            = 0;
static PFN_vkCopyMemoryToAccelerationStructureKHR            pfn_vkCopyMemoryToAccelerationStructureKHR            = 0;
static PFN_vkWriteAccelerationStructuresPropertiesKHR        pfn_vkWriteAccelerationStructuresPropertiesKHR        = 0;
static PFN_vkCmdCopyAccelerationStructureKHR                 pfn_vkCmdCopyAccelerationStructureKHR                 = 0;
static PFN_vkCmdCopyAccelerationStructureToMemoryKHR         pfn_vkCmdCopyAccelerationStructureToMemoryKHR         = 0;
static PFN_vkCmdCopyMemoryToAccelerationStructureKHR         pfn_vkCmdCopyMemoryToAccelerationStructureKHR         = 0;
static PFN_vkCmdTraceRaysKHR                                 pfn_vkCmdTraceRaysKHR                                 = 0;
static PFN_vkCreateRayTracingPipelinesKHR                    pfn_vkCreateRayTracingPipelinesKHR                    = 0;
static PFN_vkGetRayTracingShaderGroupHandlesKHR              pfn_vkGetRayTracingShaderGroupHandlesKHR              = 0;
static PFN_vkGetAccelerationStructureDeviceAddressKHR        pfn_vkGetAccelerationStructureDeviceAddressKHR        = 0;
static PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR pfn_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = 0;
static PFN_vkCmdWriteAccelerationStructuresPropertiesKHR     pfn_vkCmdWriteAccelerationStructuresPropertiesKHR     = 0;
static PFN_vkCmdTraceRaysIndirectKHR                         pfn_vkCmdTraceRaysIndirectKHR                         = 0;
static PFN_vkGetDeviceAccelerationStructureCompatibilityKHR  pfn_vkGetDeviceAccelerationStructureCompatibilityKHR  = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateAccelerationStructureKHR(
    VkDevice                                    device,
    const VkAccelerationStructureCreateInfoKHR *pCreateInfo,
    const VkAllocationCallbacks *               pAllocator,
    VkAccelerationStructureKHR *                pAccelerationStructure)
{
	assert(pfn_vkCreateAccelerationStructureKHR);
	return pfn_vkCreateAccelerationStructureKHR(device, pCreateInfo, pAllocator, pAccelerationStructure);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyAccelerationStructureKHR(
    VkDevice                     device,
    VkAccelerationStructureKHR   accelerationStructure,
    const VkAllocationCallbacks *pAllocator)
{
	assert(pfn_vkDestroyAccelerationStructureKHR);
	pfn_vkDestroyAccelerationStructureKHR(device, accelerationStructure, pAllocator);
}
VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureMemoryRequirementsKHR(
    VkDevice                                                device,
    const VkAccelerationStructureMemoryRequirementsInfoKHR *pInfo,
    VkMemoryRequirements2 *                                 pMemoryRequirements)
{
	assert(pfn_vkGetAccelerationStructureMemoryRequirementsKHR);
	pfn_vkGetAccelerationStructureMemoryRequirementsKHR(device, pInfo, pMemoryRequirements);
}
VKAPI_ATTR VkResult VKAPI_CALL vkBindAccelerationStructureMemoryKHR(
    VkDevice                                        device,
    uint32_t                                        bindInfoCount,
    const VkBindAccelerationStructureMemoryInfoKHR *pBindInfos)
{
	assert(pfn_vkBindAccelerationStructureMemoryKHR);
	return pfn_vkBindAccelerationStructureMemoryKHR(device, bindInfoCount, pBindInfos);
}
VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructureKHR(
    VkCommandBuffer                                         commandBuffer,
    uint32_t                                                infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR *     pInfos,
    const VkAccelerationStructureBuildOffsetInfoKHR *const *ppOffsetInfos)
{
	assert(pfn_vkCmdBuildAccelerationStructureKHR);
	pfn_vkCmdBuildAccelerationStructureKHR(commandBuffer, infoCount, pInfos, ppOffsetInfos);
}
VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysKHR(
    VkCommandBuffer                 commandBuffer,
    const VkStridedBufferRegionKHR *pRaygenShaderBindingTable,
    const VkStridedBufferRegionKHR *pMissShaderBindingTable,
    const VkStridedBufferRegionKHR *pHitShaderBindingTable,
    const VkStridedBufferRegionKHR *pCallableShaderBindingTable,
    uint32_t                        width,
    uint32_t                        height,
    uint32_t                        depth)
{
	assert(pfn_vkCmdTraceRaysKHR);
	pfn_vkCmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingShaderGroupHandlesKHR(
    VkDevice   device,
    VkPipeline pipeline,
    uint32_t   firstGroup,
    uint32_t   groupCount,
    size_t     dataSize,
    void *     pData)
{
	assert(pfn_vkGetRayTracingShaderGroupHandlesKHR);
	return pfn_vkGetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesKHR(
    VkDevice                                 device,
    VkPipelineCache                          pipelineCache,
    uint32_t                                 createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR *pCreateInfos,
    const VkAllocationCallbacks *            pAllocator,
    VkPipeline *                             pPipelines)
{
	assert(pfn_vkCreateRayTracingPipelinesKHR);
	return pfn_vkCreateRayTracingPipelinesKHR(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}
vk::DynamicLoader dl;
class VulkanExample : public VulkanExampleBase
{
public:

	//PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
	//PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
	//PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
	//PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;
	//PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
	//PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
	//PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
	//PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
	//PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;

	//VkPhysicalDeviceRayTracingPropertiesNV rayTracingProperties{};
  //AccelerationStructure bottomLevelAS;
	//AccelerationStructure topLevelAS;

	VkPhysicalDeviceRayTracingPropertiesKHR rayTracingProperties{};
  AccelerationStructureKHR               bottomLevelASKHR;
	AccelerationStructureKHR topLevelASKHR;

	std::array<VkRayTracingShaderGroupCreateInfoKHR, NUM_SHADER_GROUPS> m_shaderGroups{};

	vks::Buffer vertexBuffer;
	vks::Buffer indexBuffer;
	uint32_t indexCount;
	vks::Buffer shaderBindingTable;

	struct StorageImage {
		VkDeviceMemory memory;
		VkImage image;
		VkImageView view;
		VkFormat format;
	} storageImage;

	struct UniformData {
		glm::mat4 viewInverse;
		glm::mat4 projInverse;
	} uniformData;
	vks::Buffer ubo;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout;

	VulkanExample() : VulkanExampleBase()
	{
		title = "VK_NV_ray_tracing";
		settings.overlay = true;
		camera.type = Camera::CameraType::lookat;
		camera.setPerspective(60.0f, (float)width / (float)height, 0.1f, 512.0f);
		camera.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		camera.setTranslation(glm::vec3(0.0f, 0.0f, -2.5f));
		// Enable instance and device extensions required to use VK_NV_ray_tracing
		enabledInstanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		enabledDeviceExtensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
		enabledDeviceExtensions.push_back(VK_NV_RAY_TRACING_EXTENSION_NAME);
	}

	~VulkanExample()
	{
		vkDestroyPipeline(device, pipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		vkDestroyImageView(device, storageImage.view, nullptr);
		vkDestroyImage(device, storageImage.image, nullptr);
		vkFreeMemory(device, storageImage.memory, nullptr);
		vkFreeMemory(device, bottomLevelASKHR.memory, nullptr);
		vkFreeMemory(device, topLevelASKHR.memory, nullptr);
		vkDestroyAccelerationStructureKHR(device, bottomLevelASKHR.accelerationStructure, nullptr);
		vkDestroyAccelerationStructureKHR(device, topLevelASKHR.accelerationStructure, nullptr);
		vertexBuffer.destroy();
		indexBuffer.destroy();
		shaderBindingTable.destroy();
		ubo.destroy();
	}

	/*
		Set up a storage image that the ray generation shader will be writing to
	*/
	void createStorageImage()
	{
		VkImageCreateInfo image = vks::initializers::imageCreateInfo();
		image.imageType = VK_IMAGE_TYPE_2D;
		image.format = swapChain.colorFormat;
		image.extent.width = width;
		image.extent.height = height;
		image.extent.depth = 1;
		image.mipLevels = 1;
		image.arrayLayers = 1;
		image.samples = VK_SAMPLE_COUNT_1_BIT;
		image.tiling = VK_IMAGE_TILING_OPTIMAL;
		image.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VK_CHECK_RESULT(vkCreateImage(device, &image, nullptr, &storageImage.image));

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(device, storageImage.image, &memReqs);
		VkMemoryAllocateInfo memoryAllocateInfo = vks::initializers::memoryAllocateInfo();
		memoryAllocateInfo.allocationSize = memReqs.size;
		memoryAllocateInfo.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &storageImage.memory));
		VK_CHECK_RESULT(vkBindImageMemory(device, storageImage.image, storageImage.memory, 0));

		VkImageViewCreateInfo colorImageView = vks::initializers::imageViewCreateInfo();
		colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorImageView.format = swapChain.colorFormat;
		colorImageView.subresourceRange = {};
		colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorImageView.subresourceRange.baseMipLevel = 0;
		colorImageView.subresourceRange.levelCount = 1;
		colorImageView.subresourceRange.baseArrayLayer = 0;
		colorImageView.subresourceRange.layerCount = 1;
		colorImageView.image = storageImage.image;
		VK_CHECK_RESULT(vkCreateImageView(device, &colorImageView, nullptr, &storageImage.view));

		VkCommandBuffer cmdBuffer = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
		vks::tools::setImageLayout(cmdBuffer, storageImage.image,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_GENERAL,
			{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
		vulkanDevice->flushCommandBuffer(cmdBuffer, queue);
	}

	/*
		The bottom level acceleration structure contains the scene's geometry (vertices, triangles)
	*/
	//m_rtBuilder.buildBlas(allBlas, vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);
	void createBottomLevelAccelerationStructure(vk::AccelerationStructureCreateGeometryTypeInfoKHR asCreate)
	{
		VkAccelerationStructureCreateInfoKHR asCreateInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR};//VkAccelerationStructureInfoNV        accelerationStructureInfo{};//		accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		asCreateInfo.type             = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;//		accelerationStructureInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
		asCreateInfo.flags            = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		asCreateInfo.maxGeometryCount = 1;//		accelerationStructureInfo.geometryCount = 1;

    std::vector<VkAccelerationStructureCreateGeometryTypeInfoKHR> asCreateGeometryInfo;        // specifies the shape of geometries that will be
		asCreateGeometryInfo.push_back(asCreate);
	  //accelerationStructureInfo.instanceCount = 0;
	  asCreateInfo.pGeometryInfos = asCreateGeometryInfo.data();        //accelerationStructureInfo.pGeometries = geometries;

		//accelerationStructureCI.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
	  //accelerationStructureCI.info  = accelerationStructureInfo;

    //--------------------------------------------------------------------------------------------------
		// Create the acceleration structure
		//


		// 1. Create the acceleration structure
		VK_CHECK_RESULT(vkCreateAccelerationStructureKHR(device, &asCreateInfo, nullptr, &bottomLevelASKHR.accelerationStructure));        //		VK_CHECK_RESULT(vkCreateAccelerationStructureNV(device, &accelerationStructureCI, nullptr, &bottomLevelAS.accelerationStructure));


		// 2. Find memory requirements
		VkAccelerationStructureMemoryRequirementsInfoKHR memInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR};//VkAccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo{};//memoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
		memInfo.accelerationStructure = bottomLevelASKHR.accelerationStructure;
		memInfo.buildType             = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;//memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;
		memInfo.type                  = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_KHR;//memoryRequirementsInfo.accelerationStructure = bottomLevelAS.accelerationStructure;
		VkMemoryRequirements2 memReqs{VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};		//VkMemoryRequirements2 memoryRequirements2{};
		vkGetAccelerationStructureMemoryRequirementsKHR(device, &memInfo, &memReqs);//vkGetAccelerationStructureMemoryRequirementsNV(device, &memoryRequirementsInfo, &memoryRequirements2);

    // 3. Allocate memory
		VkMemoryAllocateInfo memAlloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};//VkMemoryAllocateInfo memoryAllocateInfo = vks::initializers::memoryAllocateInfo();
		memAlloc.allocationSize  = memReqs.memoryRequirements.size;//memoryAllocateInfo.allocationSize = memoryRequirements2.memoryRequirements.size;
		memAlloc.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);//memoryAllocateInfo.memoryTypeIndex = vulkanDevice->getMemoryType(memoryRequirements2.memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &bottomLevelASKHR.memory));

		VkBindAccelerationStructureMemoryInfoKHR bind{VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR};//		VkBindAccelerationStructureMemoryInfoNV accelerationStructureMemoryInfo{};		accelerationStructureMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
		bind.accelerationStructure = bottomLevelASKHR.accelerationStructure;                                                                       //accelerationStructureMemoryInfo.accelerationStructure = bottomLevelAS.accelerationStructure;


    bind.memory       = bottomLevelASKHR.memory;        //accelerationStructureMemoryInfo.memory = bottomLevelAS.memory;
    bind.memoryOffset          = 0;
	  VK_CHECK_RESULT(vkBindAccelerationStructureMemoryKHR(device, 1, &bind));        //VK_CHECK_RESULT(vkBindAccelerationStructureMemoryNV(device, 1, &accelerationStructureMemoryInfo));

		//TODO:
		//VK_CHECK_RESULT(vkGetAccelerationStructureHandleNV(device, bottomLevelAS.accelerationStructure, sizeof(uint64_t), &bottomLevelAS.handle));
	}

	/*
		The top level acceleration structure contains the scene's object instances
	*/
	void createTopLevelAccelerationStructure()
	{
		VkAccelerationStructureCreateGeometryTypeInfoKHR geometryCreate{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_GEOMETRY_TYPE_INFO_KHR};//VkAccelerationStructureInfoNV accelerationStructureInfo{};
		geometryCreate.geometryType      = VK_GEOMETRY_TYPE_INSTANCES_KHR;//		accelerationStructureInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		geometryCreate.maxPrimitiveCount = static_cast<uint32_t>(1);//accelerationStructureInfo.instanceCount = 1;
		geometryCreate.allowsTransforms  = (VK_TRUE);

		

		//accelerationStructureInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
		//accelerationStructureInfo.geometryCount = 0;

    VkAccelerationStructureCreateInfoKHR asCreateInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR};//VkAccelerationStructureCreateInfoNV accelerationStructureCI{};
		asCreateInfo.type             = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;//accelerationStructureCI.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
	  asCreateInfo.flags            = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		asCreateInfo.maxGeometryCount = 1;
		asCreateInfo.pGeometryInfos   = &geometryCreate;

		
		
		//accelerationStructureCI.info = accelerationStructureInfo;
		//VK_CHECK_RESULT(vkCreateAccelerationStructureNV(device, &accelerationStructureCI, nullptr, &topLevelAS.accelerationStructure));
		// 1. Create the acceleration structure
		VK_CHECK_RESULT(vkCreateAccelerationStructureKHR(device, &asCreateInfo, nullptr, &topLevelASKHR.accelerationStructure));

		
		
		
		
		VkAccelerationStructureMemoryRequirementsInfoKHR memInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR};//VkAccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo{};//memoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
    memInfo.accelerationStructure = topLevelASKHR.accelerationStructure;
    memInfo.buildType             = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;//memoryRequirementsInfo.accelerationStructure = topLevelAS.accelerationStructure;
    memInfo.type                  = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_KHR; //memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_NV;
    VkMemoryRequirements2 memReqs{VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};//VkMemoryRequirements2 memoryRequirements2{};
    vkGetAccelerationStructureMemoryRequirementsKHR(device, &memInfo, &memReqs);//vkGetAccelerationStructureMemoryRequirementsNV(device, &memoryRequirementsInfo, &memoryRequirements2);

	  VkMemoryAllocateFlagsInfo memFlagInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  	memFlagInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

		
		
		
		// 3. Allocate memory
		VkMemoryAllocateInfo memAlloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO}; //VkMemoryAllocateInfo memoryAllocateInfo = vks::initializers::memoryAllocateInfo();
		memAlloc.allocationSize  = memReqs.memoryRequirements.size; //memoryAllocateInfo.allocationSize = memoryRequirements2.memoryRequirements.size;
		memAlloc.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);        //		memoryAllocateInfo.memoryTypeIndex = vulkanDevice->getMemoryType(memoryRequirements2.memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &topLevelASKHR.memory));//resultAccel.allocation   = AllocateMemory(memAlloc);//VK_CHECK_RESULT(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &topLevelAS.memory));

		

		
		
		
		
		//VK_CHECK_RESULT(vkBindAccelerationStructureMemoryNV(device, 1, &accelerationStructureMemoryInfo));
		// 4. Bind memory with acceleration structure
		VkBindAccelerationStructureMemoryInfoKHR bind{VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_KHR};//VkBindAccelerationStructureMemoryInfoNV accelerationStructureMemoryInfo{};//accelerationStructureMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
		bind.accelerationStructure = topLevelASKHR.accelerationStructure;//accelerationStructureMemoryInfo.accelerationStructure = topLevelAS.accelerationStructure;
		bind.memory                = topLevelASKHR.memory;//accelerationStructureMemoryInfo.memory = topLevelAS.memory;
		bind.memoryOffset          = 0;
		VK_CHECK_RESULT(vkBindAccelerationStructureMemoryKHR(device, 1, &bind));        //VK_CHECK_RESULT(vkBindAccelerationStructureMemoryNV(device, 1, &accelerationStructureMemoryInfo));
		
		//TODO
		//VK_CHECK_RESULT(vkGetAccelerationStructureHandleNV(device, topLevelAS.accelerationStructure, sizeof(uint64_t), &topLevelAS.handle));
	}

	/*
		Create scene geometry and ray tracing acceleration structures
	*/
	void createScene()
	{
		// Setup vertices for a single triangle
		struct Vertex {
			float pos[3];
		};
		std::vector<Vertex> vertices = {
			{ {  1.0f,  1.0f, 0.0f } },
			{ { -1.0f,  1.0f, 0.0f } },
			{ {  0.0f, -1.0f, 0.0f } }
		};

		// Setup indices
		std::vector<uint32_t> indices = { 0, 1, 2 };
		indexCount = static_cast<uint32_t>(indices.size());

		// Create buffers
		// For the sake of simplicity we won't stage the vertex data to the gpu memory
		// Vertex buffer
		VK_CHECK_RESULT(vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&vertexBuffer,
			vertices.size() * sizeof(Vertex),
			vertices.data()));
		// Index buffer
		VK_CHECK_RESULT(vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&indexBuffer,
			indices.size() * sizeof(uint32_t),
			indices.data()));

		/*
			Create the bottom level acceleration structure containing the actual scene geometry
		*/
		//nvvk::RaytracingBuilderKHR::Blas                       HelloVulkan::objectToVkGeometryKHR(const ObjModel &model)
		vk::AccelerationStructureCreateGeometryTypeInfoKHR asCreate; //VkGeometryNV                                       geometry{};//geometry.sType = VK_STRUCTURE_TYPE_GEOMETRY_NV;
		asCreate.setGeometryType(vk::GeometryTypeKHR::eTriangles); //geometry.geometryType                       = VK_GEOMETRY_TYPE_TRIANGLES_NV;
		asCreate.setIndexType(vk::IndexType::eUint32);
		asCreate.setVertexFormat(vk::Format::eR32G32B32Sfloat);
		asCreate.setMaxPrimitiveCount(1);        // Nb triangles
		asCreate.setMaxVertexCount(vertices.size()); //		geometry.geometry.triangles.vertexCount = static_cast<uint32_t>(vertices.size());

		asCreate.setAllowsTransforms(VK_FALSE);        // No adding transformation matrices

		vk::DeviceAddress                                 vertexAddress = device.getBufferAddress({vertexBuffer.buffer});//geometry.geometry.triangles.vertexData = vertexBuffer.buffer;
		vk::DeviceAddress                                 indexAddress  = device.getBufferAddress({indexBuffer.buffer});//geometry.geometry.triangles.indexData = indexBuffer.buffer;
		vk::AccelerationStructureGeometryTrianglesDataKHR triangles;

		triangles.setVertexFormat(asCreate.vertexFormat); //geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_GEOMETRY_TRIANGLES_NV;
		triangles.setVertexData(vertexAddress);
		triangles.setVertexStride(sizeof(Vertex)); //geometry.geometry.triangles.vertexStride = sizeof(Vertex);
		triangles.setIndexType(asCreate.indexType);//geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
		triangles.setIndexData(indexAddress);
		triangles.setTransformData({}); //geometry.geometry.triangles.transformData = VK_NULL_HANDLE;

		
		//TODO
		//geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		
		
		
		
		
		

		vk::AccelerationStructureGeometryKHR asGeom;
		asGeom.setGeometryType(asCreate.geometryType);
		// Consider the geometry opaque for optimization
		asGeom.setFlags(vk::GeometryFlagBitsKHR::eOpaque);//geometry.flags = VK_GEOMETRY_OPAQUE_BIT_NV;
		//geometry.geometry.aabbs       = {};
		//geometry.geometry.aabbs.sType = {VK_STRUCTURE_TYPE_GEOMETRY_AABB_NV};
		asGeom.geometry.setTriangles(triangles);

		vk::AccelerationStructureBuildOffsetInfoKHR offset;
		offset.setFirstVertex(0);//geometry.geometry.triangles.vertexOffset = 0;
		offset.setPrimitiveCount(asCreate.maxPrimitiveCount);//geometry.geometry.triangles.indexCount = indexCount;
		offset.setPrimitiveOffset(0);//geometry.geometry.triangles.indexOffset = 0;
		offset.setTransformOffset(0);//geometry.geometry.triangles.transformOffset = 0;

		createBottomLevelAccelerationStructure(asCreate);

		/*
			Create the top-level acceleration structure that contains geometry instance information
		*/

		// Single instance with a 3x4 transform matrix for the ray traced triangle
		vks::Buffer instanceBuffer;

		glm::mat3x4 transform = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
		};

		GeometryInstance geometryInstance{};
		geometryInstance.transform = transform;
		geometryInstance.instanceId = 0;
		geometryInstance.mask = 0xff;
		geometryInstance.instanceOffset = 0;
		geometryInstance.flags                       = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		geometryInstance.accelerationStructureHandle = bottomLevelASKHR.handle;

		VK_CHECK_RESULT(vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&instanceBuffer,
			sizeof(GeometryInstance),
			&geometryInstance));

		createTopLevelAccelerationStructure();

		/*
			Build the acceleration structure
		*/

		// Acceleration structure build requires some scratch space to store temporary information
		VkAccelerationStructureMemoryRequirementsInfoKHR memoryRequirementsInfo{
		    VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR};//VkAccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo{};//memoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
		memoryRequirementsInfo.type                  = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_KHR;//memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_NV;
		memoryRequirementsInfo.accelerationStructure = bottomLevelASKHR.accelerationStructure;
		memoryRequirementsInfo.buildType             = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;

		
		

		VkMemoryRequirements2 memReqBottomLevelAS;
		memoryRequirementsInfo.accelerationStructure = bottomLevelASKHR.accelerationStructure;
		vkGetAccelerationStructureMemoryRequirementsKHR(device, &memoryRequirementsInfo, &memReqBottomLevelAS);

		VkMemoryRequirements2 memReqTopLevelAS;
		memoryRequirementsInfo.accelerationStructure = topLevelASKHR.accelerationStructure;
		vkGetAccelerationStructureMemoryRequirementsKHR(device, &memoryRequirementsInfo, &memReqTopLevelAS);

		const VkDeviceSize scratchBufferSize = std::max(memReqBottomLevelAS.memoryRequirements.size, memReqTopLevelAS.memoryRequirements.size);

		vks::Buffer scratchBuffer;
		VK_CHECK_RESULT(vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_RAY_TRACING_BIT_NV,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&scratchBuffer,
			scratchBufferSize));
		VkBufferDeviceAddressInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
		bufferInfo.buffer = scratchBuffer.buffer;
		VkDeviceAddress scratchAddress = vkGetBufferDeviceAddress(device, &bufferInfo);

		VkCommandBuffer cmdBuffer = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		/*
			Build bottom level acceleration structure
		*/
		std::vector<VkAccelerationStructureGeometryKHR> geom;
		geom.push_back(asGeom);
		const VkAccelerationStructureGeometryKHR *  pGeometry = geom.data(); //blas.asGeometry.data();
		VkAccelerationStructureBuildGeometryInfoKHR bottomASInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR};//VkAccelerationStructureInfoNV buildInfo{};//buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV;
		bottomASInfo.type                      = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;//buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_NV;
		bottomASInfo.flags                     = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		bottomASInfo.update                    = VK_FALSE;
		bottomASInfo.srcAccelerationStructure  = VK_NULL_HANDLE;
		bottomASInfo.dstAccelerationStructure  = bottomLevelASKHR.accelerationStructure;
		bottomASInfo.geometryArrayOfPointers   = VK_FALSE;
		bottomASInfo.geometryCount             = (uint32_t) 1;//buildInfo.geometryCount = 1;
		bottomASInfo.ppGeometries              = &pGeometry; //buildInfo.pGeometries = &geometry;
		bottomASInfo.scratchData.deviceAddress = scratchAddress;

		// Pointers of offset
		std::vector<VkAccelerationStructureBuildOffsetInfoKHR> buildOffsetInfo;
		buildOffsetInfo.push_back(offset);
		std::vector<const VkAccelerationStructureBuildOffsetInfoKHR *> pBuildOffset(1);
		for (size_t i = 0; i < pBuildOffset.size(); i++)
			pBuildOffset[i] = &buildOffsetInfo[i];

		// Building the AS
		//vkCmdBuildAccelerationStructureNV(
		//    cmdBuffer,
		//    &buildInfo,
		//    VK_NULL_HANDLE,
		//    0,
		//    VK_FALSE,
		//    bottomLevelAS.accelerationStructure,
		//    VK_NULL_HANDLE,
		//    scratchBuffer.buffer,
		//    0);
		vkCmdBuildAccelerationStructureKHR(cmdBuffer, 1, &bottomASInfo, pBuildOffset.data());

		// Since the scratch buffer is reused across builds, we need a barrier to ensure one build
		// is finished before starting the next one
		//VkMemoryBarrier memoryBarrier = vks::initializers::memoryBarrier();
		//memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;
		//memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;
		//vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);
		VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
		barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
		barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
		                     VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &barrier, 0, nullptr, 0, nullptr);//vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV, 0, 1, &memoryBarrier, 0, 0, 0, 0);



		/*
			Build top-level acceleration structure
		*/
		//buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_NV;
		//buildInfo.pGeometries = 0;
		//buildInfo.geometryCount = 0;
		//buildInfo.instanceCount = 1;

		//vkCmdBuildAccelerationStructureNV(
		//	cmdBuffer,
		//	&buildInfo,
		//	instanceBuffer.buffer,
		//	0,
		//	VK_FALSE,
		//	topLevelAS.accelerationStructure,
		//	VK_NULL_HANDLE,
		//	scratchBuffer.buffer,
		//	0);

		

		vulkanDevice->flushCommandBuffer(cmdBuffer, queue);

		scratchBuffer.destroy();
		instanceBuffer.destroy();
	}

	VkDeviceSize copyShaderIdentifier(uint8_t* data, const uint8_t* shaderHandleStorage, uint32_t groupIndex) {
		const uint32_t shaderGroupHandleSize = rayTracingProperties.shaderGroupHandleSize;
		memcpy(data, shaderHandleStorage + groupIndex * shaderGroupHandleSize, shaderGroupHandleSize);
		return shaderGroupHandleSize;
	}

	/*
		Create the Shader Binding Table that binds the programs and top-level acceleration structure
	*/
	void createShaderBindingTable() {
		// Create buffer for the shader binding table
		const uint32_t sbtSize = rayTracingProperties.shaderGroupHandleSize * NUM_SHADER_GROUPS;
		VK_CHECK_RESULT(vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_RAY_TRACING_BIT_KHR,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			&shaderBindingTable,
			sbtSize));
		shaderBindingTable.map();

		auto shaderHandleStorage = new uint8_t[sbtSize];
		// Get shader identifiers
		VK_CHECK_RESULT(vkGetRayTracingShaderGroupHandlesKHR(device, pipeline, 0, NUM_SHADER_GROUPS, sbtSize, shaderHandleStorage));
		auto* data = static_cast<uint8_t*>(shaderBindingTable.mapped);
		// Copy the shader identifiers to the shader binding table
		data += copyShaderIdentifier(data, shaderHandleStorage, INDEX_RAYGEN);
		data += copyShaderIdentifier(data, shaderHandleStorage, INDEX_MISS);
		data += copyShaderIdentifier(data, shaderHandleStorage, INDEX_CLOSEST_HIT);
		shaderBindingTable.unmap();
	}

	/*
		Create the descriptor sets used for the ray tracing dispatch
	*/
	void createDescriptorSets()
	{
		std::vector<VkDescriptorPoolSize> poolSizes = {
			{ VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }
		};
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = vks::initializers::descriptorPoolCreateInfo(poolSizes, 1);
		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = vks::initializers::descriptorSetAllocateInfo(descriptorPool, &descriptorSetLayout, 1);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet));

		VkWriteDescriptorSetAccelerationStructureKHR descriptorAccelerationStructureInfo{};
		descriptorAccelerationStructureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
		descriptorAccelerationStructureInfo.accelerationStructureCount = 1;
		descriptorAccelerationStructureInfo.pAccelerationStructures    = &topLevelASKHR.accelerationStructure;

		VkWriteDescriptorSet accelerationStructureWrite{};
		accelerationStructureWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		// The specialized acceleration structure descriptor has to be chained
		accelerationStructureWrite.pNext = &descriptorAccelerationStructureInfo;
		accelerationStructureWrite.dstSet = descriptorSet;
		accelerationStructureWrite.dstBinding = 0;
		accelerationStructureWrite.descriptorCount = 1;
		accelerationStructureWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

		VkDescriptorImageInfo storageImageDescriptor{};
		storageImageDescriptor.imageView = storageImage.view;
		storageImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		VkWriteDescriptorSet resultImageWrite = vks::initializers::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, &storageImageDescriptor);
		VkWriteDescriptorSet uniformBufferWrite = vks::initializers::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, &ubo.descriptor);

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			accelerationStructureWrite,
			resultImageWrite,
			uniformBufferWrite
		};
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, VK_NULL_HANDLE);
	}

	/*
		Create our ray tracing pipeline
	*/
	void createRayTracingPipeline()
	{
		VkDescriptorSetLayoutBinding accelerationStructureLayoutBinding{};
		accelerationStructureLayoutBinding.binding = 0;
		accelerationStructureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		accelerationStructureLayoutBinding.descriptorCount = 1;
		accelerationStructureLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

		VkDescriptorSetLayoutBinding resultImageLayoutBinding{};
		resultImageLayoutBinding.binding = 1;
		resultImageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		resultImageLayoutBinding.descriptorCount = 1;
		resultImageLayoutBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

		VkDescriptorSetLayoutBinding uniformBufferBinding{};
		uniformBufferBinding.binding = 2;
		uniformBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBufferBinding.descriptorCount = 1;
		uniformBufferBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

		std::vector<VkDescriptorSetLayoutBinding> bindings({
			accelerationStructureLayoutBinding,
			resultImageLayoutBinding,
			uniformBufferBinding
			});

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout));

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

		const uint32_t shaderIndexRaygen = 0;
		const uint32_t shaderIndexMiss = 1;
		const uint32_t shaderIndexClosestHit = 2;

		std::array<VkPipelineShaderStageCreateInfo, 3> shaderStages;
		shaderStages[shaderIndexRaygen] = loadShader(getShadersPath() + "nv_ray_tracing_basic/raygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR);
		shaderStages[shaderIndexMiss] = loadShader(getShadersPath() + "nv_ray_tracing_basic/miss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR);
		shaderStages[shaderIndexClosestHit] = loadShader(getShadersPath() + "nv_ray_tracing_basic/closesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);

		/*
			Setup ray tracing shader groups
		*/
		
		for (auto& group : m_shaderGroups) {
			// Init all groups with some default values
			group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
			group.generalShader = VK_SHADER_UNUSED_KHR;
			group.closestHitShader   = VK_SHADER_UNUSED_KHR;
			group.anyHitShader       = VK_SHADER_UNUSED_KHR;
			group.intersectionShader = VK_SHADER_UNUSED_KHR;
		}

		// Links shaders and types to ray tracing shader groups
		m_shaderGroups[INDEX_RAYGEN].type          = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		m_shaderGroups[INDEX_RAYGEN].generalShader = shaderIndexRaygen;
		m_shaderGroups[INDEX_MISS].type            = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		m_shaderGroups[INDEX_MISS].generalShader   = shaderIndexMiss;
		m_shaderGroups[INDEX_CLOSEST_HIT].type     = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		m_shaderGroups[INDEX_CLOSEST_HIT].generalShader = VK_SHADER_UNUSED_KHR;
		m_shaderGroups[INDEX_CLOSEST_HIT].closestHitShader = shaderIndexClosestHit;

		VkRayTracingPipelineCreateInfoKHR rayPipelineInfo{};
		rayPipelineInfo.sType             = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
		rayPipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		rayPipelineInfo.pStages = shaderStages.data();
		rayPipelineInfo.groupCount        = static_cast<uint32_t>(m_shaderGroups.size());
		rayPipelineInfo.pGroups           = m_shaderGroups.data();
		rayPipelineInfo.maxRecursionDepth = 1;
		rayPipelineInfo.layout = pipelineLayout;
		VK_CHECK_RESULT(vkCreateRayTracingPipelinesKHR(device, VK_NULL_HANDLE, 1, &rayPipelineInfo, nullptr, &pipeline));
	}

	/*
		Create the uniform buffer used to pass matrices to the ray tracing ray generation shader
	*/
	void createUniformBuffer()
	{
		VK_CHECK_RESULT(vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&ubo,
			sizeof(uniformData),
			&uniformData));
		VK_CHECK_RESULT(ubo.map());

		updateUniformBuffers();
	}

	/*
		Command buffer generation
	*/
	void buildCommandBuffers()
	{
		VkCommandBufferBeginInfo cmdBufInfo = vks::initializers::commandBufferBeginInfo();

		VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
		{
			VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo));

			/*
				Dispatch the ray tracing commands
			*/
			vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipeline);
			vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipelineLayout, 0, 1, &descriptorSet, 0, 0);

			// Calculate shader binding offsets, which is pretty straight forward in our example
			VkDeviceSize bindingOffsetRayGenShader = rayTracingProperties.shaderGroupHandleSize * INDEX_RAYGEN;
			VkDeviceSize bindingOffsetMissShader   = rayTracingProperties.shaderGroupHandleSize * INDEX_MISS;
			VkDeviceSize bindingOffsetHitShader    = rayTracingProperties.shaderGroupHandleSize * INDEX_CLOSEST_HIT;
			VkDeviceSize bindingStride             = rayTracingProperties.shaderGroupHandleSize;

			vk::DeviceSize progSize =
			    rayTracingProperties.shaderGroupBaseAlignment;        // Size of a program identifier
			vk::DeviceSize rayGenOffset   = 0u * progSize;            // Start at the beginning of m_sbtBuffer
			vk::DeviceSize missOffset     = 1u * progSize;            // Jump over raygen
			vk::DeviceSize hitGroupOffset = 2u * progSize;            // Jump over the previous shaders

			vk::DeviceSize sbtSize = progSize * (vk::DeviceSize) m_shaderGroups.size();

			VkStridedBufferRegionKHR raygenShaderBindingTable, missShaderBindingTable, hitShaderBindingTable, callableShaderBindingTable;
			raygenShaderBindingTable.buffer = shaderBindingTable.buffer;
			raygenShaderBindingTable.offset = rayGenOffset;
			raygenShaderBindingTable.size   = sbtSize;
			raygenShaderBindingTable.stride = progSize;

			missShaderBindingTable.buffer   = shaderBindingTable.buffer;
			missShaderBindingTable.offset   = missOffset;
			missShaderBindingTable.size     = sbtSize;
			missShaderBindingTable.stride   = progSize;

  		hitShaderBindingTable.buffer  = shaderBindingTable.buffer;
			hitShaderBindingTable.offset  = hitGroupOffset;
			hitShaderBindingTable.size    = sbtSize;
			hitShaderBindingTable.stride  = progSize;

			vkCmdTraceRaysKHR(drawCmdBuffers[i],
			                  &raygenShaderBindingTable,
			                  &missShaderBindingTable,
			                  &hitShaderBindingTable,
			                  &callableShaderBindingTable,
			                  width, height, 1);

			//vkCmdTraceRaysKHR(drawCmdBuffers[i],
			//	shaderBindingTable.buffer, bindingOffsetRayGenShader,
			//	shaderBindingTable.buffer, bindingOffsetMissShader, bindingStride,
			//	shaderBindingTable.buffer, bindingOffsetHitShader, bindingStride,
			//	VK_NULL_HANDLE, 0, 0,
			//	width, height, 1);

			/*
				Copy raytracing output to swap chain image
			*/

			// Prepare current swapchain image as transfer destination
			vks::tools::setImageLayout(
				drawCmdBuffers[i],
				swapChain.images[i],
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				subresourceRange);

			// Prepare ray tracing output image as transfer source
			vks::tools::setImageLayout(
				drawCmdBuffers[i],
				storageImage.image,
				VK_IMAGE_LAYOUT_GENERAL,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				subresourceRange);

			VkImageCopy copyRegion{};
			copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
			copyRegion.srcOffset = { 0, 0, 0 };
			copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
			copyRegion.dstOffset = { 0, 0, 0 };
			copyRegion.extent = { width, height, 1 };
			vkCmdCopyImage(drawCmdBuffers[i], storageImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapChain.images[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

			// Transition swap chain image back for presentation
			vks::tools::setImageLayout(
				drawCmdBuffers[i],
				swapChain.images[i],
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				subresourceRange);

			// Transition ray tracing output image back to general layout
			vks::tools::setImageLayout(
				drawCmdBuffers[i],
				storageImage.image,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_IMAGE_LAYOUT_GENERAL,
				subresourceRange);

			//@todo: Default render pass setup willl overwrite contents
			//vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			//drawUI(drawCmdBuffers[i]);
			//vkCmdEndRenderPass(drawCmdBuffers[i]);

			VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
		}
	}

	void updateUniformBuffers()
	{
		uniformData.projInverse = glm::inverse(camera.matrices.perspective);
		uniformData.viewInverse = glm::inverse(camera.matrices.view);
		memcpy(ubo.mapped, &uniformData, sizeof(uniformData));
	}

	void prepare()
	{
		VulkanExampleBase::prepare();

		// Query the ray tracing properties of the current implementation, we will need them later on
		rayTracingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_KHR;
		VkPhysicalDeviceProperties2 deviceProps2{};
		deviceProps2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		deviceProps2.pNext = &rayTracingProperties;
		vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProps2);

		// Get VK_NV_ray_tracing related function pointers
		//vkCreateAccelerationStructureNV = reinterpret_cast<PFN_vkCreateAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureNV"));
		//vkDestroyAccelerationStructureNV = reinterpret_cast<PFN_vkDestroyAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureNV"));
		//vkBindAccelerationStructureMemoryNV = reinterpret_cast<PFN_vkBindAccelerationStructureMemoryNV>(vkGetDeviceProcAddr(device, "vkBindAccelerationStructureMemoryNV"));
		//vkGetAccelerationStructureHandleNV = reinterpret_cast<PFN_vkGetAccelerationStructureHandleNV>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureHandleNV"));
		//vkGetAccelerationStructureMemoryRequirementsNV = reinterpret_cast<PFN_vkGetAccelerationStructureMemoryRequirementsNV>(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureMemoryRequirementsNV"));
		//vkCmdBuildAccelerationStructureNV = reinterpret_cast<PFN_vkCmdBuildAccelerationStructureNV>(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructureNV"));
		//vkCreateRayTracingPipelinesNV = reinterpret_cast<PFN_vkCreateRayTracingPipelinesNV>(vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesNV"));
		//vkGetRayTracingShaderGroupHandlesNV = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesNV>(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesNV"));
		//vkCmdTraceRaysNV = reinterpret_cast<PFN_vkCmdTraceRaysNV>(vkGetDeviceProcAddr(device, "vkCmdTraceRaysNV"));


		pfn_vkCreateAccelerationStructureKHR         = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
		pfn_vkDestroyAccelerationStructureKHR                 = (PFN_vkDestroyAccelerationStructureKHR) vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR");
		pfn_vkGetAccelerationStructureMemoryRequirementsKHR   = (PFN_vkGetAccelerationStructureMemoryRequirementsKHR) vkGetDeviceProcAddr(device, "vkGetAccelerationStructureMemoryRequirementsKHR");
		pfn_vkBindAccelerationStructureMemoryKHR              = (PFN_vkBindAccelerationStructureMemoryKHR) vkGetDeviceProcAddr(device, "vkBindAccelerationStructureMemoryKHR");
		pfn_vkCmdBuildAccelerationStructureKHR                = (PFN_vkCmdBuildAccelerationStructureKHR) vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructureKHR");
		pfn_vkCmdBuildAccelerationStructureIndirectKHR        = (PFN_vkCmdBuildAccelerationStructureIndirectKHR) vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructureIndirectKHR");
		pfn_vkBuildAccelerationStructureKHR                   = (PFN_vkBuildAccelerationStructureKHR) vkGetDeviceProcAddr(device, "vkBuildAccelerationStructureKHR");
		pfn_vkCopyAccelerationStructureKHR                    = (PFN_vkCopyAccelerationStructureKHR) vkGetDeviceProcAddr(device, "vkCopyAccelerationStructureKHR");
		pfn_vkCopyAccelerationStructureToMemoryKHR            = (PFN_vkCopyAccelerationStructureToMemoryKHR) vkGetDeviceProcAddr(device, "vkCopyAccelerationStructureToMemoryKHR");
		pfn_vkCopyMemoryToAccelerationStructureKHR            = (PFN_vkCopyMemoryToAccelerationStructureKHR) vkGetDeviceProcAddr(device, "vkCopyMemoryToAccelerationStructureKHR");
		pfn_vkWriteAccelerationStructuresPropertiesKHR        = (PFN_vkWriteAccelerationStructuresPropertiesKHR) vkGetDeviceProcAddr(device, "vkWriteAccelerationStructuresPropertiesKHR");
		pfn_vkCmdCopyAccelerationStructureKHR                 = (PFN_vkCmdCopyAccelerationStructureKHR) vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureKHR");
		pfn_vkCmdCopyAccelerationStructureToMemoryKHR         = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR) vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureToMemoryKHR");
		pfn_vkCmdCopyMemoryToAccelerationStructureKHR         = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR) vkGetDeviceProcAddr(device, "vkCmdCopyMemoryToAccelerationStructureKHR");
		pfn_vkCmdTraceRaysKHR                                 = (PFN_vkCmdTraceRaysKHR) vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR");
		pfn_vkCreateRayTracingPipelinesKHR                    = (PFN_vkCreateRayTracingPipelinesKHR) vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR");
		pfn_vkGetRayTracingShaderGroupHandlesKHR              = (PFN_vkGetRayTracingShaderGroupHandlesKHR) vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR");
		pfn_vkGetAccelerationStructureDeviceAddressKHR        = (PFN_vkGetAccelerationStructureDeviceAddressKHR) vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR");
		pfn_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR) vkGetDeviceProcAddr(device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
		pfn_vkCmdWriteAccelerationStructuresPropertiesKHR     = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR) vkGetDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesKHR");
		pfn_vkCmdTraceRaysIndirectKHR                         = (PFN_vkCmdTraceRaysIndirectKHR) vkGetDeviceProcAddr(device, "vkCmdTraceRaysIndirectKHR");


		createScene();
		createStorageImage();
		createUniformBuffer();
		createRayTracingPipeline();
		createShaderBindingTable();
		createDescriptorSets();
		buildCommandBuffers();
		prepared = true;
	}

	void draw()
	{
		VulkanExampleBase::prepareFrame();
		submitInfo.commandBufferCount = 1;
		VkCommandBuffer tmp           = drawCmdBuffers[currentBuffer];
		submitInfo.pCommandBuffers    = &tmp;
		VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
		VulkanExampleBase::submitFrame();
	}

	virtual void render()
	{
		if (!prepared)
			return;
		draw();
		if (camera.updated)
			updateUniformBuffers();
	}
};

VULKAN_EXAMPLE_MAIN()
