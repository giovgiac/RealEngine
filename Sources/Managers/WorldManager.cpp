/**
 * WorldManager.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Device.h"
#include "GraphicsManager.h"
#include "Renderer.h"
#include "SpriteComponent.h"
#include "Window.h"
#include "WindowManager.h"
#include "WorldManager.h"

#include <iostream>
#include <vulkan/vulkan.h>

WorldManager::WorldManager() {
    this->renderer = nullptr;
}

WorldManager::~WorldManager() {
    this->renderer.reset();
}

Result<VkDevice> WorldManager::getGraphicsDevice() const noexcept {
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

Result<std::shared_ptr<Renderer>> WorldManager::getRenderer() const noexcept {
    if (this->renderer != nullptr) {
        return Result<std::shared_ptr<Renderer>>(this->renderer);
    }
    else {
        return Result<std::shared_ptr<Renderer>>::createError(Error::WorldManagerNotStartedUp);
    }
}

Result<void> WorldManager::play() {
    WindowManager &windowManager = WindowManager::getManager();
    Result<std::shared_ptr<Window>> result = windowManager.getWindow();

    if (!result.hasError()) {
        auto window = static_cast<std::shared_ptr<Window>>(result);

        // Load Test Resources
        std::shared_ptr<SpriteComponent> spriteComponent = nullptr;
        Result<std::shared_ptr<SpriteComponent>> spriteResult
                = SpriteComponent::createSpriteComponent(glm::vec2(0.0f, 0.0f),
                                                         glm::angleAxis(glm::radians(0.0f),
                                                                        glm::vec3(0.0f,
                                                                                  0.0f,
                                                                                  1.0f)),
                                                         glm::vec2(1.0f, 1.0f),
                                                         "goku.png");

        if (!spriteResult.hasError()) {
            spriteComponent = static_cast<std::shared_ptr<SpriteComponent>>(spriteResult);
            spriteComponent->load();
        }

        while (!window->shouldClose()) {
            // Update
            spriteComponent->move(0.0005f, 0.0f);

            // Render Loop
            this->renderer->begin();
            this->renderer->draw(spriteComponent);
            this->renderer->end();


            window->pollEvents();
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> WorldManager::startup() {
    std::cout << "Starting Up WorldManager..." << std::endl;

    this->renderer = std::make_shared<Renderer>();

    Result<void> rendererResult = this->renderer->startup();
    if (rendererResult.hasError()) {
        std::cout << "Failed To Start Up WorldManager - Renderer..." << std::endl;
        return Result<void>::createError(rendererResult.getError());
    }

    return Result<void>::createError(Error::None);
}

void WorldManager::shutdown() {
    if (this->renderer != nullptr) {
        this->renderer->shutdown();
    }

    this->renderer.reset();
    std::cout << "Shutting Down WorldManager..." << std::endl;
}
