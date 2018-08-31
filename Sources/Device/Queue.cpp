/**
 * Queue.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Device.h"
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

VkCommandPoolCreateInfo Queue::getCommandPoolCreateInfo() const noexcept {
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};

    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = nullptr;
    commandPoolCreateInfo.queueFamilyIndex = this->familyIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    return commandPoolCreateInfo;
}

VkSubmitInfo Queue::getSubmitInfo() const noexcept {
    VkSubmitInfo submitInfo = {};

    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &this->buffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;

    return submitInfo;
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
            return Result<std::shared_ptr<Queue>>(std::move(queue));
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

Result<void> Queue::submit() const noexcept {
    VkSubmitInfo submitInfo = this->getSubmitInfo();

    VkResult result = vkQueueSubmit(this->queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (result == VK_SUCCESS) {
        return Result<void>::createError(Error::None);
    }
    else {
        return Result<void>::createError(Error::FailedToSubmitQueue);
    }
}