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
#include "Material.h"
#include "Renderer.h"
#include "SpriteComponent.h"
#include "Queue.h"
#include "Texture.h"

#include <iostream>
#include <vulkan/vulkan.h>

Result<void> Renderer::allocateDescriptorSets() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = this->getDescriptorSetAllocateInfo();

        this->descriptorSets.resize(1);
        if (vkAllocateDescriptorSets(device,
                &descriptorSetAllocateInfo,
                this->descriptorSets.data()) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToAllocateDescriptorSets);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createDescriptorLayouts() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        std::vector<VkDescriptorSetLayoutBinding> bindings = this->getDescriptorSetLayoutBindings();
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

Result<void> Renderer::createDescriptorPool() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        std::vector<VkDescriptorPoolSize> poolSize = this->getDescriptorPoolSize();
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
                this->getDescriptorPoolCreateInfo(&poolSize);

        if (vkCreateDescriptorPool(device,
                                   &descriptorPoolCreateInfo,
                                   nullptr,
                                   &this->descriptorPool) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateDescriptorPool);
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

VkDescriptorSetAllocateInfo Renderer::getDescriptorSetAllocateInfo() const noexcept {
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};

    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = this->descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &this->descriptorLayout;

    return descriptorSetAllocateInfo;
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

VkDescriptorPoolCreateInfo Renderer::getDescriptorPoolCreateInfo(
        std::vector<VkDescriptorPoolSize> *poolSize) const noexcept {
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};

    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.maxSets = 2;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(poolSize->size());
    descriptorPoolCreateInfo.pPoolSizes = poolSize->data();

    return descriptorPoolCreateInfo;
}

std::vector<VkDescriptorPoolSize> Renderer::getDescriptorPoolSize() const noexcept {
    std::vector<VkDescriptorPoolSize> descriptorPoolSize (2);

    // Configure Transform Size
    descriptorPoolSize[0].descriptorCount = 1;
    descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    // Configure Texture Size
    descriptorPoolSize[1].descriptorCount = 1;
    descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    return descriptorPoolSize;
}

VkDescriptorSetLayoutCreateInfo Renderer::getDescriptorSetLayoutCreateInfo(
        std::vector<VkDescriptorSetLayoutBinding> *bindings) const noexcept {
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};

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
    if (!this->deviceQueues.empty() || this->transferQueue != nullptr || this->pipeline != VK_NULL_HANDLE ||
        this->pipelineLayout != VK_NULL_HANDLE || this->descriptorLayout != VK_NULL_HANDLE) {
        std::cout << "WARNING: Renderer deleted without being shutdown..." << std::endl;
        this->shutdown();
    }
}

Result<void> Renderer::begin() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        this->device = static_cast<VkDevice>(result);

        for (auto &queue : this->deviceQueues) {
            if (queue != this->transferQueue) {
                queue->bindPipeline(this->pipeline);
            }
        }

        if (this->transformBuffer == VK_NULL_HANDLE) {
            Result<std::shared_ptr<Buffer>> bufferResult = Buffer::createBuffer(sizeof(Transform),
                                                                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            if (bufferResult.hasError()) {
                return Result<void>::createError(bufferResult.getError());
            }
            this->transformBuffer = static_cast<std::shared_ptr<Buffer>>(bufferResult);
        }

        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(result.getError());
}

void Renderer::draw(std::shared_ptr<SpriteComponent> spriteComponent) {
    Transform matrixTransform;
    VkDescriptorBufferInfo descriptorBufferInfo = {};
    VkDescriptorImageInfo descriptorImageInfo = {};
    VkWriteDescriptorSet writeDescriptorSet[2] = {};

    // Configure Transform
    matrixTransform.model = spriteComponent->getModelTransform();
    matrixTransform.view = glm::lookAtLH(glm::vec3(0.0f, 0.0f, -2.0f),
                                         glm::vec3(0.0f, 0.0f, 0.0f),
                                         glm::vec3(0.0f, 1.0f, 0.0f));
    matrixTransform.proj = glm::orthoLH(-4.0f, 4.0f, -4.0f, 4.0f, 0.0f, 1.0f);

    // Fill Transform Buffer
    this->transformBuffer->fillBuffer(0, sizeof(matrixTransform), &matrixTransform);

    // Configure Buffer Info
    descriptorBufferInfo.buffer = static_cast<VkBuffer>(this->transformBuffer->getVulkanBuffer());
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = VK_WHOLE_SIZE;

    // Configure Image Info
    descriptorImageInfo.sampler = VK_NULL_HANDLE;
    descriptorImageInfo.imageView = spriteComponent->getTexture()->getImageView();
    descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Configure Transform Matrix
    writeDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet[0].pNext = nullptr;
    writeDescriptorSet[0].dstSet = this->descriptorSets[0];
    writeDescriptorSet[0].dstBinding = 0;
    writeDescriptorSet[0].dstArrayElement = 0;
    writeDescriptorSet[0].descriptorCount = 1;
    writeDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet[0].pImageInfo = nullptr;
    writeDescriptorSet[0].pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSet[0].pTexelBufferView = nullptr;

    // Configure Texture and Sampler
    writeDescriptorSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet[1].pNext = nullptr;
    writeDescriptorSet[1].dstSet = this->descriptorSets[0];
    writeDescriptorSet[1].dstBinding = 1;
    writeDescriptorSet[1].dstArrayElement = 0;
    writeDescriptorSet[1].descriptorCount = 1;
    writeDescriptorSet[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet[1].pImageInfo = &descriptorImageInfo;
    writeDescriptorSet[1].pBufferInfo = nullptr;
    writeDescriptorSet[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(this->device,
                           1,
                           writeDescriptorSet,
                           0,
                           nullptr);
}

Result<void> Renderer::end() {
    return Result<void>::createError(Error::None);
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

    Result<void> descriptorPoolResult = this->createDescriptorPool();
    if (descriptorPoolResult.hasError()) {
        return Result<void>::createError(descriptorPoolResult.getError());
    }

    Result<void> descriptorSetResult = this->allocateDescriptorSets();
    if (descriptorSetResult.hasError()) {
        return Result<void>::createError(descriptorSetResult.getError());
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

        if (!this->descriptorSets.empty()) {
            this->descriptorSets.clear();
        }

        if (this->descriptorPool != VK_NULL_HANDLE) {
            vkResetDescriptorPool(device, this->descriptorPool, 0);
            vkDestroyDescriptorPool(device, this->descriptorPool, nullptr);
            this->descriptorPool = VK_NULL_HANDLE;
        }

        if (this->descriptorLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(device, this->descriptorLayout, nullptr);
            this->descriptorLayout = VK_NULL_HANDLE;
        }
    }

    this->transformBuffer.reset();
    this->deviceQueues.clear();
    this->transferQueue = nullptr;
}
