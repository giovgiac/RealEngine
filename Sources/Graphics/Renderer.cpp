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
    this->deviceQueues = std::vector<std::shared_ptr<class Queue>>();
}

Renderer::~Renderer() {
    this->deviceQueues.clear();
}

Result<void> Renderer::flush() const noexcept {
    // Check Mutexes for Queues
    // Submit All Queues if Mutexes Available
    for (auto &queue : this->deviceQueues) {
        Result<void> result = queue->submit();

        if (result.hasError()) {
            return Result<void>::createError(result.getError());
        }
    }

    return Result<void>::createError(Error::None);
}

Result<void> Renderer::startup() {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    Result<std::weak_ptr<const Device>> result = graphicsManager.getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<std::weak_ptr<const Device>>(result);

        if (std::shared_ptr<const Device> dev = device.lock()) {
            this->deviceQueues = dev->getDeviceQueues();
            return Result<void>::createError(Error::None);
        }
    }

    return Result<void>::createError(result.getError());
}

void Renderer::shutdown() {
    this->deviceQueues.clear();
}

Result<void> Renderer::submit(Command cmd, CopyBufferInfo info) const noexcept {
    if (cmd != Command::CopyBuffer)
        return Result<void>::createError(Error::SubmitParametersNotMatching);

    // Select Queue For Command
    // Set Queue Mutex
    // Process Command and Add to Queue
    // vkCmdCopyBuffer()
    // Unset Queue Mutex

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

Result<void> Renderer::submit(Command cmd, SetBufferInfo info) const noexcept {
    if (cmd != Command::SetBuffer)
        return Result<void>::createError(Error::SubmitParametersNotMatching);

    // Select Queue For Command
    // Set Queue Mutex
    // Process Command and Add to Queue
    // vkCmdUpdateBuffer()
    // Unset Queue Mutex

    std::shared_ptr<Queue> randomQueue = this->deviceQueues[0];

    Result<VkCommandBuffer> result = randomQueue->getVulkanBuffer();
    if (!result.hasError()) {
        auto commandBuffer = static_cast<VkCommandBuffer>(result);

        Result<VkBuffer> rslt = info.buffer->getVulkanBuffer();
        if (!rslt.hasError()) {
            auto buffer = static_cast<VkBuffer>(rslt);

            // Store Command For Execution
            vkCmdUpdateBuffer(commandBuffer,
                              buffer,
                              info.offset,
                              info.size,
                              info.data);

            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(rslt.getError());
        }
    }

    return Result<void>::createError(result.getError());
}
