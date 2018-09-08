/**
 * MemoryManager.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Allocator.h"
#include "Device.h"
#include "GraphicsManager.h"
#include "MemoryManager.h"
#include "PoolAllocator.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

MemoryManager::MemoryManager() {
    this->allocatorList = std::forward_list<std::shared_ptr<PoolAllocator>>();
    this->memoryProperties = std::make_unique<VkPhysicalDeviceMemoryProperties>();
}

MemoryManager::~MemoryManager() {
    this->allocatorList.clear();

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

Result<std::shared_ptr<PoolAllocator>> MemoryManager::requestPoolAllocator(uint64 alignment,
                                                                           uint64 chunkSize,
                                                                           uint32 flags) noexcept {
    for (auto &alloc : allocatorList) {
        if (alloc->getAllocatorAlignment() == alignment &&
            alloc->getAllocatorChunkSize() == chunkSize) {

            return Result<std::shared_ptr<PoolAllocator>>(alloc);
        }
    }

    Result<std::shared_ptr<PoolAllocator>> result =
            PoolAllocator::createAllocator(chunkSize * 10,
                                           chunkSize,
                                           alignment,
                                           flags);

    if (!result.hasError()) {
        auto alloc = static_cast<std::shared_ptr<PoolAllocator>>(result);

        this->allocatorList.push_front(alloc);
        return Result<std::shared_ptr<PoolAllocator>>(alloc);
    }

    return Result<std::shared_ptr<PoolAllocator>>::createError(result.getError());
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
