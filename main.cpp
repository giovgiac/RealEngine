/**
 * main.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "GraphicsManager.h"
#include "Instance.h"
#include "Memory.h"
#include "MemoryManager.h"
#include "PoolAllocator.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

int main() {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    MemoryManager &memoryManager = MemoryManager::getManager();

    // Startup Managers
    Result<void> graphicsResult = graphicsManager.startup();
    if (graphicsResult.hasError())
        return 1;

    Result<void> memoryResult = memoryManager.startup();
    if (memoryResult.hasError())
        return 1;

    // Test Instance
    Result<std::weak_ptr<const Instance>> res = graphicsManager.getGraphicsInstance();
    if (!res.hasError()) {
        auto instance = static_cast<std::weak_ptr<const Instance>>(res);

        if (std::shared_ptr<const Instance> inst = instance.lock()) {
            Result<VkInstance> vulkanResult = inst->getVulkanInstance();

            if (!vulkanResult.hasError()) {
                auto vulkanInstance = static_cast<VkInstance>(vulkanResult);

                std::cout << "Vulkan Instance: " << vulkanInstance << std::endl;
            }
        }
    }

    // Test Memory
    const VkMemoryRequirements memoryRequirements = { 2048, 64, (1 << 5) | (1 << 6) | (1 << 8) };
    VkMemoryPropertyFlags requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    Result<uint32> result = Memory::chooseHeapFromFlags(memoryRequirements, requiredFlags);
    if (!result.hasError()) {
        auto heap = static_cast<uint32>(result);
        std::cout << "Chosen Heap: " << heap << std::endl;
    }
    else {
        std::cout << "Memory Error: " << static_cast<uint32>(result.getError()) << std::endl;
        return 1;
    }

    // Test Allocators
    Result<std::shared_ptr<PoolAllocator>> rslt = PoolAllocator::createAllocator(4096, 64, 16, requiredFlags);
    if (!rslt.hasError()) {
        auto allocator = static_cast<std::shared_ptr<PoolAllocator>>(rslt);

        // Allocate Some Memory
        Result<std::unique_ptr<Memory>> res = allocator->allocate();

        if (!res.hasError()) {
            auto memory = static_cast<std::unique_ptr<Memory>>(res);

            std::cout << "Testing Allocators..." << std::endl;
            allocator->free(memory);
        } else {
            std::cout << "Allocator Error: " << static_cast<uint32>(res.getError()) << std::endl;
            return 1;
        }
    }
    else {
        std::cout << "Allocator Error: " << static_cast<uint32>(rslt.getError()) << std::endl;
        return 1;
    }

    // Shutdown Managers
    memoryManager.shutdown();
    graphicsManager.shutdown();

    return EXIT_SUCCESS;
}
