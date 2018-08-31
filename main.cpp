/**
 * main.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Buffer.h"
#include "GraphicsManager.h"
#include "Image.h"
#include "Instance.h"
#include "Memory.h"
#include "MemoryManager.h"
#include "PoolAllocator.h"
#include "Renderer.h"

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

    // Test Buffers
    Result<std::shared_ptr<Buffer>> bufferResult = Buffer::createBuffer(128, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    std::shared_ptr<Buffer> buffer = nullptr;
    if (!bufferResult.hasError()) {
        buffer = static_cast<std::shared_ptr<Buffer>>(bufferResult);
        Result<VkBuffer> bufferRslt = buffer->getVulkanBuffer();

        if (!bufferRslt.hasError()) {
            auto vulkanBuffer = static_cast<VkBuffer>(bufferRslt);

            std::cout << "Created Buffer Resource: " << vulkanBuffer << std::endl;
        }
        else {
            std::cout << "Buffer Error: " << static_cast<uint32>(bufferRslt.getError()) << std::endl;
            return 1;
        }
    }
    else {
        std::cout << "Buffer Error: " << static_cast<uint32>(bufferResult.getError()) << std::endl;
        return 1;
    }

    // Test Renderer
    int data[32] = { 5, 4, 2, 1, 0, 2, 4, 5, 8, 10, 11 };
    Result<std::weak_ptr<const Renderer>> rendererResult = graphicsManager.getRenderer();
    if (!rendererResult.hasError()) {
        auto weak_renderer = static_cast<std::weak_ptr<const Renderer>>(rendererResult);

        if (std::shared_ptr<const Renderer> renderer = weak_renderer.lock()) {
            // Configure Command
            SetBufferInfo setBufferInfo = {};

            setBufferInfo.buffer = buffer;
            setBufferInfo.size = 128;
            setBufferInfo.offset = 0;
            setBufferInfo.data = &data;

            // Send Command
            Result<void> result = renderer->submit(Command::SetBuffer, setBufferInfo);
            if (!result.hasError()) {
                Result<void> rslt = renderer->flush();

                if (!rslt.hasError()) {
                    std::cout << "Successfully Used Set Buffer Command and Executed It..." << std::endl;
                }
                else {
                    std::cout << "Flush Renderer Error: " << static_cast<uint32>(rslt.getError()) << std::endl;
                    return 1;
                }
            }
            else {
                std::cout << "Submit Renderer Error: " << static_cast<uint32>(result.getError()) << std::endl;
                return 1;
            }
        }
        else {
            std::cout << "Renderer Error: Failed To Solidify Pointer..." << std::endl;
            return 1;
        }
    }
    else {
        std::cout << "Renderer Error: " << static_cast<uint32>(rendererResult.getError()) << std::endl;
        return 1;
    }

    // Test Images
    Result<std::shared_ptr<Image>> imageResult = Image::createImage({1024, 1024, 1},
                                                                    VK_IMAGE_TYPE_2D,
                                                                    1,
                                                                    8,
                                                                    VK_IMAGE_USAGE_SAMPLED_BIT &
                                                                    VK_IMAGE_USAGE_STORAGE_BIT,
                                                                    VK_FORMAT_R8G8B8A8_UNORM,
                                                                    VK_IMAGE_TILING_OPTIMAL);
    if (!imageResult.hasError()) {
        auto image = static_cast<std::shared_ptr<Image>>(imageResult);
        Result<VkImage> imageRslt = image->getVulkanImage();

        if (!imageRslt.hasError()) {
            auto vulkanImage = static_cast<VkImage>(imageRslt);

            std::cout << "Created Image Resource: " << vulkanImage << std::endl;
        }
        else {
            std::cout << "Image Error: " << static_cast<uint32>(imageRslt.getError()) << std::endl;
            return 1;
        }
    }
    else {
        std::cout << "Image Error: " << static_cast<uint32>(imageResult.getError()) << std::endl;
        return 1;
    }

    // Shutdown Managers
    memoryManager.shutdown();
    graphicsManager.shutdown();

    return EXIT_SUCCESS;
}
