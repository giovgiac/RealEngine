/**
 * Memory.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Memory.h"
#include "MemoryManager.h"

#include <vulkan/vulkan.hpp>

Memory::Memory() {
    this->heap = 0;
    this->memory = nullptr;
    this->offset = 0;
}

Memory::~Memory() {
    this->heap = 0;
    this->memory = VK_NULL_HANDLE;
    this->offset = 0;
}

Result<uint32> Memory::chooseHeapFromFlags(const VkMemoryRequirements &memoryRequirements,
                                           VkMemoryPropertyFlags requiredFlags) noexcept {
    MemoryManager &memoryManager = MemoryManager::getManager();
    Result<VkPhysicalDeviceMemoryProperties> result = memoryManager.getMemoryProperties();

    if (!result.hasError()) {
        auto memoryProperties = static_cast<VkPhysicalDeviceMemoryProperties>(result);

        uint32 selectedType = 0;
        for (uint32 memoryType = 0; memoryType < 32; memoryType++) {
            if (memoryRequirements.memoryTypeBits & (1 << memoryType)) {
                const VkMemoryType &type = memoryProperties.memoryTypes[memoryType];

                if ((type.propertyFlags & requiredFlags) == requiredFlags) {
                    selectedType = memoryType;
                    break;
                }
            }
        }

        return Result<uint32>(selectedType);
    }

    return Result<uint32>::createError(result.getError());
}

std::unique_ptr<Memory> Memory::createMemory(VkDeviceMemory mem, VkDeviceSize off, uint32 hp) noexcept {
    std::unique_ptr<Memory> newMemory(new Memory);

    newMemory->heap = hp;
    newMemory->memory = mem;
    newMemory->offset = off;

    return std::move(newMemory);
}
