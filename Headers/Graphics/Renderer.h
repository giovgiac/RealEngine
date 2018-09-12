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

    struct VkDescriptorPool_T *descriptorPool;

    struct VkDescriptorSetLayout_T *descriptorLayout;

    std::vector<struct VkDescriptorSet_T *> descriptorSets;

    struct VkPipelineLayout_T *pipelineLayout;

    struct VkPipeline_T *pipeline;

    struct VkDevice_T *device;

    struct VkSwapchainKHR_T *swapchain;

    std::vector<struct VkImage_T *> imageBuffers;

    std::shared_ptr<class Buffer> transformBuffer;

    struct VkSemaphore_T *imageSemaphore;

    std::vector<struct VkSemaphore_T *> queueSemaphores;

    uint32 imageIndex;

private:
    Result<void> acquireSwapchainAndBuffers();

    Result<void> allocateDescriptorSets();

    Result<void> createDescriptorLayouts();

    Result<void> createDescriptorPool();

    Result<void> createPipelineLayouts();

    Result<void> createPipelines();

    Result<void> createSemaphores();

    Result<void> createTransformBuffer();

    struct VkDescriptorSetAllocateInfo getDescriptorSetAllocateInfo() const noexcept;

    std::vector<struct VkDescriptorSetLayoutBinding> getDescriptorSetLayoutBindings() const noexcept;

    struct VkDescriptorPoolCreateInfo getDescriptorPoolCreateInfo(
            std::vector<struct VkDescriptorPoolSize> *poolSize) const noexcept;

    std::vector<struct VkDescriptorPoolSize> getDescriptorPoolSize() const noexcept;

    struct VkDescriptorSetLayoutCreateInfo getDescriptorSetLayoutCreateInfo(
            std::vector<struct VkDescriptorSetLayoutBinding> *bindings) const noexcept;

    struct VkPipelineLayoutCreateInfo getPipelineLayoutCreateInfo() const noexcept;

    struct VkPresentInfoKHR getPresentInfoKHR() const noexcept;

    struct VkSemaphoreCreateInfo getSemaphoreCreateInfo() const noexcept;

    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

    Result<void> loadQueues();

    struct VkCommandBuffer_T *selectCommandBuffer() const noexcept;

    void updateDescriptorSets(std::shared_ptr<class SpriteComponent> &spriteComponent);

public:
    explicit Renderer();

    virtual ~Renderer();

    Result<void> begin();

    void draw(std::shared_ptr<class SpriteComponent> spriteComponent);

    Result<void> end();

    Result<void> executeTransferBuffer(struct VkCommandBuffer_T *cmdBuffer) const noexcept;

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
