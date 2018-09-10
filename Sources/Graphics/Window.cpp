/**
 * Window.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "GraphicsManager.h"
#include "Device.h"
#include "Instance.h"
#include "Window.h"

#include <iostream>
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

Result<void> Window::createVulkanWindowAndSurface() {
    Result<VkInstance> result = this->getGraphicsInstance();

    if (!result.hasError()) {
        auto instance = static_cast<VkInstance>(result);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        this->window = glfwCreateWindow(this->width,
                                        this->height,
                                        this->title,
                                        nullptr,
                                        nullptr);

        VkResult rslt = glfwCreateWindowSurface(instance, this->window, nullptr, &this->surface);
        if (rslt == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateSurface);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Window::createVulkanSwapchain() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkSwapchainCreateInfoKHR swapchainCreateInfoKHR = this->getSwapchainCreateInfo();

        VkResult rslt = vkCreateSwapchainKHR(device,
                                             &swapchainCreateInfoKHR,
                                             nullptr,
                                             &this->swapchain);
        if (rslt == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateSwapchain);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<VkDevice> Window::getGraphicsDevice() const noexcept {
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

Result<VkInstance> Window::getGraphicsInstance() const noexcept {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    Result<std::weak_ptr<const Instance>> result = graphicsManager.getGraphicsInstance();

    if (!result.hasError()) {
        auto device = static_cast<std::weak_ptr<const Instance>>(result);

        if (std::shared_ptr<const Instance> dev = device.lock())
            return dev->getVulkanInstance();
        else
            return Result<VkInstance>::createError(Error::GraphicsManagerNotStartedUp);
    }

    return Result<VkInstance>::createError(result.getError());
}

VkSwapchainCreateInfoKHR Window::getSwapchainCreateInfo() const noexcept {
    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};

    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.pNext = nullptr;
    swapChainCreateInfo.surface = this->surface;
    swapChainCreateInfo.minImageCount = 2;
    swapChainCreateInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    swapChainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapChainCreateInfo.imageExtent = { this->width, this->height };
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
    swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    swapChainCreateInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = this->swapchain;

    return swapChainCreateInfo;
}

Window::Window(uint32 width, uint32 height, const utf8 *title) {
    this->width = width;
    this->height = height;
    this->title = title;
}

Window::~Window() {
    if (this->window) {
        std::cout << "WARNING: Window destructor called before shutdown..." << std::endl;
        this->shutdown();
    }
}

void Window::pollEvents() const noexcept {
    glfwPollEvents();
}

bool Window::shouldClose() const noexcept {
    return static_cast<bool>(glfwWindowShouldClose(this->window));
}

Result<void> Window::startup() {
    Result<void> surfaceResult = this->createVulkanWindowAndSurface();
    if (surfaceResult.hasError()) {
        return Result<void>::createError(surfaceResult.getError());
    }

    Result<void> swapchainResult = this->createVulkanSwapchain();
    if (swapchainResult.hasError()) {
        return Result<void>::createError(swapchainResult.getError());
    }

    return Result<void>::createError(Error::None);
}

void Window::shutdown() {
    Result<VkDevice> deviceResult = this->getGraphicsDevice();
    if (!deviceResult.hasError()) {
        auto device = static_cast<VkDevice>(deviceResult);

        vkDestroySwapchainKHR(device, this->swapchain, nullptr);
        this->swapchain = VK_NULL_HANDLE;
    }

    Result<VkInstance> instanceResult = this->getGraphicsInstance();
    if (!instanceResult.hasError()) {
        auto instance = static_cast<VkInstance>(instanceResult);

        vkDestroySurfaceKHR(instance, this->surface, nullptr);
        this->surface = VK_NULL_HANDLE;
    }

    glfwDestroyWindow(this->window);
    this->window = nullptr;
}
