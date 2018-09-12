/**
 * Queue.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Device.h"
#include "GraphicsManager.h"
#include "Queue.h"

#include <vulkan/vulkan.h>

Queue::Queue() {
    this->buffer = VK_NULL_HANDLE;
    this->pool = VK_NULL_HANDLE;
    this->queue = VK_NULL_HANDLE;
    this->familyIndex = 0;
    this->queueIndex = 0;
}

VkCommandBufferAllocateInfo Queue::getCommandBufferAllocateInfo() const noexcept {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};

    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandBufferCount = 1;
    commandBufferAllocateInfo.commandPool = this->pool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    return commandBufferAllocateInfo;
}

VkCommandBufferBeginInfo Queue::getCommandBufferBeginInfo() const noexcept {
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    
    return commandBufferBeginInfo;
}

VkCommandPoolCreateInfo Queue::getCommandPoolCreateInfo() const noexcept {
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};

    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.queueFamilyIndex = this->familyIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    return commandPoolCreateInfo;
}

Result<VkDevice> Queue::getGraphicsDevice() const noexcept {
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

VkSubmitInfo Queue::getSubmitInfo(VkSemaphore const *signals,
                                  uint32 signalCount,
                                  VkSemaphore const *waits,
                                  uint32 waitCount,
                                  uint32 *stages) const noexcept {
    VkSubmitInfo submitInfo = {};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->buffer;
    submitInfo.waitSemaphoreCount = waitCount;
    submitInfo.pWaitSemaphores = waits;
    submitInfo.signalSemaphoreCount = signalCount;
    submitInfo.pSignalSemaphores = signals;
    submitInfo.pWaitDstStageMask = stages;

    return submitInfo;
}

Queue::~Queue() {
    Result<VkDevice> result = this->getGraphicsDevice();
    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);

        if (this->buffer != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(device, this->pool, 1, &this->buffer);
            this->buffer = VK_NULL_HANDLE;
        }

        if (this->pool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(device, this->pool, nullptr);
            this->pool = VK_NULL_HANDLE;
        }
    }
}

void Queue::bindPipeline(VkPipeline pipeline) {
    //vkCmdBindPipeline(this->buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

Result<std::shared_ptr<Queue>> Queue::createQueue(VkDevice device,
                                                  uint32 familyIndex,
                                                  uint32 queueIndex) {
    std::shared_ptr<Queue> queue(new Queue);

    queue->familyIndex = familyIndex;
    queue->queueIndex = queueIndex;

    // Retrieve Queue from Device
    vkGetDeviceQueue(device, familyIndex, queueIndex, &queue->queue);

    VkCommandPoolCreateInfo commandPoolCreateInfo = queue->getCommandPoolCreateInfo();
    VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &queue->pool);

    if (result == VK_SUCCESS) {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = queue->getCommandBufferAllocateInfo();
        VkResult rslt = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &queue->buffer);
        
        if (rslt == VK_SUCCESS) {
            VkCommandBufferBeginInfo commandBufferBeginInfo = queue->getCommandBufferBeginInfo();
            
            VkResult res = vkBeginCommandBuffer(queue->buffer, &commandBufferBeginInfo);
            if (res == VK_SUCCESS) {
                return Result<std::shared_ptr<Queue>>(std::move(queue));
            }
            else {
                return Result<std::shared_ptr<Queue>>::createError(Error::FailedToAllocateCommandBuffer);
            }
        }
        else {
            return Result<std::shared_ptr<Queue>>::createError(Error::FailedToAllocateCommandBuffer);
        }
    }

    return Result<std::shared_ptr<Queue>>::createError(Error::FailedToCreateCommandPool);
}

Result<VkCommandBuffer> Queue::getVulkanBuffer() const noexcept {
    if (this->buffer != VK_NULL_HANDLE)
        return Result<VkCommandBuffer>(this->buffer);
    else
        return Result<VkCommandBuffer>::createError(Error::FailedToRetrieveQueue);
}

Result<VkCommandPool> Queue::getVulkanPool() const noexcept {
    if (this->pool != VK_NULL_HANDLE)
        return Result<VkCommandPool>(this->pool);
    else
        return Result<VkCommandPool>::createError(Error::FailedToRetrieveQueue);
}

Result<VkQueue> Queue::getVulkanQueue() const noexcept {
    if (this->queue != VK_NULL_HANDLE)
        return Result<VkQueue>(this->queue);
    else
        return Result<VkQueue>::createError(Error::FailedToRetrieveQueue);
}

void Queue::resetBuffers() {
    VkCommandBufferBeginInfo commandBufferBeginInfo = this->getCommandBufferBeginInfo();

    vkResetCommandBuffer(this->buffer, 0);
    vkBeginCommandBuffer(this->buffer, &commandBufferBeginInfo);
}

Result<void> Queue::submit(VkSemaphore const *signals,
                           uint32 signalCount,
                           VkSemaphore const *waits,
                           uint32 waitCount,
                           uint32 *stages) const noexcept {
    VkSubmitInfo submitInfo = this->getSubmitInfo(signals, signalCount, waits, waitCount, stages);
    
    vkEndCommandBuffer(this->buffer);
    VkResult result = vkQueueSubmit(this->queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (result == VK_SUCCESS) {
        return Result<void>::createError(Error::None);
    }
    else {
        return Result<void>::createError(Error::FailedToSubmitQueue);
    }
}
