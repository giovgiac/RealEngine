/**
 * Image.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Image.h"
#include "Device.h"
#include "GraphicsManager.h"
#include "Memory.h"
#include "Queue.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

Image::Image() {
    this->image = VK_NULL_HANDLE;
    this->sharingMode = VK_SHARING_MODE_MAX_ENUM;
    this->usage = 0;
    this->type = VK_IMAGE_TYPE_MAX_ENUM;
    this->format = VK_FORMAT_MAX_ENUM;
    this->extent = {};
    this->samples = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
    this->tiling = VK_IMAGE_TILING_MAX_ENUM;
    this->layout = VK_IMAGE_LAYOUT_MAX_ENUM;
    this->mipLevels = 0;
    this->arrayLayers = 0;
    this->memory = nullptr;
    this->queueList = {};
}

void Image::allocateMemory() {

}

Result<VkDevice> Image::getGraphicsDevice() const noexcept {
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

VkImageCreateInfo Image::getImageCreateInfo() const noexcept {
    VkImageCreateInfo imageCreateInfo = {};

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = static_cast<VkImageType>(this->type);
    imageCreateInfo.format = static_cast<VkFormat>(this->format);
    imageCreateInfo.extent = *this->extent;
    imageCreateInfo.mipLevels = this->mipLevels;
    imageCreateInfo.arrayLayers = this->arrayLayers;
    imageCreateInfo.samples = static_cast<VkSampleCountFlagBits>(this->samples);
    imageCreateInfo.tiling = static_cast<VkImageTiling>(this->tiling);
    imageCreateInfo.usage = static_cast<VkImageUsageFlags>(this->usage);
    imageCreateInfo.sharingMode = static_cast<VkSharingMode>(this->sharingMode);
    imageCreateInfo.initialLayout = static_cast<VkImageLayout>(this->layout);

    if (imageCreateInfo.sharingMode == VK_SHARING_MODE_CONCURRENT) {
        // TODO: Extract Queues from Queue Class
        imageCreateInfo.queueFamilyIndexCount = static_cast<uint32>(queueList.size());
    }
    else {
        imageCreateInfo.pQueueFamilyIndices = nullptr;
        imageCreateInfo.queueFamilyIndexCount = 0;
    }

    return imageCreateInfo;
}

Image::~Image() {
    this->queueList.clear();

    if (this->image != VK_NULL_HANDLE) {
        Result<VkDevice> result = this->getGraphicsDevice();

        if (!result.hasError()) {
            auto device = static_cast<VkDevice>(result);

            std::cout << "Destroying Image Resource: " << this->image << std::endl;

            vkDestroyImage(device, image, nullptr);
            image = VK_NULL_HANDLE;
        }
    }
}

Result<std::shared_ptr<Image>> Image::createImage(VkExtent3D ext,
                                                  uint32 tp,
                                                  uint32 layers,
                                                  uint32 mips,
                                                  VkImageUsageFlags usg,
                                                  uint32 fmt,
                                                  uint32 tlng) {
    std::shared_ptr<Image> image(new Image);

    image->type = tp;
    image->format = fmt;
    image->extent = std::make_unique<VkExtent3D>(ext);
    image->mipLevels = mips;
    image->arrayLayers = layers;
    image->samples = VK_SAMPLE_COUNT_1_BIT;
    image->tiling = tlng;
    image->usage = usg;
    image->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image->layout = VK_IMAGE_LAYOUT_UNDEFINED;

    Result<VkDevice> result = image->getGraphicsDevice();
    VkImageCreateInfo imageCreateInfo = image->getImageCreateInfo();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkResult rslt = vkCreateImage(device, &imageCreateInfo, nullptr, &image->image);

        if (rslt == VK_SUCCESS) {
            image->allocateMemory();
            return Result<std::shared_ptr<Image>>(std::move(image));
        }
        else
            return Result<std::shared_ptr<Image>>::createError(Error::FailedToCreateImage);
    }

    return Result<std::shared_ptr<Image>>::createError(result.getError());
}

Result<std::shared_ptr<Image>> Image::createSharedImage(VkExtent3D ext,
                                                        uint32 tp,
                                                        uint32 layers,
                                                        uint32 mips,
                                                        VkImageUsageFlags usg,
                                                        uint32 fmt,
                                                        uint32 tlng,
                                                        std::vector<std::weak_ptr<Queue>> &queues) {
    std::shared_ptr<Image> image(new Image);

    image->type = tp;
    image->format = fmt;
    image->extent = std::make_unique<VkExtent3D>(ext);
    image->mipLevels = mips;
    image->arrayLayers = layers;
    image->samples = VK_SAMPLE_COUNT_1_BIT;
    image->tiling = tlng;
    image->usage = usg;
    image->sharingMode = VK_SHARING_MODE_CONCURRENT;
    image->layout = VK_IMAGE_LAYOUT_UNDEFINED;
    image->queueList = std::move(queues);

    Result<VkDevice> result = image->getGraphicsDevice();
    VkImageCreateInfo imageCreateInfo = image->getImageCreateInfo();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkResult rslt = vkCreateImage(device, &imageCreateInfo, nullptr, &image->image);

        if (rslt == VK_SUCCESS) {
            image->allocateMemory();
            return Result<std::shared_ptr<Image>>(std::move(image));
        }
        else
            return Result<std::shared_ptr<Image>>::createError(Error::FailedToCreateImage);
    }

    return Result<std::shared_ptr<Image>>::createError(result.getError());
}

Result<VkImage> Image::getVulkanImage() const noexcept {
    if (this->image != VK_NULL_HANDLE)
        return Result<VkImage>(this->image);
    else
        return Result<VkImage>::createError(Error::FailedToRetrieveImage);
}
