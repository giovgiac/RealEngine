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
    CopyBuffer,
    SetBuffer
};

struct CopyBufferInfo {

};

struct SetBufferInfo {
    std::shared_ptr<class Buffer> buffer;
    uint64 offset;
    uint64 size;
    void *data;
};

#endif /* COMMAND_H_ */
