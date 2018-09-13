/**
 * Renderer.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef RENDERER_H_
#define RENDERER_H_

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

    std::vector<struct VkImageView_T *> imageBuffers;

    uint32 imageIndex;

    std::shared_ptr<class Buffer> transformBuffer;

    struct VkFence_T *imageFence;

    struct VkSemaphore_T *imageSemaphore;

    std::vector<struct VkSemaphore_T *> queueSemaphores;

    struct VkSampler_T *textureSampler;

    struct VkRenderPass_T *renderPass;

    std::vector<struct VkFramebuffer_T *> framebuffers;

    std::shared_ptr<class Material> material;

    uint32 width;

    uint32 height;

private:
    Result<void> acquireSwapchainAndBuffers();

    Result<void> allocateDescriptorSets();

    Result<void> createDescriptorLayouts();

    Result<void> createDescriptorPool();

    Result<void> createFences();

    Result<void> createFramebuffers();

    Result<void> createMaterial();

    Result<void> createPipelineLayouts();

    Result<void> createPipeline();

    Result<void> createRenderPass();

    Result<void> createSemaphores();

    Result<void> createTextureSampler();

    Result<void> createTransformBuffer();

    std::vector<struct VkAttachmentDescription> getAttachmentDescription() const noexcept;

    struct VkAttachmentReference getAttachmentReference() const noexcept;

    struct VkPipelineColorBlendAttachmentState getColorBlendAttachmentState() const noexcept;

    struct VkPipelineColorBlendStateCreateInfo getColorBlendStateCreateInfo(
            struct VkPipelineColorBlendAttachmentState *attachmentState) const noexcept;

    struct VkDescriptorSetAllocateInfo getDescriptorSetAllocateInfo() const noexcept;

    std::vector<struct VkDescriptorSetLayoutBinding> getDescriptorSetLayoutBindings() const noexcept;

    struct VkDescriptorPoolCreateInfo getDescriptorPoolCreateInfo(
            std::vector<struct VkDescriptorPoolSize> *poolSize) const noexcept;

    std::vector<struct VkDescriptorPoolSize> getDescriptorPoolSize() const noexcept;

    struct VkDescriptorSetLayoutCreateInfo getDescriptorSetLayoutCreateInfo(
            std::vector<struct VkDescriptorSetLayoutBinding> *bindings) const noexcept;

    struct VkFenceCreateInfo getFenceCreateInfo() const noexcept;

    struct VkFramebufferCreateInfo getFramebufferCreateInfo(
            uint32 imageIndex) const noexcept;

    struct VkGraphicsPipelineCreateInfo getGraphicsPipelineCreateInfo(
            std::vector<struct VkPipelineShaderStageCreateInfo> *shaderStages,
            struct VkPipelineVertexInputStateCreateInfo *vertexInputState,
            struct VkPipelineInputAssemblyStateCreateInfo *inputAssemblyState,
            struct VkPipelineViewportStateCreateInfo *viewportState,
            struct VkPipelineRasterizationStateCreateInfo *rasterizationState,
            struct VkPipelineMultisampleStateCreateInfo *multisampleState,
            struct VkPipelineColorBlendStateCreateInfo *colorBlendState
    ) const noexcept;

    struct VkPipelineInputAssemblyStateCreateInfo getInputAssemblyStateCreateInfo() const noexcept;

    struct VkPipelineMultisampleStateCreateInfo getMultisampleStateCreateInfo() const noexcept;

    struct VkPipelineLayoutCreateInfo getPipelineLayoutCreateInfo() const noexcept;

    struct VkPresentInfoKHR getPresentInfoKHR() const noexcept;

    struct VkPipelineRasterizationStateCreateInfo getRasterizationStateCreateInfo() const noexcept;

    struct VkRect2D getRect2D() const noexcept;

    struct VkRenderPassBeginInfo getRenderPassBeginInfo(
            union VkClearValue *clearColor) const noexcept;

    struct VkRenderPassCreateInfo getRenderPassCreateInfo(
            std::vector<struct VkAttachmentDescription> *attachments,
            std::vector<struct VkSubpassDescription> *subpasses,
            std::vector<struct VkSubpassDependency> *dependencies) const noexcept;

    struct VkSamplerCreateInfo getSamplerCreateInfo() const noexcept;

    struct VkSemaphoreCreateInfo getSemaphoreCreateInfo() const noexcept;

    std::vector<struct VkPipelineShaderStageCreateInfo> getShaderStageCreateInfo() const noexcept;

    std::vector<struct VkSubpassDependency> getSubpassDependency() const noexcept;

    std::vector<struct VkSubpassDescription> getSubpassDescription(
            struct VkAttachmentReference *attachmentReference) const noexcept;

    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

    struct VkPipelineVertexInputStateCreateInfo getVertexInputStateCreateInfo(
            struct VkVertexInputBindingDescription *bindings,
            std::vector<struct VkVertexInputAttributeDescription> *attributes) const noexcept;

    struct VkViewport getViewport() const noexcept;

    struct VkPipelineViewportStateCreateInfo getViewportStateCreateInfo(
            struct VkViewport *viewport,
            struct VkRect2D *rect) const noexcept;

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
