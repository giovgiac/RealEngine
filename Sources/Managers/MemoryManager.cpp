/**
 * MemoryManager.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Device.h"
#include "GraphicsManager.h"
#include "MemoryManager.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

MemoryManager::MemoryManager() {
    memoryProperties = std::make_unique<VkPhysicalDeviceMemoryProperties>();
}

MemoryManager::~MemoryManager() {
    if (this->memoryProperties != nullptr)
        std::cout << "WARNING: MemoryManager deleted without being shutdown..." << std::endl,
        this->shutdown();
}

Result<VkPhysicalDevice> MemoryManager::getPhysicalDevice() const noexcept {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    Result<std::weak_ptr<const Device>> result = graphicsManager.getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<std::weak_ptr<const Device>>(result);

        if (std::shared_ptr<const Device> dev = device.lock())
            return dev->getVulkanPhysicalDevice();
        else
            return Result<VkPhysicalDevice>::createError(Error::GraphicsManagerNotStartedUp);
    }

    return Result<VkPhysicalDevice>::createError(result.getError());
}

Result<void> MemoryManager::getPhysicalDeviceMemoryProperties() noexcept {
    Result<VkPhysicalDevice> result = this->getPhysicalDevice();

    if (!result.hasError()) {
        auto physicalDevice = static_cast<VkPhysicalDevice>(result);

        vkGetPhysicalDeviceMemoryProperties(physicalDevice, this->memoryProperties.get());
        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(result.getError());
}

Result<VkPhysicalDeviceMemoryProperties> MemoryManager::getMemoryProperties() const noexcept {
    if (this->memoryProperties != nullptr)
        return Result<VkPhysicalDeviceMemoryProperties>(*this->memoryProperties);
    else
        return Result<VkPhysicalDeviceMemoryProperties>::createError(Error::MemoryManagerNotStartedUp);
}

Result<void> MemoryManager::startup() {
    std::cout << "Starting Up MemoryManager..." << std::endl;

    Result<void> result = this->getPhysicalDeviceMemoryProperties();
    if (result.hasError())
        return Result<void>::createError(result.getError());

    return Result<void>::createError(Error::None);
}

void MemoryManager::shutdown() {
    // Clear objects
    this->memoryProperties.reset();

    std::cout << "Shutting Down MemoryManager..." << std::endl;
}