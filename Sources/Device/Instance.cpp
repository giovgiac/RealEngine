/**
 * Instance.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Instance.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

VkApplicationInfo Instance::getApplicationInfo() const noexcept {
    VkApplicationInfo applicationInfo = {};

    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = this->applicationName;
    applicationInfo.applicationVersion = this->applicationVersion;
    applicationInfo.pEngineName = "Real Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    return applicationInfo;
}

VkInstanceCreateInfo Instance::getInstanceCreateInfo(VkApplicationInfo *applicationInfo) const noexcept {
    VkInstanceCreateInfo instanceCreateInfo = {};

    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = nullptr;
    instanceCreateInfo.pApplicationInfo = applicationInfo;
    instanceCreateInfo.enabledExtensionCount = 0;
    instanceCreateInfo.ppEnabledExtensionNames = nullptr;
    instanceCreateInfo.flags = 0;

    if (this->bIsDebug)
        instanceCreateInfo.enabledLayerCount = 0,
        instanceCreateInfo.ppEnabledLayerNames = nullptr;

    return instanceCreateInfo;
}

Instance::Instance(const utf8 *appName, uint32 appVersion, bool bDebug) {
    this->applicationName = appName;
    this->applicationVersion = appVersion;
    this->bIsDebug = bDebug;
    this->instance = VK_NULL_HANDLE;
}

Result<VkInstance> Instance::getVulkanInstance() const noexcept {
    if (this->instance != VK_NULL_HANDLE)
        return Result<VkInstance>(this->instance);
    else
        return Result<VkInstance>::createError(Error::InstanceNotStartedUp);
}

Result<void> Instance::startup() {
    VkApplicationInfo applicationInfo = this->getApplicationInfo();
    VkInstanceCreateInfo instanceCreateInfo = this->getInstanceCreateInfo(&applicationInfo);

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &this->instance);
    if (result == VK_SUCCESS) {
        std::cout << "Created Device Instance..." << std::endl;
        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(Error::FailedToCreateInstance);
}

void Instance::shutdown() {
    vkDestroyInstance(this->instance, nullptr);
    this->instance = VK_NULL_HANDLE;

    std::cout << "Destroyed Device Instance..." << std::endl;
}