/**
 * GraphicsManager.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Device.h"
#include "GraphicsManager.h"
#include "Instance.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

GraphicsManager::GraphicsManager() {
    this->device = nullptr;
    this->instance = nullptr;
}

GraphicsManager::~GraphicsManager() {
    if (this->device != nullptr || this->instance != nullptr)
        std::cout << "WARNING: GraphicsManager deleted without being shutdown..." << std::endl,
        this->shutdown();
}

Result<std::weak_ptr<const Device>> GraphicsManager::getGraphicsDevice() const noexcept {
    if (this->device)
        return Result<std::weak_ptr<const Device>>(this->device);
    else
        return Result<std::weak_ptr<const Device>>::createError(Error::GraphicsManagerNotStartedUp);
}

Result<std::weak_ptr<const Instance>> GraphicsManager::getGraphicsInstance() const noexcept {
    if (this->instance)
        return Result<std::weak_ptr<const Instance>>(this->instance);
    else
        return Result<std::weak_ptr<const class Instance>>::createError(Error::GraphicsManagerNotStartedUp);
}

Result<void> GraphicsManager::startup() {
    std::cout << "Starting Up GraphicsManager..." << std::endl;

    // Gather requirements
    std::vector<const utf8 *> extensions = {};
    VkPhysicalDeviceFeatures features = {};
    VkPhysicalDeviceLimits limits = {};

    // Allocate objects
    instance = std::make_shared<Instance>("Test Application", VK_MAKE_VERSION(1, 0, 0), false);
    device = std::make_shared<Device>(instance, extensions, features, limits, false);

    // Initialize objects
    Result<void> instanceResult = instance->startup();
    if (instanceResult.hasError()) {
        std::cout << "Failed To Start Up GraphicsManager - Instance..." << std::endl;
        return Result<void>::createError(instanceResult.getError());
    }

    Result<void> deviceResult = device->startup();
    if (deviceResult.hasError()) {
        std::cout << "Failed To Start Up GraphicsManager - Device..." << std::endl;
        return Result<void>::createError(deviceResult.getError());
    }

    return Result<void>::createError(Error::None);
}

void GraphicsManager::shutdown() {
    device->shutdown();
    instance->shutdown();

    // Clear objects
    device.reset();
    instance.reset();

    std::cout << "Shutting Down GraphicsManager..." << std::endl;
}
