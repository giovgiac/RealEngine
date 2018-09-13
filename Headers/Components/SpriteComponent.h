/**
 * SpriteComponent.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef SPRITECOMPONENT_H_
#define SPRITECOMPONENT_H_

#include "Result.h"

struct Transform {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoords;

    static struct VkVertexInputBindingDescription getBindingDescription() noexcept;

    static std::vector<struct VkVertexInputAttributeDescription> getAttributeDescription() noexcept;
};

class SpriteComponent {
private:
    glm::vec2 position;

    glm::quat rotation;

    glm::vec2 scale;

    std::shared_ptr<class Buffer> vertexBuffer;

    std::shared_ptr<class Texture> texture;

private:
    explicit SpriteComponent();

    std::vector<Vertex> getVertexData() const noexcept;

    Result<void> setupVertexBuffer();

public:
    virtual ~SpriteComponent();

    glm::mat4 getModelTransform() const noexcept;

    std::shared_ptr<class Texture> getTexture() const noexcept;

    std::shared_ptr<class Buffer> getVertexBuffer() const noexcept;

    void move(float dx, float dy);

    Result<void> load();

    static Result<std::shared_ptr<SpriteComponent>> createSpriteComponent(glm::vec2 pos,
                                                                          glm::quat rot,
                                                                          glm::vec2 sc,
                                                                          const utf8 *textureFilename);

public:
    SpriteComponent(const SpriteComponent &) = delete;
    SpriteComponent(SpriteComponent &&) = delete;

    SpriteComponent &operator=(const SpriteComponent &) = delete;
    SpriteComponent &operator=(SpriteComponent &&) = delete;
};

#endif /* SPRITECOMPONENT_H_ */
