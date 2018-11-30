/**
 * main.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Game.h"
#include "Renderer.h"
#include "SpriteComponent.h"
#include "Texture.h"
#include "WorldManager.h"

#include <iostream>
#include <vulkan/vulkan.h>

class MyGame : public Game {
public:
    void begin() override {
        WorldManager &worldManager = WorldManager::getManager();
        std::shared_ptr<Renderer> renderer = worldManager.getRenderer().unwrap();

        // Create Textures
        std::shared_ptr<Texture> gokuTexture = Texture::createTextureFromFile("goku.png").unwrap();

        // Load Textures
        gokuTexture->load();

        // Create Sprites
        std::forward_list<std::shared_ptr<SpriteComponent>> components = {};
        for (uint32 i = 0; i < 1000; ++i) {
            std::shared_ptr<SpriteComponent> sprite = SpriteComponent::createSpriteComponent(
                    glm::vec2(rand() % 25600 / 100.0f - rand() % 25600 / 100.0f,
                            rand() % 25600 / 100.0f - rand() % 25600 / 100.0f),
                    glm::angleAxis(glm::radians(rand() % 18000 / 100.0f),
                            glm::vec3(0.0f,
                                    0.0f,
                                    1.0f)),
                    glm::vec2(2.0f, 2.0f),
                    gokuTexture).unwrap();

            components.emplace_front(sprite);
        }

        for (auto &spr : components) {
            renderer->addObject(spr);
        }

        renderer->load();
    }

    void update() override {

    }
};

int main() {
    MyGame game;

    if (game.startup().hasError()) {
        std::cout << "ERROR: Failed to startup Game..." << std::endl;
        return 1;
    }

    if (game.play().hasError()) {
        std::cout << "ERROR: Failed to play Game..." << std::endl;
        return 1;
    }

    game.shutdown();
    return EXIT_SUCCESS;
}
