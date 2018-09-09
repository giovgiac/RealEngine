/**
 * main.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Buffer.h"
#include "Device.h"
#include "GraphicsManager.h"
#include "Image.h"
#include "Instance.h"
#include "Memory.h"
#include "MemoryManager.h"
#include "PoolAllocator.h"
#include "Renderer.h"
#include "Texture.h"
#include "Window.h"

#include <iostream>
#include <vulkan/vulkan.h>

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
    {
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

        // Test Device
        VkDevice device = VK_NULL_HANDLE;
        Result<std::weak_ptr<const Device>> devRes = graphicsManager.getGraphicsDevice();
        if (!res.hasError()) {
            auto deviceAbs = static_cast<std::weak_ptr<const Device>>(devRes);

            if (std::shared_ptr<const Device> dev = deviceAbs.lock()) {
                Result<VkDevice> deviceResult = dev->getVulkanDevice();

                if (!deviceResult.hasError()) {
                    device = static_cast<VkDevice>(deviceResult);

                    std::cout << "Vulkan Device: " << device << std::endl;
                }
            }
        }

        // Test Buffers
        Result<std::shared_ptr<Buffer>> bufferResult = Buffer::createBuffer(128, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
        std::shared_ptr<Buffer> buffer = nullptr;
        if (!bufferResult.hasError()) {
            buffer = static_cast<std::shared_ptr<Buffer>>(bufferResult);
            Result<VkBuffer> bufferRslt = buffer->getVulkanBuffer();

            if (!bufferRslt.hasError()) {
                auto vulkanBuffer = static_cast<VkBuffer>(bufferRslt);

                std::cout << "Created Buffer Resource: " << vulkanBuffer << std::endl;
            } else {
                std::cout << "Buffer Error: " << static_cast<uint32>(bufferRslt.getError()) << std::endl;
                return 1;
            }
        } else {
            std::cout << "Buffer Error: " << static_cast<uint32>(bufferResult.getError()) << std::endl;
            return 1;
        }

        // Test Textures
        Result<std::shared_ptr<Texture>> textureResult = Texture::createTextureFromFile("goku.png");
        if (!textureResult.hasError()) {
            auto texture = static_cast<std::shared_ptr<Texture>>(textureResult);

            auto buffer = static_cast<std::weak_ptr<Buffer>>(texture->getBuffer());
            if (std::shared_ptr<Buffer> buf = buffer.lock()) {
                VkDeviceMemory mem = static_cast<VkDeviceMemory>(buf->getVulkanMemory());

                void *readData;
                uint8 *integerData;
                vkMapMemory(device, mem, 0, VK_WHOLE_SIZE, 0, &readData);
                integerData = static_cast<uint8 *>(readData);
                for (int i = 0; i < 32; i++)
                    std::cout << "Data in Buffer " << i << ": " << +(*(integerData + i)) << std::endl;
                vkUnmapMemory(device, mem);
            } else {
                std::cout << "Failed To Lock Texture..." << std::endl;
                return 1;
            }

            Result<void> loadResult = texture->load();
            if (!loadResult.hasError()) {
                std::cout << "Successfully Loaded Texture..." << std::endl;
            } else {
                std::cout << "Failed To Load Texture..." << std::endl;
            }
        } else {
            std::cout << "Texture Error: " << static_cast<uint32>(textureResult.getError()) << std::endl;
            return 1;
        }

        // Test Renderer
        /*
        int data[32] = { 5, 4, 2, 1, 0, 2, 4, 5, 8, 10, 11 };
        Result<std::weak_ptr<const Renderer>> rendererResult = graphicsManager.getRenderer();
        if (!rendererResult.hasError()) {
            auto weak_renderer = static_cast<std::weak_ptr<const Renderer>>(rendererResult);

            if (std::shared_ptr<const Renderer> renderer = weak_renderer.lock()) {
                // Set Buffer Data
                Result<void> result = buffer->fillBuffer(0, 128, data);

                if (!result.hasError()) {
                    std::cout << "Successfully Copied Data To Buffer..." << std::endl;

                    VkDeviceMemory mem = static_cast<VkDeviceMemory>(buffer->getVulkanMemory());

                    void *readData;
                    int *integerData;
                    vkMapMemory(device, mem, 0, VK_WHOLE_SIZE, 0, &readData);
                    integerData = static_cast<int*>(readData);
                    for (int i = 0; i < 11; i++)
                        std::cout << "Data in Buffer " << i << ": " << *(integerData + i) << std::endl;
                    vkUnmapMemory(device, mem);
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
        */

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
            } else {
                std::cout << "Image Error: " << static_cast<uint32>(imageRslt.getError()) << std::endl;
                return 1;
            }
        } else {
            std::cout << "Image Error: " << static_cast<uint32>(imageResult.getError()) << std::endl;
            return 1;
        }
    }

    // Test Window
    std::shared_ptr<Window> window = std::make_shared<Window>(640, 480, "Real Engine");
    window->startup();

    while (!window->shouldClose()) {
        window->pollEvents();
    }

    window->shutdown();
    // Shutdown Managers
    memoryManager.shutdown();
    graphicsManager.shutdown();

    return EXIT_SUCCESS;
}
