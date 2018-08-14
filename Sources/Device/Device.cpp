/**
 * Device.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Device.h"
#include "Instance.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

bool Device::checkPhysicalDeviceExtensions(VkPhysicalDevice pd) const noexcept {
    return true;
}

bool Device::checkPhysicalDeviceFeatures(VkPhysicalDevice pd) const noexcept {
    return true;
}

bool Device::checkPhysicalDeviceLimits(VkPhysicalDevice pd) const noexcept {
    return true;
}

Result<void> Device::createVulkanDevice() {
    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfo = this->getDeviceQueueCreateInfo();
    VkDeviceCreateInfo deviceCreateInfo = this->getDeviceCreateInfo(&deviceQueueCreateInfo);

    if (this->physicalDevice != VK_NULL_HANDLE) {
        VkResult result = vkCreateDevice(this->physicalDevice, &deviceCreateInfo, nullptr, &this->device);
        if (result == VK_SUCCESS) {
            std::cout << "Created Logical Device..." << std::endl;
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateDevice);
        }
    }
    else {
        return Result<void>::createError(Error::NoPhysicalDeviceAvailable);
    }
}

VkDeviceCreateInfo Device::getDeviceCreateInfo(
        std::vector<struct VkDeviceQueueCreateInfo> *deviceQueueCreateInfo) const noexcept {
    VkDeviceCreateInfo deviceCreateInfo = {};

    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.ppEnabledExtensionNames = this->requiredExtensions.data();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32>(this->requiredExtensions.size());
    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfo->data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(deviceQueueCreateInfo->size());
    deviceCreateInfo.pEnabledFeatures = this->requiredFeatures.get();
    deviceCreateInfo.flags = 0;

    if (this->bIsDebug)
        deviceCreateInfo.enabledLayerCount = 0,
        deviceCreateInfo.ppEnabledLayerNames = nullptr;

    return deviceCreateInfo;
}

std::vector<VkDeviceQueueCreateInfo> Device::getDeviceQueueCreateInfo() const noexcept {
    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfo = {};
    std::vector<VkQueueFamilyProperties> queueFamilyProperties = this->getPhysicalDeviceQueueFamilyProperties();

    for (uint32 i = 0; i < static_cast<uint32>(queueFamilyProperties.size()); i++) {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            VkDeviceQueueCreateInfo createInfo = {};

            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.queueFamilyIndex = i;
            createInfo.queueCount = queueFamilyProperties[i].queueCount;
            createInfo.pQueuePriorities = nullptr;
            createInfo.flags = 0;

            deviceQueueCreateInfo.push_back(createInfo);
        }
    }

    return deviceQueueCreateInfo;
}

std::vector<VkQueueFamilyProperties> Device::getPhysicalDeviceQueueFamilyProperties() const noexcept {
    std::vector<VkQueueFamilyProperties> queueFamilyProperties = {};
    uint32 queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, nullptr);
    queueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(this->physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    return queueFamilyProperties;
}

Result<std::vector<VkPhysicalDevice>> Device::getPhysicalDevices() const noexcept {
    std::vector<VkPhysicalDevice> physicalDeviceList = {};
    uint32 physicalDeviceCount = 0;

    if (std::shared_ptr<const Instance> inst = this->instance.lock()) {
        Result<VkInstance> res = inst->getVulkanInstance();
        if (!res.hasError()) {
            auto vkInstance = static_cast<VkInstance>(res);

            vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
            physicalDeviceList.resize(physicalDeviceCount);
            vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDeviceList.data());

            return Result<std::vector<VkPhysicalDevice>>(physicalDeviceList);
        } else {
            return Result<std::vector<VkPhysicalDevice>>::createError(Error::DeviceNotStartedUp);
        }
    }

    return Result<std::vector<VkPhysicalDevice>>::createError(Error::InstanceNotStartedUp);
}

Result<void> Device::selectVulkanPhysicalDevice() {
    Result<std::vector<VkPhysicalDevice>> deviceList = this->getPhysicalDevices();

    if (!deviceList.hasError()) {
        auto physicalDeviceList = static_cast<std::vector<VkPhysicalDevice>>(deviceList);

        for (auto &&pd : physicalDeviceList) {
            if (this->checkPhysicalDeviceExtensions(pd) && this->checkPhysicalDeviceFeatures(pd) &&
                this->checkPhysicalDeviceLimits(pd)) {
                this->physicalDevice = pd;

                return Result<void>::createError(Error::None);
            }
        }

        return Result<void>::createError(Error::NoPhysicalDeviceAvailable);
    }
    else {
        return Result<void>::createError(Error::FailedRetrievingPhysicalDevices);
    }
}

Device::Device(std::weak_ptr<const Instance> inst, std::vector<const utf8 *> extensions,
               struct VkPhysicalDeviceFeatures features, struct VkPhysicalDeviceLimits limits, bool bDebug) {
    this->instance = std::move(inst);
    this->requiredExtensions = std::move(extensions);
    this->requiredFeatures = std::make_unique<VkPhysicalDeviceFeatures>(features);
    this->requiredLimits = std::make_unique<VkPhysicalDeviceLimits>(limits);
    this->bIsDebug = bDebug;
}

Device::~Device() {
    this->instance.reset();

    this->requiredExtensions = {};
    this->requiredFeatures = nullptr;
    this->requiredLimits = nullptr;
    this->bIsDebug = false;

    if (this->device != VK_NULL_HANDLE || this->physicalDevice != VK_NULL_HANDLE)
        std::cout << "WARNING: Device deleted without being shutdown..." << std::endl,
        this->shutdown();
}

Result<VkDevice> Device::getVulkanDevice() const noexcept {
    if (this->device != VK_NULL_HANDLE)
        return Result<VkDevice>(this->device);
    else
        return Result<VkDevice>::createError(Error::DeviceNotStartedUp);
}

Result<VkPhysicalDevice> Device::getVulkanPhysicalDevice() const noexcept {
    if (this->physicalDevice != VK_NULL_HANDLE)
        return Result<VkPhysicalDevice>(this->physicalDevice);
    else
        return Result<VkPhysicalDevice>::createError(Error::DeviceNotStartedUp);
}

Result<void> Device::startup() {
    if (this->selectVulkanPhysicalDevice().hasError())
        return Result<void>::createError(Error::NoPhysicalDeviceAvailable);

    if (this->createVulkanDevice().hasError())
        return Result<void>::createError(Error::FailedToCreateDevice);

    return Result<void>::createError(Error::None);
}

void Device::shutdown() {
    if (this->instance.lock())
        vkDestroyDevice(this->device, nullptr);

    this->device = VK_NULL_HANDLE;
    this->physicalDevice = VK_NULL_HANDLE;

    std::cout << "Destroyed Logical Device..." << std::endl;
}
