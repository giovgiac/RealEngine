/**
 * Texture.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Result.h"

const uint32 TILE_SIZE = 32;

struct RawImageInfo {
    uint32 width;
    uint32 height;
    uint8 *pixels;
};

class Texture {
private:

    std::shared_ptr<class Buffer> buffer;

    std::shared_ptr<class Image> image;

    uint32 width;

    uint32 height;

private:
    explicit Texture();

    struct VkBufferImageCopy getBufferImageCopy() const noexcept;

    Result<std::shared_ptr<const class Renderer>> getRenderer() const noexcept;

    Result<RawImageInfo> loadImage(const utf8 *filename) const noexcept;

public:

    static Result<std::shared_ptr<Texture>> createTextureFromFile(const utf8 *filename);

    Result<std::weak_ptr<class Buffer>> getBuffer() const noexcept;

    Result<void> load();
};

#endif /* TEXTURE_H_ */
