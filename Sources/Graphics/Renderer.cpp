/**
 * Renderer.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Device.h"
#include "GraphicsManager.h"
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

}

Renderer::~Renderer() {

}

Result<void> Renderer::flush() noexcept {
    // Check Mutexes for Queues
    // Submit All Queues if Mutexes Available
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

Result<void> Renderer::submit(Command cmd, CopyBufferInfo info) noexcept {
    if (cmd != Command::CopyBuffer)
        return Result<void>::createError(Error::SubmitParametersNotMatching);

    // Select Queue For Command
    // Set Queue Mutex
    // Process Command and Add to Queue
    // vkCmdCopyBuffer()
    // Unset Queue Mutex
}

Result<void> Renderer::submit(Command cmd, SetBufferInfo info) noexcept {
    if (cmd != Command::SetBuffer)
        return Result<void>::createError(Error::SubmitParametersNotMatching);

    // Select Queue For Command
    // Set Queue Mutex
    // Process Command and Add to Queue
    // vkCmdUpdateBuffer()
    // Unset Queue Mutex
}
