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

        // Create Textures
        std::shared_ptr<Texture> blueTexture = Texture::createTextureFromFile("blue.png").unwrap();
        std::shared_ptr<Texture> gohaTexture = Texture::createTextureFromFile("gohan.png").unwrap();
        std::shared_ptr<Texture> gokuTexture = Texture::createTextureFromFile("goku.png").unwrap();
        std::shared_ptr<Texture> vegeTexture = Texture::createTextureFromFile("vegeta.png").unwrap();

        // Load Textures
        blueTexture->load();
        gohaTexture->load();
        gokuTexture->load();
        vegeTexture->load();

        // Create Sprites
        for (uint32 i = 0; i < 10000; ++i) {
            std::shared_ptr<SpriteComponent> sprite = nullptr;
            glm::vec2 pos = glm::vec2(rand()%25600/100.0f-rand()%25600/100.0f,
                                      rand()%25600/100.0f-rand()%25600/100.0f);
            glm::quat rot = glm::angleAxis(glm::radians(rand()%18000/100.0f),
                                           glm::vec3(0.0f,0.0f,1.0f));
            glm::vec2 scl = glm::vec2(2.0f, 2.0f);

            int r = rand() % 4;
            if (r == 0) {
                sprite = SpriteComponent::createSpriteComponent(pos, rot, scl, blueTexture).unwrap();
            }
            else if (r == 1) {
                sprite = SpriteComponent::createSpriteComponent(pos, rot, scl, gohaTexture).unwrap();
            }
            else if (r == 2) {
                sprite = SpriteComponent::createSpriteComponent(pos, rot, scl, gokuTexture).unwrap();
            }
            else {
                sprite = SpriteComponent::createSpriteComponent(pos, rot, scl, vegeTexture).unwrap();
            }

            components.emplace_front(sprite);
        }

        for (auto &spr : components) {
            worldManager.addObject(spr);
        }
    }

    void update() override {
        for (auto &spr : components) {
            spr->move((rand() % 4) - (rand() % 4), (rand() % 4) - (rand() % 4));
        }
    }

private:
    std::forward_list<std::shared_ptr<SpriteComponent>> components;
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
