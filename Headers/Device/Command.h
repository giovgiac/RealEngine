/**
 * Command.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include "Types.h"

enum class Command : uint16 {
    ClearImage,
    CopyBuffer,
    SetBuffer
};

struct ClearImageInfo {

};

struct CopyBufferInfo {
    std::shared_ptr<class Buffer> src;
    std::shared_ptr<class Buffer> dst;
    uint32 regionCount;
    const struct VkBufferCopy *regions;
};

struct SetBufferInfo {
    std::shared_ptr<class Buffer> buffer;
    uint64 offset;
    uint64 size;
    void *data;
};

#endif /* COMMAND_H_ */
