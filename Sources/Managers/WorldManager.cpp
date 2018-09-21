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
#include "Texture.h"
#include "Window.h"
#include "WindowManager.h"
#include "WorldManager.h"

#include <iostream>
#include <time.h>
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

    srand(time(NULL));

    if (!result.hasError()) {
        auto window = static_cast<std::shared_ptr<Window>>(result);

        // Load Test Textures
        std::shared_ptr<Texture> gokuTexture = nullptr;
        Result<std::shared_ptr<Texture>> textureResult = Texture::createTextureFromFile("goku.png");

        if (!textureResult.hasError()) {
            gokuTexture = static_cast<std::shared_ptr<Texture>>(textureResult);
            gokuTexture->load();
        }

        // Load Test Resources
        std::forward_list<std::shared_ptr<SpriteComponent>> components = {};
        for (uint32 i = 0; i < 1000; i++) {
            Result<std::shared_ptr<SpriteComponent>> spriteResult
                    = SpriteComponent::createSpriteComponent(
                            glm::vec2(rand() % 25600 / 100.0f - rand() % 25600 / 100.0f,
                                      rand() % 25600 / 100.0f - rand() % 25600 / 100.0f),
                            glm::angleAxis(glm::radians(rand() % 18000 / 100.0f),
                                           glm::vec3(0.0f,
                                                     0.0f,
                                                     1.0f)),
                            glm::vec2(2.0f, 2.0f),
                            gokuTexture);

            if (!spriteResult.hasError()) {
                components.push_front(static_cast<std::shared_ptr<SpriteComponent>>(spriteResult));
            }
        }

        // Add Objects
        for (auto &obj : components) {
            this->renderer->addObject(obj);
        }

        // Load
        Result<void> loadResult = this->renderer->load();
        if (loadResult.hasError()) {
            std::cout << "Error Loading Renderer: " << static_cast<uint32>(loadResult.getError()) << std::endl;
            return Result<void>::createError(loadResult.getError());
        }

        clock_t startTime = clock();
        clock_t currentTime;
        clock_t lastTime = startTime;
        uint32 fps = 0;
        float time = 0.0f;

        while (!window->shouldClose()) {
            currentTime = clock();
            if ((static_cast<double>(currentTime - startTime)/CLOCKS_PER_SEC) >= 1.0) {
                std::cout << "FPS: " << fps << std::endl;
                startTime = currentTime;
                fps = 0;
            }

            // Delta Time
            float deltaTime = static_cast<float>(currentTime - lastTime)/CLOCKS_PER_SEC;
            for (auto &obj : components) {
                obj->setPosition(rand() % 25600 / 100.0f - rand() % 25600 / 100.0f,
                                 rand() % 25600 / 100.0f - rand() % 25600 / 100.0f);
                obj->setRotation(rand() % 18000 / 100.0f);
            }

            // Render Loop
            this->renderer->begin();
            this->renderer->draw();
            this->renderer->end();


            window->pollEvents();
            time += 0.01f;
            fps += 1;
            lastTime = currentTime;
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
