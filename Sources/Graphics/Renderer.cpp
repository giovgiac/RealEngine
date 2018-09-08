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

Renderer::Renderer() {
    this->deviceQueues = std::vector<std::shared_ptr<Queue>>();
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

void Renderer::shutdown() {
    this->deviceQueues.clear();
}

/*
Result<void> Renderer::submit(Command cmd, CopyBufferInfo info) const noexcept {
    if (cmd != Command::CopyBuffer)
        return Result<void>::createError(Error::SubmitParametersNotMatching);

    std::shared_ptr<Queue> randomQueue = this->deviceQueues[0];

    Result<VkCommandBuffer> result = randomQueue->getVulkanBuffer();
    if (!result.hasError()) {
        auto commandBuffer = static_cast<VkCommandBuffer>(result);

        Result<VkBuffer> rslt = info.src->getVulkanBuffer();
        if (!rslt.hasError()) {
            auto src = static_cast<VkBuffer>(rslt);

            Result<VkBuffer> res = info.dst->getVulkanBuffer();
            if (!res.hasError()) {
                auto dst = static_cast<VkBuffer>(res);

                // Store Command For Execution
                vkCmdCopyBuffer(commandBuffer,
                                src,
                                dst,
                                info.regionCount,
                                info.regions);

                return Result<void>::createError(Error::None);
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
*/
