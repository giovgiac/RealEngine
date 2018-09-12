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

    struct VkImageView_T *view;

private:
    explicit Texture();

    Result<void> createImageView();

    struct VkBufferImageCopy getBufferImageCopy() const noexcept;

    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

    Result<std::shared_ptr<class Renderer>> getRenderer() const noexcept;

    Result<RawImageInfo> loadImage(const utf8 *filename) const noexcept;

public:
    ~Texture();

    static Result<std::shared_ptr<Texture>> createTextureFromFile(const utf8 *filename);

    Result<std::weak_ptr<class Buffer>> getBuffer() const noexcept;

    struct VkImageView_T *getImageView() const noexcept;

    Result<void> load();
};

#endif /* TEXTURE_H_ */
