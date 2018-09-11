/**
 * Renderer.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Buffer.h"
#include "Device.h"
#include "GraphicsManager.h"
#include "Image.h"
#include "Renderer.h"
#include "Queue.h"

#include <iostream>
#include <vulkan/vulkan.h>

Result<void> Renderer::createDescriptorLayouts() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo =
                this->getDescriptorSetLayoutCreateInfo(&bindings);

        if (vkCreateDescriptorSetLayout(device,
                                        &descriptorSetLayoutCreateInfo,
                                        nullptr,
                                        &this->descriptorLayout) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateDescriptorSetLayout);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createPipelineLayouts() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = this->getPipelineLayoutCreateInfo();

        if (vkCreatePipelineLayout(device,
                                   &pipelineLayoutCreateInfo,
                                   nullptr,
                                   &this->pipelineLayout) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreatePipelineLayout);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createPipelines() {
    return Result<void>::createError(Error::None);
}

std::vector<VkDescriptorSetLayoutBinding> Renderer::getDescriptorSetLayoutBindings() const noexcept {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings (2);

    // Configure Transform Bindings
    descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].pImmutableSamplers = nullptr;

    // Configure Texture Bindings
    descriptorSetLayoutBindings[1].binding = 1;
    descriptorSetLayoutBindings[1].descriptorCount = 1;
    descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptorSetLayoutBindings[1].pImmutableSamplers = nullptr;

    return descriptorSetLayoutBindings;
}

VkDescriptorSetLayoutCreateInfo Renderer::getDescriptorSetLayoutCreateInfo(
        std::vector<VkDescriptorSetLayoutBinding> *bindings) const noexcept {
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    *bindings = this->getDescriptorSetLayoutBindings();

    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.flags = 0;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(bindings->size());
    descriptorSetLayoutCreateInfo.pBindings = bindings->data();

    return descriptorSetLayoutCreateInfo;
}

VkPipelineLayoutCreateInfo Renderer::getPipelineLayoutCreateInfo() const noexcept {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};

    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &this->descriptorLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    return pipelineLayoutCreateInfo;
}

Result<VkDevice> Renderer::getGraphicsDevice() const noexcept {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    Result<std::weak_ptr<const Device>> result = graphicsManager.getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<std::weak_ptr<const Device>>(result);

        if (std::shared_ptr<const Device> dev = device.lock())
            return dev->getVulkanDevice();
        else
            return Result<VkDevice>::createError(Error::GraphicsManagerNotStartedUp);
    }

    return Result<VkDevice>::createError(result.getError());
}

Result<void> Renderer::loadQueues() {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    Result<std::weak_ptr<const Device>> result = graphicsManager.getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<std::weak_ptr<const Device>>(result);

        if (std::shared_ptr<const Device> dev = device.lock()) {
            this->deviceQueues = dev->getDeviceQueues();
            this->transferQueue = this->deviceQueues[this->deviceQueues.size() - 1];

            return Result<void>::createError(Error::None);
        }
    }

    return Result<void>::createError(result.getError());
}

Renderer::Renderer() {
    this->descriptorLayout = VK_NULL_HANDLE;
    this->pipelineLayout = VK_NULL_HANDLE;
    this->pipeline = VK_NULL_HANDLE;
    this->deviceQueues = std::vector<std::shared_ptr<Queue>>();
    this->transferQueue = nullptr;
}

Renderer::~Renderer() {
    if (!this->deviceQueues.empty()) {
        std::cout << "WARNING: Renderer deleted without being shutdown..." << std::endl;
        this->shutdown();
    }
}

Result<void> Renderer::executeTransferBuffer(VkCommandBuffer cmdBuffer) const noexcept {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        Result<VkCommandPool> rslt = this->transferQueue->getVulkanPool();

        if (!rslt.hasError()) {
            auto cmdPool = static_cast<VkCommandPool>(rslt);
            Result<VkQueue> res = this->transferQueue->getVulkanQueue();

            if (!res.hasError()) {
                auto queue = static_cast<VkQueue>(res);
                vkEndCommandBuffer(cmdBuffer);

                VkSubmitInfo submitInfo = {};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.pNext = nullptr;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &cmdBuffer;
                submitInfo.signalSemaphoreCount = 0;
                submitInfo.pSignalSemaphores = nullptr;
                submitInfo.waitSemaphoreCount = 0;
                submitInfo.pWaitSemaphores = nullptr;
                submitInfo.pWaitDstStageMask = nullptr;

                VkResult submitResult = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
                VkResult waitResult = vkQueueWaitIdle(queue);

                if (submitResult == VK_SUCCESS && waitResult == VK_SUCCESS) {
                    vkFreeCommandBuffers(device, cmdPool, 1, &cmdBuffer);
                    return Result<void>::createError(Error::None);
                }
                else {
                    return Result<void>::createError(Error::FailedToSubmitQueue);
                }
            }
            else {
                return Result<void>::createError(res.getError());
            }
        }
        else {
            return Result<void>::createError(rslt.getError());
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::flush() const noexcept {
    for (auto &queue : this->deviceQueues) {
        Result<void> result = queue->submit();

        if (result.hasError()) {
            return Result<void>::createError(result.getError());
        }
    }
    
    // Temporarily Wait For Device
    Result<VkDevice> result = this->getGraphicsDevice();
    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        
        VkResult rslt = vkDeviceWaitIdle(device);
        if (rslt != VK_SUCCESS) {
            return Result<void>::createError(Error::FailedToFlushRenderer);
        }
    }
    
    return Result<void>::createError(Error::None);
}

Result<VkCommandBuffer> Renderer::requestTransferBuffer() const noexcept {
    Result<VkCommandPool> result = this->transferQueue->getVulkanPool();
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    if (!result.hasError()) {
        auto commandPool = static_cast<VkCommandPool>(result);
        Result<VkDevice> rslt = this->getGraphicsDevice();

        if (!rslt.hasError()) {
            auto device = static_cast<VkDevice>(rslt);
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};

            commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.pNext = nullptr;
            commandBufferAllocateInfo.commandBufferCount = 1;
            commandBufferAllocateInfo.commandPool = commandPool;
            commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

            VkResult res = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
            if (res == VK_SUCCESS) {
                VkCommandBufferBeginInfo commandBufferBeginInfo = {};

                commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                commandBufferAllocateInfo.pNext = nullptr;
                commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                VkResult rst = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
                if (rst == VK_SUCCESS) {
                    return Result<VkCommandBuffer>(commandBuffer);
                }
                else {
                    return Result<VkCommandBuffer>::createError(Error::FailedToAllocateCommandBuffer);
                }
            }
            else {
                return Result<VkCommandBuffer>::createError(Error::FailedToAllocateCommandBuffer);
            }
        }
    }

    return Result<VkCommandBuffer>::createError(result.getError());
}

Result<void> Renderer::startup() {
    Result<void> loadResult = this->loadQueues();
    if (loadResult.hasError()) {
        return Result<void>::createError(loadResult.getError());
    }

    Result<void> descriptorLayoutResult = this->createDescriptorLayouts();
    if (descriptorLayoutResult.hasError()) {
        return Result<void>::createError(descriptorLayoutResult.getError());
    }

    Result<void> pipelineLayoutResult = this->createPipelineLayouts();
    if (pipelineLayoutResult.hasError()) {
        return Result<void>::createError(pipelineLayoutResult.getError());
    }

    Result<void> pipelineResult = this->createPipelines();
    if (pipelineResult.hasError()) {
        return Result<void>::createError(pipelineResult.getError());
    }

    return Result<void>::createError(Error::None);
}

void Renderer::shutdown() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);

        if (this->pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(device, this->pipeline, nullptr);
            this->pipeline = VK_NULL_HANDLE;
        }

        if (this->pipelineLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
            this->pipelineLayout = VK_NULL_HANDLE;
        }

        if (this->descriptorLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(device, this->descriptorLayout, nullptr);
            this->descriptorLayout = VK_NULL_HANDLE;
        }
    }

    this->deviceQueues.clear();
    this->transferQueue = nullptr;
}
