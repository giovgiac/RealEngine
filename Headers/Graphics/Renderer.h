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

    std::shared_ptr<class Queue> transferQueue;

private:
    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

public:
    explicit Renderer();

    virtual ~Renderer();

    Result<void> executeTransferBuffer(struct VkCommandBuffer_T *cmdBuffer) const noexcept;

    Result<void> flush() const noexcept;

    Result<struct VkCommandBuffer_T *> requestTransferBuffer() const noexcept;

    Result<void> startup();

    void shutdown();

public:
    Renderer(const Renderer &) = delete;
    Renderer(Renderer &&) = delete;

    Renderer &operator=(const Renderer &) = delete;
    Renderer &operator=(Renderer &&) = delete;
};

#endif /* RENDERER_H_ */
