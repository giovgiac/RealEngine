/**
 * Window.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include "Result.h"

class Window {
private:
    struct GLFWwindow *window;

    uint32 width;

    uint32 height;

    const utf8 *title;

    struct VkSurfaceKHR_T *surface;

    struct VkSwapchainKHR_T *swapchain;

private:
    Result<void> createVulkanWindowAndSurface();

    Result<void> createVulkanSwapchain();

    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

    Result<struct VkInstance_T *> getGraphicsInstance() const noexcept;

    struct VkSwapchainCreateInfoKHR getSwapchainCreateInfo() const noexcept;

public:
    explicit Window(uint32 width, uint32 height, const utf8 *title);

    virtual ~Window();

    void pollEvents() const noexcept;

    bool shouldClose() const noexcept;

    Result<void> startup();

    void shutdown();

public:
    Window(const Window &) = delete;
    Window(Window &&) = delete;

    Window &operator=(const Window &) = delete;
    Window &operator=(Window &&) = delete;

};

#endif /* WINDOW_H_ */
