/**
 * Texture.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Buffer.h"
#include "Device.h"
#include "GraphicsManager.h"
#include "Image.h"
#include "Renderer.h"
#include "Texture.h"
#include "WorldManager.h"

#include <FreeImage.h>
#include <vulkan/vulkan.h>

Texture::Texture() {
    this->width = 0;
    this->height = 0;
    this->buffer = nullptr;
    this->image = nullptr;
    this->view = VK_NULL_HANDLE;
}

Result<void> Texture::createImageView() {
    VkComponentMapping components = {};
    VkImageSubresourceRange subresources = {};
    VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D;

    // Configure Components
    components.r = VK_COMPONENT_SWIZZLE_R;
    components.g = VK_COMPONENT_SWIZZLE_G;
    components.b = VK_COMPONENT_SWIZZLE_B;
    components.a = VK_COMPONENT_SWIZZLE_A;

    // Configure Subresource
    subresources.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresources.baseArrayLayer = 0;
    subresources.layerCount = 1;
    subresources.baseMipLevel = 0;
    subresources.levelCount = 1;

    Result<VkImageView> result = this->image->getImageView(components,
                                                           subresources,
                                                           type);
    if (!result.hasError()) {
        this->view = static_cast<VkImageView>(result);
        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(result.getError());
}

VkBufferImageCopy Texture::getBufferImageCopy() const noexcept {
    VkBufferImageCopy bufferImageCopy = {};

    bufferImageCopy.bufferOffset = 0;
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.bufferImageHeight = 0;
    bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferImageCopy.imageSubresource.mipLevel = 0;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.layerCount = 1;
    bufferImageCopy.imageExtent = { width, height, 1 };
    bufferImageCopy.imageOffset = { 0, 0, 0 };

    return bufferImageCopy;
}

Result<VkDevice> Texture::getGraphicsDevice() const noexcept {
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

Result<std::shared_ptr<Renderer>> Texture::getRenderer() const noexcept {
    WorldManager &worldManager = WorldManager::getManager();
    return worldManager.getRenderer();
}

Result<RawImageInfo> Texture::loadImage(const utf8 *filename) const noexcept {
    FREE_IMAGE_FORMAT fmt = FreeImage_GetFileType(filename);
    RawImageInfo info = {};

    if (fmt != FIF_UNKNOWN) {
        FIBITMAP *img = FreeImage_Load(fmt, filename);
        FIBITMAP *tmp = FreeImage_ConvertTo32Bits(img);

        if (img && tmp) {
            FreeImage_Unload(img);
            img = tmp;

            // Configure Image Info
            info.width = FreeImage_GetWidth(img);
            info.height = FreeImage_GetHeight(img);
            info.pixels = static_cast<uint8*>(FreeImage_GetBits(img));

            return Result<RawImageInfo>(info);
        }
        else {
            return Result<RawImageInfo>::createError(Error::FailedToLoadImage);
        }
    }

    return Result<RawImageInfo>::createError(Error::UnknownImageFormat);
}

Texture::~Texture() {
    this->buffer.reset();
    this->image.reset();
    this->width = 0;
    this->height = 0;

    if (this->view != VK_NULL_HANDLE) {
        Result<VkDevice> result = this->getGraphicsDevice();

        if (!result.hasError()) {
            auto device = static_cast<VkDevice>(result);
            vkDestroyImageView(device, this->view, nullptr);
        }
    }
}

Result<std::shared_ptr<Texture>> Texture::createTextureFromFile(const utf8 *filename) {
    std::shared_ptr<Texture> texture(new Texture);

    // Load Image
    Result<RawImageInfo> imageResult = texture->loadImage(filename);
    if (!imageResult.hasError()) {
        auto rawImageInfo = static_cast<RawImageInfo>(imageResult);

        VkDeviceSize size = 4 * rawImageInfo.width * rawImageInfo.height;

        Result<std::shared_ptr<Buffer>> bufferResult = Buffer::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        if (!bufferResult.hasError()) {
            texture->buffer = static_cast<std::shared_ptr<Buffer>>(bufferResult);
            texture->buffer->fillBuffer(size, static_cast<void*>(rawImageInfo.pixels));

            VkExtent3D extent = {};
            texture->width = extent.width = rawImageInfo.width;
            texture->height = extent.height = rawImageInfo.height;
            extent.depth = 1;

            Result<std::shared_ptr<Image>> imageResult = Image::createImage(extent,
                                                                            VK_IMAGE_TYPE_2D,
                                                                            1,
                                                                            1,
                                                                            VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                            VK_IMAGE_USAGE_SAMPLED_BIT,
                                                                            VK_FORMAT_R8G8B8A8_UNORM,
                                                                            VK_IMAGE_TILING_OPTIMAL);

            if (!imageResult.hasError()) {
                texture->image = static_cast<std::shared_ptr<Image>>(imageResult);
                return Result<std::shared_ptr<Texture>>(std::move(texture));
            }
            else {
                return Result<std::shared_ptr<Texture>>::createError(imageResult.getError());
            }
        }
        else {
            return Result<std::shared_ptr<Texture>>::createError(bufferResult.getError());
        }
    }

    return Result<std::shared_ptr<Texture>>::createError(imageResult.getError());
}

Result<std::weak_ptr<class Buffer>> Texture::getBuffer() const noexcept {
    if (this->buffer != nullptr)
        return Result<std::weak_ptr<class Buffer>>(this->buffer);
    else
        return Result<std::weak_ptr<class Buffer>>::createError(Error::FailedToRetrieveBuffer);
}

VkImageView Texture::getImageView() const noexcept {
    return this->view;
}


Result<void> Texture::load() {
    Result<std::shared_ptr<Renderer>> result = this->getRenderer();

    if (!result.hasError()) {
        auto renderer = static_cast<std::shared_ptr<Renderer>>(result);
        Result<VkCommandBuffer> rslt = renderer->requestTransferBuffer();

        if (!rslt.hasError()) {
            auto buf = static_cast<VkBuffer>(this->buffer->getVulkanBuffer());
            auto img = static_cast<VkImage>(this->image->getVulkanImage());
            auto transferBuffer = static_cast<VkCommandBuffer>(rslt);
            VkBufferImageCopy bufferImageCopy = this->getBufferImageCopy();

            this->image->transitionLayout(transferBuffer,
                                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                          VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                          VK_PIPELINE_STAGE_TRANSFER_BIT);
            vkCmdCopyBufferToImage(transferBuffer,
                                   buf,
                                   img,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   1,
                                   &bufferImageCopy);
            this->image->transitionLayout(transferBuffer,
                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                          VK_PIPELINE_STAGE_TRANSFER_BIT,
                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);


            Result<void> executeResult = renderer->executeTransferBuffer(transferBuffer);
            if (!executeResult.hasError()) {
                Result<void> viewResult = this->createImageView();

                if (!viewResult.hasError()) {
                    return Result<void>::createError(Error::None);
                }
                else {
                    return Result<void>::createError(viewResult.getError());
                }
            }
            else {
                return Result<void>::createError(executeResult.getError());
            }
        }
        else {
            return Result<void>::createError(rslt.getError());
        }
    }

    return Result<void>::createError(result.getError());
}
