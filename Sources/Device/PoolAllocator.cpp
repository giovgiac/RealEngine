/**
 * PoolAllocator.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "PoolAllocator.h"
#include "Device.h"
#include "GraphicsManager.h"
#include "Memory.h"

#include <vulkan/vulkan.hpp>

PoolAllocator::PoolAllocator() {
    this->alignment = 0;
    this->chunkSize = 0;
    this->flags = 0;
    this->freeList = std::forward_list<std::unique_ptr<Memory>>();
    this->heap = 0;
    this->memory = VK_NULL_HANDLE;
    this->size = 0;
}

void PoolAllocator::chunkMemory() {
    this->freeList.clear();

    for (VkDeviceSize i = this->size / this->chunkSize - 1; i * this->chunkSize > 0; i--)
        this->freeList.emplace_front(Memory::createMemory(this->memory, i * this->chunkSize, heap));
    this->freeList.emplace_front(Memory::createMemory(this->memory, 0, heap));
}

Result<VkDevice> PoolAllocator::getGraphicsDevice() const noexcept {
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

VkMemoryAllocateInfo PoolAllocator::getMemoryAllocateInfo() const noexcept {
    VkMemoryAllocateInfo memoryAllocateInfo = {};

    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = nullptr;
    memoryAllocateInfo.allocationSize = this->size;
    memoryAllocateInfo.memoryTypeIndex = this->heap;

    return memoryAllocateInfo;
}

VkMemoryRequirements PoolAllocator::getMemoryRequirements() const noexcept {
    VkMemoryRequirements memoryRequirements = {};

    memoryRequirements.size = this->size;
    memoryRequirements.alignment = this->alignment;
    memoryRequirements.memoryTypeBits = 0;
    memoryRequirements.memoryTypeBits ^= ~memoryRequirements.memoryTypeBits;

    return memoryRequirements;
}

PoolAllocator::~PoolAllocator() {
    this->freeList.clear();

    if (this->memory != VK_NULL_HANDLE) {
        Result<VkDevice> result = this->getGraphicsDevice();

        if (!result.hasError()) {
            auto device = static_cast<VkDevice>(result);

            vkFreeMemory(device, this->memory, nullptr);
            this->memory = VK_NULL_HANDLE;
        }
    }
}

Result<std::unique_ptr<Memory>> PoolAllocator::allocate(VkDeviceSize siz) {
    if (!this->freeList.empty()) {
        std::unique_ptr<Memory> mem = std::move(this->freeList.front());

        this->freeList.pop_front();
        return Result<std::unique_ptr<Memory>>(std::move(mem));
    }

    return Result<std::unique_ptr<Memory>>::createError(Error::NoMemoryAvailableInAllocator);
}

void PoolAllocator::free(std::unique_ptr<Memory> &mem) {
    this->freeList.emplace_front(std::move(mem));
}

Result<std::shared_ptr<PoolAllocator>> PoolAllocator::createAllocator(VkDeviceSize initialSize,
                                                                      VkDeviceSize partitionSize,
                                                                      VkDeviceSize alignment,
                                                                      VkMemoryPropertyFlags flags) {
    std::shared_ptr<PoolAllocator> allocator(new PoolAllocator);

    allocator->alignment = alignment;
    allocator->chunkSize = partitionSize;
    allocator->flags = flags;
    allocator->size = initialSize;

    Result<uint32> res = Memory::chooseHeapFromFlags(allocator->getMemoryRequirements(), flags);

    if (!res.hasError()) {
        auto hp = static_cast<uint32>(res);
        allocator->heap = hp;

        // Allocate Memory with Vulkan
        Result<VkDevice> rslt = allocator->getGraphicsDevice();
        VkMemoryAllocateInfo memoryAllocateInfo = allocator->getMemoryAllocateInfo();

        if (!rslt.hasError()) {
            auto device = static_cast<VkDevice>(rslt);
            VkResult result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &allocator->memory);

            if (result == VK_SUCCESS) {
                allocator->chunkMemory();
                return Result<std::shared_ptr<PoolAllocator>>(std::move(allocator));
            }
            else
                return Result<std::shared_ptr<PoolAllocator>>::createError(Error::FailedToAllocateDeviceMemory);
        }
        else
            return Result<std::shared_ptr<PoolAllocator>>::createError(rslt.getError());
    }

    return Result<std::shared_ptr<PoolAllocator>>::createError(res.getError());
}
