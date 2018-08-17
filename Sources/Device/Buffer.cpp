/**
 * Buffer.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Buffer.h"
#include "Device.h"
#include "GraphicsManager.h"
#include "Memory.h"
#include "Queue.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

Buffer::Buffer() {
    this->buffer = VK_NULL_HANDLE;
    this->memory = nullptr;
    this->queueList = {};
    this->sharingMode = VK_SHARING_MODE_MAX_ENUM;
    this->size = 0;
    this->usage = 0;
}

void Buffer::allocateMemory() {

}

VkBufferCreateInfo Buffer::getBufferCreateInfo() const noexcept {
    VkBufferCreateInfo bufferCreateInfo = {};

    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.sharingMode = static_cast<VkSharingMode>(this->sharingMode);
    bufferCreateInfo.size = this->size;
    bufferCreateInfo.usage = this->usage;
    bufferCreateInfo.flags = 0;

    if (bufferCreateInfo.sharingMode == VK_SHARING_MODE_CONCURRENT) {
        // TODO: Extract Queues from Queue Class
        bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32>(queueList.size());
    }
    else {
        bufferCreateInfo.pQueueFamilyIndices = nullptr;
        bufferCreateInfo.queueFamilyIndexCount = 0;
    }

    return bufferCreateInfo;
}

Result<VkDevice> Buffer::getGraphicsDevice() const noexcept {
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

Buffer::~Buffer() {
    this->queueList.clear();

    if (this->buffer != VK_NULL_HANDLE) {
        Result<VkDevice> result = this->getGraphicsDevice();

        if (!result.hasError()) {
            auto device = static_cast<VkDevice>(result);

            std::cout << "Destroying Buffer Resource: " << this->buffer << std::endl;

            vkDestroyBuffer(device, this->buffer, nullptr);
            this->buffer = VK_NULL_HANDLE;
        }
    }
}

Result<std::shared_ptr<Buffer>> Buffer::createBuffer(VkDeviceSize siz, VkBufferUsageFlags usg) {
    std::shared_ptr<Buffer> buffer(new Buffer);

    buffer->size = siz;
    buffer->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer->usage = usg;

    Result<VkDevice> result = buffer->getGraphicsDevice();
    VkBufferCreateInfo bufferCreateInfo = buffer->getBufferCreateInfo();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkResult rslt = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer->buffer);

        if (rslt == VK_SUCCESS) {
            buffer->allocateMemory();
            return Result<std::shared_ptr<Buffer>>(std::move(buffer));
        }
        else
            return Result<std::shared_ptr<Buffer>>::createError(Error::FailedToCreateBuffer);
    }

    return Result<std::shared_ptr<Buffer>>::createError(result.getError());
}

Result<std::shared_ptr<Buffer>> Buffer::createSharedBuffer(VkDeviceSize siz,
                                                           VkBufferUsageFlags usg,
                                                           std::vector<std::weak_ptr<Queue>> &queues) {
    std::shared_ptr<Buffer> buffer(new Buffer);

    buffer->size = siz;
    buffer->sharingMode = VK_SHARING_MODE_CONCURRENT;
    buffer->usage = usg;
    buffer->queueList = std::move(queues);

    Result<VkDevice> result = buffer->getGraphicsDevice();
    VkBufferCreateInfo bufferCreateInfo = buffer->getBufferCreateInfo();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkResult rslt = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer->buffer);

        if (rslt == VK_SUCCESS) {
            buffer->allocateMemory();
            return Result<std::shared_ptr<Buffer>>(std::move(buffer));
        }
        else
            return Result<std::shared_ptr<Buffer>>::createError(Error::FailedToCreateBuffer);
    }

    return Result<std::shared_ptr<Buffer>>::createError(result.getError());
}

Result<VkBuffer> Buffer::getVulkanBuffer() const noexcept {
    if (this->buffer != VK_NULL_HANDLE)
        return Result<VkBuffer>(this->buffer);
    else
        return Result<VkBuffer>::createError(Error::FailedToRetrieveBuffer);
}
