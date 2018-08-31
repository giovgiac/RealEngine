/**
 * Renderer.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "Command.h"
#include "Result.h"

class Renderer final {
private:

    std::vector<std::shared_ptr<class Queue>> deviceQueues;

private:
    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

public:
    explicit Renderer();

    virtual ~Renderer();

    Result<void> flush() const noexcept;

    Result<void> startup();

    void shutdown();

    Result<void> submit(Command cmd, CopyBufferInfo info) const noexcept;
    Result<void> submit(Command cmd, SetBufferInfo info) const noexcept;
};

#endif /* RENDERER_H_ */
