/**
 * Queue.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "Result.h"

/**
 * A classe Queue é uma abstração sobre o objeto VkQueue da API Vulkan. Sua funcionalidade básica é submeter comandos
 * à respectiva fila que representa e, portanto, tais objetos serão usados frequentemente em conjunto com o
 * Escalonador (Renderer) enquanto este decide em qual das filas é apropriado realizar as operações
 * a ele requisitadas.
 *
 * A classe Queue necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class Queue {
protected:
    struct VkCommandBuffer_T *buffer;

    struct VkCommandPool_T *pool;

    struct VkQueue_T *queue;

    uint32 familyIndex;

    uint32 queueIndex;

protected:
    explicit Queue();

    struct VkCommandBufferAllocateInfo getCommandBufferAllocateInfo() const noexcept;

    struct VkCommandPoolCreateInfo getCommandPoolCreateInfo() const noexcept;

public:
    static Result<std::shared_ptr<Queue>> createQueue(struct VkDevice_T *device,
                                                      uint32 familyIndex,
                                                      uint32 queueIndex);

    Result<struct VkCommandBuffer_T *> getVulkanBuffer() const noexcept;

    Result<struct VkCommandPool_T *> getVulkanPool() const noexcept;

    Result<struct VkQueue_T *> getVulkanQueue() const noexcept;

public:
    Queue(const Queue &) = delete;
    Queue(Queue &&) = delete;

    Queue &operator=(const Queue &) = delete;
    Queue &operator=(Queue &&) = delete;
};

#endif /* QUEUE_H_ */
