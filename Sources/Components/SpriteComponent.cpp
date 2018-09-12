/**
 * SpriteComponent.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Buffer.h"
#include "Material.h"
#include "SpriteComponent.h"
#include "Texture.h"

#include <vulkan/vulkan.h>

SpriteComponent::SpriteComponent() {
    this->position = glm::vec2(0.0f, 0.0f);
    this->rotation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    this->scale = glm::vec2(1.0f, 1.0f);
    this->material = nullptr;
    this->texture = nullptr;
    this->vertexBuffer = nullptr;
}

std::vector<Vertex> SpriteComponent::getVertexData() const noexcept {
    std::vector<Vertex> vertexData (6);

    // Configure 1st Vertex
    vertexData[0].position = glm::vec3(-0.5f, -0.5f, 0.0f);
    vertexData[0].color = glm::vec3(1.0f, 1.0f, 1.0f);
    vertexData[0].texCoords = glm::vec2(0.0f, 0.0f);

    // Configure 2nd Vertex
    vertexData[1].position = glm::vec3(0.5f, -0.5f, 0.0f);
    vertexData[1].color = glm::vec3(1.0f, 1.0f, 1.0f);
    vertexData[1].texCoords = glm::vec2(1.0f, 0.0f);

    // Configure 3rd Vertex
    vertexData[2].position = glm::vec3(0.5f, 0.5f, 0.0f);
    vertexData[2].color = glm::vec3(1.0f, 1.0f, 1.0f);
    vertexData[2].texCoords = glm::vec2(1.0f, 1.0f);

    // Configure 4th Vertex
    vertexData[3].position = glm::vec3(0.5f, 0.5f, 0.0f);
    vertexData[3].color = glm::vec3(1.0f, 1.0f, 1.0f);
    vertexData[3].texCoords = glm::vec2(1.0f, 1.0f);

    // Configure 5th Vertex
    vertexData[4].position = glm::vec3(-0.5f, 0.5f, 0.0f);
    vertexData[4].color = glm::vec3(1.0f, 1.0f, 1.0f);
    vertexData[4].texCoords = glm::vec2(0.0f, 1.0f);

    // Configure 6th Vertex
    vertexData[5].position = glm::vec3(-0.5f, -0.5f, 0.0f);
    vertexData[5].color = glm::vec3(1.0f, 1.0f, 1.0f);
    vertexData[5].texCoords = glm::vec2(0.0f, 0.0f);

    return vertexData;
}

Result<void> SpriteComponent::setupVertexBuffer() {
    Result<std::shared_ptr<Buffer>> result = Buffer::createBuffer(6 * sizeof(Vertex),
                                                                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    if (!result.hasError()) {
        this->vertexBuffer = static_cast<std::shared_ptr<Buffer>>(result);
        std::vector<Vertex> vertexData = this->getVertexData();

        Result<void> rslt = this->vertexBuffer->fillBuffer(0,
                                                           static_cast<uint64>(vertexData.size()),
                                                           vertexData.data());
        if (!rslt.hasError()) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(rslt.getError());
        }
    }

    return Result<void>::createError(result.getError());
}

SpriteComponent::~SpriteComponent() {
    this->material.reset();
    this->texture.reset();
    this->vertexBuffer.reset();
}

std::shared_ptr<class Material> SpriteComponent::getMaterial() const noexcept {
    return this->material;
}

glm::mat4 SpriteComponent::getModelTransform() const noexcept {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(this->position.x, this->position.y, 0.0f));
    glm::mat4 rotate = glm::mat4_cast(this->rotation);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(this->scale.x, this->scale.y, 1.0f));

    return translate * rotate * scale;
}

std::shared_ptr<Texture> SpriteComponent::getTexture() const noexcept {
    return this->texture;
}

std::shared_ptr<Buffer> SpriteComponent::getVertexBuffer() const noexcept {
    return this->vertexBuffer;
}

Result<void> SpriteComponent::load() {
    Result<void> vertexResult = this->setupVertexBuffer();
    if (vertexResult.hasError()) {
        return Result<void>::createError(vertexResult.getError());
    }

    Result<void> textureResult = this->texture->load();
    if (textureResult.hasError()) {
        return Result<void>::createError(textureResult.getError());
    }

    return Result<void>::createError(Error::None);
}

Result<std::shared_ptr<SpriteComponent>> SpriteComponent::createSpriteComponent(glm::vec2 pos,
                                                                                glm::quat rot,
                                                                                glm::vec2 sc,
                                                                                const utf8 *textureFilename,
                                                                                const utf8 *vertexFilename,
                                                                                const utf8 *fragmentFilename) {
    std::shared_ptr<SpriteComponent> spriteComponent(new SpriteComponent);
    Result<std::shared_ptr<Texture>> result = Texture::createTextureFromFile(textureFilename);

    spriteComponent->position = pos;
    spriteComponent->rotation = rot;
    spriteComponent->scale = sc;

    if (!result.hasError()) {
        spriteComponent->texture = static_cast<std::shared_ptr<Texture>>(result);
        Result<std::shared_ptr<Material>> rslt = Material::createMaterial(vertexFilename,
                                                                          fragmentFilename);

        if (!rslt.hasError()) {
            spriteComponent->material = static_cast<std::shared_ptr<Material>>(rslt);
            return Result<std::shared_ptr<SpriteComponent>>(spriteComponent);
        }
        else {
            return Result<std::shared_ptr<SpriteComponent>>::createError(rslt.getError());
        }
    }

    return Result<std::shared_ptr<SpriteComponent>>::createError(result.getError());
}
