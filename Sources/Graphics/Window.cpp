/**
 * Window.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "GraphicsManager.h"
#include "Instance.h"
#include "Window.h"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

void Window::shutdown() {
    Result<VkInstance> result = this->getGraphicsInstance();

    if (!result.hasError()) {
        auto instance = static_cast<VkInstance>(result);
        vkDestroySurfaceKHR(instance, this->surface, nullptr);
        this->surface = VK_NULL_HANDLE;
    }

    glfwDestroyWindow(this->window);
    this->window = nullptr;
}

