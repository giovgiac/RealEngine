/**
 * Instance.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Instance.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

const std::vector<const utf8 *> validationExtensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};

const std::vector<const utf8 *> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData) {

    std::cerr << "VALIDATION MESSAGE: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pCallback) {
    auto func =
            (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                       "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT callback,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func =
            (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                        "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

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

    if (this->bIsDebug) {
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32>(validationExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = validationExtensions.data();
        instanceCreateInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }

    return instanceCreateInfo;
}

void Instance::setupDebug() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;

    if (createDebugUtilsMessengerEXT(this->instance, &createInfo, nullptr, &this->callback) != VK_SUCCESS) {

    }
}

Instance::Instance(const utf8 *appName, uint32 appVersion, bool bDebug) {
    this->applicationName = appName;
    this->applicationVersion = appVersion;
    this->bIsDebug = bDebug;
    this->instance = VK_NULL_HANDLE;
}

Instance::~Instance() {
    this->applicationName = nullptr;
    this->applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    this->bIsDebug = false;

    if (this->instance != VK_NULL_HANDLE) {
        std::cout << "WARNING: Instance deleted without being shutdown..." << std::endl;
        this->shutdown();
    }
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
        if (this->bIsDebug) {
            setupDebug();
        }

        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(Error::FailedToCreateInstance);
}

void Instance::shutdown() {
    if (this->bIsDebug) {
        destroyDebugUtilsMessengerEXT(this->instance, this->callback, nullptr);
    }

    vkDestroyInstance(this->instance, nullptr);
    this->instance = VK_NULL_HANDLE;

    std::cout << "Destroyed Device Instance..." << std::endl;
}
