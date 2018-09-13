/**
 * Renderer.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Buffer.h"
#include "Device.h"
#include "GraphicsManager.h"
#include "Image.h"
#include "Material.h"
#include "Renderer.h"
#include "SpriteComponent.h"
#include "Queue.h"
#include "Texture.h"
#include "Window.h"
#include "WindowManager.h"

#include <iostream>
#include <vulkan/vulkan.h>

Result<void> Renderer::acquireSwapchainAndBuffers() {
    WindowManager &windowManager = WindowManager::getManager();
    Result<std::shared_ptr<Window>> result = windowManager.getWindow();

    if (!result.hasError()) {
        auto window = static_cast<std::shared_ptr<Window>>(result);

        Result<VkSwapchainKHR> rslt = window->getSwapchain();
        if (rslt.hasError()) {
            return Result<void>::createError(rslt.getError());
        }
        else {
            this->swapchain = static_cast<VkSwapchainKHR>(rslt);
        }

        Result<std::vector<VkImage>> res = window->getImageBuffers();
        if (res.hasError()) {
            return Result<void>::createError(res.getError());
        }
        else {
            auto images = static_cast<std::vector<VkImage>>(res);
            Result<VkDevice> deviceResult = this->getGraphicsDevice();

            if (!deviceResult.hasError()) {
                auto device = static_cast<VkDevice>(deviceResult);

                this->imageBuffers.resize(images.size());
                for (uint32 i = 0; i < static_cast<uint32>(images.size()); i++) {
                    VkImageViewCreateInfo imageViewCreateInfo = {};

                    // Configure Image View
                    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    imageViewCreateInfo.pNext = nullptr;
                    imageViewCreateInfo.flags = 0;
                    imageViewCreateInfo.image = images[i];
                    imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
                    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
                    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
                    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
                    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
                    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
                    imageViewCreateInfo.subresourceRange.levelCount = 1;
                    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                    imageViewCreateInfo.subresourceRange.layerCount = 1;
                    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

                    if (vkCreateImageView(device,
                            &imageViewCreateInfo,
                            nullptr,
                            &this->imageBuffers[i]) != VK_SUCCESS) {
                        return Result<void>::createError(Error::FailedToCreateImageView);
                    }
                }
            }
            else {
                return Result<void>::createError(deviceResult.getError());
            }
        }

        this->width = window->getWidth();
        this->height = window->getHeight();

        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::allocateDescriptorSets() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = this->getDescriptorSetAllocateInfo();

        this->descriptorSets.resize(1);
        if (vkAllocateDescriptorSets(device,
                &descriptorSetAllocateInfo,
                this->descriptorSets.data()) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToAllocateDescriptorSets);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createDescriptorLayouts() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        std::vector<VkDescriptorSetLayoutBinding> bindings = this->getDescriptorSetLayoutBindings();
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo =
                this->getDescriptorSetLayoutCreateInfo(&bindings);

        if (vkCreateDescriptorSetLayout(device,
                                        &descriptorSetLayoutCreateInfo,
                                        nullptr,
                                        &this->descriptorLayout) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateDescriptorSetLayout);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createDescriptorPool() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        std::vector<VkDescriptorPoolSize> poolSize = this->getDescriptorPoolSize();
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
                this->getDescriptorPoolCreateInfo(&poolSize);

        if (vkCreateDescriptorPool(device,
                                   &descriptorPoolCreateInfo,
                                   nullptr,
                                   &this->descriptorPool) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateDescriptorPool);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createFences() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkFenceCreateInfo fenceCreateInfo = this->getFenceCreateInfo();

        if (vkCreateFence(device,
                          &fenceCreateInfo,
                          nullptr,
                          &this->imageFence) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateFence);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createFramebuffers() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);

        this->framebuffers.resize(this->imageBuffers.size());
        for (uint32 i = 0; i < static_cast<uint32>(this->imageBuffers.size()); i++) {
            VkFramebufferCreateInfo framebufferCreateInfo = this->getFramebufferCreateInfo(i);

            if (vkCreateFramebuffer(device,
                                    &framebufferCreateInfo,
                                    nullptr,
                                    &this->framebuffers[i]) != VK_SUCCESS) {
                return Result<void>::createError(Error::FailedToCreateFramebuffer);
            }
        }

        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createMaterial() {
    Result<std::shared_ptr<Material>> result = Material::createMaterial("Shaders/vert.spv",
                                                                        "Shaders/frag.spv");

    if (!result.hasError()) {
        this->material = static_cast<std::shared_ptr<Material>>(result);
        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createPipelineLayouts() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = this->getPipelineLayoutCreateInfo();

        if (vkCreatePipelineLayout(device,
                                   &pipelineLayoutCreateInfo,
                                   nullptr,
                                   &this->pipelineLayout) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreatePipelineLayout);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createPipeline() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkPipelineColorBlendAttachmentState attachmentState = this->getColorBlendAttachmentState();
        VkViewport viewport = this->getViewport();
        VkRect2D rect = this->getRect2D();
        VkVertexInputBindingDescription bindings = Vertex::getBindingDescription();
        std::vector<VkVertexInputAttributeDescription> attributes = Vertex::getAttributeDescription();
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = this->getShaderStageCreateInfo();
        VkPipelineVertexInputStateCreateInfo vertexInputState = this->getVertexInputStateCreateInfo(&bindings,
                                                                                                    &attributes);
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = this->getInputAssemblyStateCreateInfo();
        VkPipelineViewportStateCreateInfo viewportState = this->getViewportStateCreateInfo(&viewport,
                                                                                           &rect);
        VkPipelineRasterizationStateCreateInfo rasterizationState = this->getRasterizationStateCreateInfo();
        VkPipelineMultisampleStateCreateInfo multisampleState = this->getMultisampleStateCreateInfo();
        VkPipelineColorBlendStateCreateInfo colorBlendState = this->getColorBlendStateCreateInfo(&attachmentState);
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo
                = this->getGraphicsPipelineCreateInfo(&shaderStages,
                                                      &vertexInputState,
                                                      &inputAssemblyState,
                                                      &viewportState,
                                                      &rasterizationState,
                                                      &multisampleState,
                                                      &colorBlendState);

        if (vkCreateGraphicsPipelines(device,
                                      VK_NULL_HANDLE,
                                      1,
                                      &graphicsPipelineCreateInfo,
                                      nullptr,
                                      &this->pipeline) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateGraphicsPipeline);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createRenderPass() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkAttachmentReference reference = this->getAttachmentReference();
        std::vector<VkAttachmentDescription> attachments = this->getAttachmentDescription();
        std::vector<VkSubpassDescription> subpasses = this->getSubpassDescription(&reference);
        std::vector<VkSubpassDependency> dependencies = this->getSubpassDependency();
        VkRenderPassCreateInfo renderPassCreateInfo = this->getRenderPassCreateInfo(&attachments,
                                                                                    &subpasses,
                                                                                    &dependencies);

        if (vkCreateRenderPass(device,
                               &renderPassCreateInfo,
                               nullptr,
                               &this->renderPass) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateRenderpass);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createSemaphores() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkSemaphoreCreateInfo semaphoreCreateInfo = this->getSemaphoreCreateInfo();

        // Create Image Buffer Semaphore
        if (vkCreateSemaphore(device,
                              &semaphoreCreateInfo,
                              nullptr,
                              &this->imageSemaphore) != VK_SUCCESS) {
            return Result<void>::createError(Error::FailedToCreateSemaphore);
        }

        // Create Queue Semaphores
        this->queueSemaphores.resize(this->deviceQueues.size() - 1);
        for (uint32 i = 0; i < static_cast<uint32>(this->deviceQueues.size() - 1); i++) {
            if (vkCreateSemaphore(device,
                                  &semaphoreCreateInfo,
                                  nullptr,
                                  &this->queueSemaphores[i]) != VK_SUCCESS) {
                return Result<void>::createError(Error::FailedToCreateSemaphore);
            }
        }

        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createTextureSampler() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        VkSamplerCreateInfo samplerCreateInfo = this->getSamplerCreateInfo();

        if (vkCreateSampler(device,
                            &samplerCreateInfo,
                            nullptr,
                            &this->textureSampler) == VK_SUCCESS) {
            return Result<void>::createError(Error::None);
        }
        else {
            return Result<void>::createError(Error::FailedToCreateSampler);
        }
    }

    return Result<void>::createError(result.getError());
}

Result<void> Renderer::createTransformBuffer() {
    Result<std::shared_ptr<Buffer>> bufferResult = Buffer::createBuffer(sizeof(Transform),
                                                                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    if (bufferResult.hasError()) {
        return Result<void>::createError(bufferResult.getError());
    }

    this->transformBuffer = static_cast<std::shared_ptr<Buffer>>(bufferResult);
    return Result<void>::createError(Error::None);
}

std::vector<VkAttachmentDescription> Renderer::getAttachmentDescription() const noexcept {
    std::vector<VkAttachmentDescription> attachments (1);

    // Configure Attachment
    attachments[0].flags = 0;
    attachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    return attachments;
}

VkAttachmentReference Renderer::getAttachmentReference() const noexcept {
    VkAttachmentReference attachmentReference = {};

    attachmentReference.attachment = 0;
    attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    return attachmentReference;
}

VkPipelineColorBlendAttachmentState Renderer::getColorBlendAttachmentState() const noexcept {
    VkPipelineColorBlendAttachmentState attachmentState = {};

    attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    attachmentState.blendEnable = VK_TRUE;
    attachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    attachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    attachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    attachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    attachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    return attachmentState;
}

VkPipelineColorBlendStateCreateInfo Renderer::getColorBlendStateCreateInfo(
        VkPipelineColorBlendAttachmentState *attachmentState) const noexcept {
    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};

    pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineColorBlendStateCreateInfo.pNext = nullptr;
    pipelineColorBlendStateCreateInfo.flags = 0;
    pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    pipelineColorBlendStateCreateInfo.attachmentCount = 1;
    pipelineColorBlendStateCreateInfo.pAttachments = attachmentState;
    pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    return pipelineColorBlendStateCreateInfo;
}

VkDescriptorSetAllocateInfo Renderer::getDescriptorSetAllocateInfo() const noexcept {
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};

    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = this->descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &this->descriptorLayout;

    return descriptorSetAllocateInfo;
}

std::vector<VkDescriptorSetLayoutBinding> Renderer::getDescriptorSetLayoutBindings() const noexcept {
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings (2);

    // Configure Transform Bindings
    descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorSetLayoutBindings[0].pImmutableSamplers = nullptr;

    // Configure Texture Bindings
    descriptorSetLayoutBindings[1].binding = 1;
    descriptorSetLayoutBindings[1].descriptorCount = 1;
    descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptorSetLayoutBindings[1].pImmutableSamplers = nullptr;

    return descriptorSetLayoutBindings;
}

VkDescriptorPoolCreateInfo Renderer::getDescriptorPoolCreateInfo(
        std::vector<VkDescriptorPoolSize> *poolSize) const noexcept {
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};

    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = 0;
    descriptorPoolCreateInfo.maxSets = 2;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(poolSize->size());
    descriptorPoolCreateInfo.pPoolSizes = poolSize->data();

    return descriptorPoolCreateInfo;
}

std::vector<VkDescriptorPoolSize> Renderer::getDescriptorPoolSize() const noexcept {
    std::vector<VkDescriptorPoolSize> descriptorPoolSize (2);

    // Configure Transform Size
    descriptorPoolSize[0].descriptorCount = 1;
    descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    // Configure Texture Size
    descriptorPoolSize[1].descriptorCount = 1;
    descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    return descriptorPoolSize;
}

VkDescriptorSetLayoutCreateInfo Renderer::getDescriptorSetLayoutCreateInfo(
        std::vector<VkDescriptorSetLayoutBinding> *bindings) const noexcept {
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};

    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.flags = 0;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(bindings->size());
    descriptorSetLayoutCreateInfo.pBindings = bindings->data();

    return descriptorSetLayoutCreateInfo;
}

VkFenceCreateInfo Renderer::getFenceCreateInfo() const noexcept {
    VkFenceCreateInfo fenceCreateInfo = {};

    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = 0;

    return fenceCreateInfo;
}

VkFramebufferCreateInfo Renderer::getFramebufferCreateInfo(
        uint32 imageIndex) const noexcept {
    VkFramebufferCreateInfo framebufferCreateInfo = {};

    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext = 0;
    framebufferCreateInfo.flags = 0;
    framebufferCreateInfo.renderPass = this->renderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = &this->imageBuffers[imageIndex];
    framebufferCreateInfo.width = this->width;
    framebufferCreateInfo.height = this->height;
    framebufferCreateInfo.layers = 1;

    return framebufferCreateInfo;
}

VkGraphicsPipelineCreateInfo Renderer::getGraphicsPipelineCreateInfo(
        std::vector<VkPipelineShaderStageCreateInfo> *shaderStages,
        VkPipelineVertexInputStateCreateInfo *vertexInputState,
        VkPipelineInputAssemblyStateCreateInfo *inputAssemblyState,
        VkPipelineViewportStateCreateInfo *viewportState,
        VkPipelineRasterizationStateCreateInfo *rasterizationState,
        VkPipelineMultisampleStateCreateInfo *multisampleState,
        VkPipelineColorBlendStateCreateInfo *colorBlendState) const noexcept {
    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};

    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.pNext = nullptr;
    graphicsPipelineCreateInfo.flags = 0;
    graphicsPipelineCreateInfo.stageCount = static_cast<uint32>(shaderStages->size());
    graphicsPipelineCreateInfo.pStages = shaderStages->data();
    graphicsPipelineCreateInfo.pVertexInputState = vertexInputState;
    graphicsPipelineCreateInfo.pInputAssemblyState = inputAssemblyState;
    graphicsPipelineCreateInfo.pTessellationState = nullptr;
    graphicsPipelineCreateInfo.pViewportState = viewportState;
    graphicsPipelineCreateInfo.pRasterizationState = rasterizationState;
    graphicsPipelineCreateInfo.pMultisampleState = multisampleState;
    graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
    graphicsPipelineCreateInfo.pColorBlendState = colorBlendState;
    graphicsPipelineCreateInfo.pDynamicState = nullptr;
    graphicsPipelineCreateInfo.layout = this->pipelineLayout;
    graphicsPipelineCreateInfo.renderPass = this->renderPass;
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = 0;

    return graphicsPipelineCreateInfo;
}

VkPipelineInputAssemblyStateCreateInfo Renderer::getInputAssemblyStateCreateInfo() const noexcept {
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};

    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.pNext = nullptr;
    pipelineInputAssemblyStateCreateInfo.flags = 0;
    pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    return pipelineInputAssemblyStateCreateInfo;
}

VkPipelineMultisampleStateCreateInfo Renderer::getMultisampleStateCreateInfo() const noexcept {
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};

    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.pNext = nullptr;
    multisampleStateCreateInfo.flags = 0;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.minSampleShading = 1.0f;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    return multisampleStateCreateInfo;
}

VkPipelineLayoutCreateInfo Renderer::getPipelineLayoutCreateInfo() const noexcept {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};

    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &this->descriptorLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

    return pipelineLayoutCreateInfo;
}

VkPresentInfoKHR Renderer::getPresentInfoKHR() const noexcept {
    VkPresentInfoKHR presentInfoKHR = {};

    presentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfoKHR.pNext = nullptr;
    presentInfoKHR.waitSemaphoreCount = 1;
    presentInfoKHR.pWaitSemaphores = &this->queueSemaphores[0];
    presentInfoKHR.swapchainCount = 1;
    presentInfoKHR.pSwapchains = &this->swapchain;
    presentInfoKHR.pImageIndices = &this->imageIndex;
    presentInfoKHR.pResults = nullptr;

    return presentInfoKHR;
}

VkPipelineRasterizationStateCreateInfo Renderer::getRasterizationStateCreateInfo() const noexcept {
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};

    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext = nullptr;
    rasterizationStateCreateInfo.flags = 0;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationStateCreateInfo.lineWidth = 1.0f;

    return rasterizationStateCreateInfo;
}

VkRect2D Renderer::getRect2D() const noexcept {
    VkRect2D rect = {};

    rect.offset.x = 0;
    rect.offset.y = 0;
    rect.extent.width = this->width;
    rect.extent.height = this->height;

    return rect;
}

VkRenderPassBeginInfo Renderer::getRenderPassBeginInfo(VkClearValue *clearColor) const noexcept {
    VkRenderPassBeginInfo renderPassBeginInfo = {};

    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = this->renderPass;
    renderPassBeginInfo.framebuffer = this->framebuffers[this->imageIndex];
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = this->width;
    renderPassBeginInfo.renderArea.extent.height = this->height;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = clearColor;

    return renderPassBeginInfo;
}

VkRenderPassCreateInfo Renderer::getRenderPassCreateInfo(
        std::vector<VkAttachmentDescription> *attachments,
        std::vector<VkSubpassDescription> *subpasses,
        std::vector<VkSubpassDependency> *dependencies) const noexcept {
    VkRenderPassCreateInfo renderPassCreateInfo = {};

    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = static_cast<uint32>(attachments->size());
    renderPassCreateInfo.pAttachments = attachments->data();
    renderPassCreateInfo.subpassCount = static_cast<uint32>(subpasses->size());
    renderPassCreateInfo.pSubpasses = subpasses->data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32>(dependencies->size());
    renderPassCreateInfo.pDependencies = dependencies->data();

    return renderPassCreateInfo;
}

VkSamplerCreateInfo Renderer::getSamplerCreateInfo() const noexcept {
    VkSamplerCreateInfo samplerCreateInfo = {};

    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.pNext = nullptr;
    samplerCreateInfo.flags = 0;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.anisotropyEnable = VK_FALSE;
    samplerCreateInfo.maxAnisotropy = 0.0f;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = 1.0f;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

    return samplerCreateInfo;
}

VkSemaphoreCreateInfo Renderer::getSemaphoreCreateInfo() const noexcept {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};

    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    return semaphoreCreateInfo;
}

std::vector<VkPipelineShaderStageCreateInfo> Renderer::getShaderStageCreateInfo() const noexcept {
    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfo (2);

    // Configure Vertex Shader
    pipelineShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo[0].pNext = nullptr;
    pipelineShaderStageCreateInfo[0].flags = 0;
    pipelineShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    pipelineShaderStageCreateInfo[0].module = this->material->getVertexModule();
    pipelineShaderStageCreateInfo[0].pName = "main";
    pipelineShaderStageCreateInfo[0].pSpecializationInfo = nullptr;

    // Configure Fragment Shader
    pipelineShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineShaderStageCreateInfo[1].pNext = nullptr;
    pipelineShaderStageCreateInfo[1].flags = 0;
    pipelineShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pipelineShaderStageCreateInfo[1].module = this->material->getFragmentModule();
    pipelineShaderStageCreateInfo[1].pName = "main";
    pipelineShaderStageCreateInfo[1].pSpecializationInfo = nullptr;

    return pipelineShaderStageCreateInfo;
}

std::vector<VkSubpassDependency> Renderer::getSubpassDependency() const noexcept {
    std::vector<VkSubpassDependency> dependencies (1);

    // Configure Dependency
    dependencies[0].dependencyFlags = 0;
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = 0;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    return dependencies;
}

std::vector<VkSubpassDescription> Renderer::getSubpassDescription(
        VkAttachmentReference *attachmentReference) const noexcept {
    std::vector<VkSubpassDescription> subpasses (1);

    // Configure Subpass
    subpasses[0].flags = 0;
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].inputAttachmentCount = 0;
    subpasses[0].pInputAttachments = nullptr;
    subpasses[0].colorAttachmentCount = 1;
    subpasses[0].pColorAttachments = attachmentReference;
    subpasses[0].pResolveAttachments = nullptr;
    subpasses[0].pDepthStencilAttachment = nullptr;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;

    return subpasses;
}

Result<VkDevice> Renderer::getGraphicsDevice() const noexcept {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    Result<std::weak_ptr<const Device>> result = graphicsManager.getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<std::weak_ptr<const Device>>(result);

        if (std::shared_ptr<const Device> dev = device.lock())
            return dev->getVulkanDevice();
        else
            return Result<VkDevice>::createError(Error::GraphicsManagerNotStartedUp);
    }

    return Result<VkDevice>::createError(result.getError());
}

VkPipelineVertexInputStateCreateInfo Renderer::getVertexInputStateCreateInfo(
        VkVertexInputBindingDescription *bindings,
        std::vector<VkVertexInputAttributeDescription> *attributes) const noexcept {
    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};

    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.pNext = nullptr;
    pipelineVertexInputStateCreateInfo.flags = 0;
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = bindings;
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(attributes->size());
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = attributes->data();

    return pipelineVertexInputStateCreateInfo;
}

VkViewport Renderer::getViewport() const noexcept {
    VkViewport viewport = {};

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(this->width);
    viewport.height = static_cast<float>(this->height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    return viewport;
}

VkPipelineViewportStateCreateInfo Renderer::getViewportStateCreateInfo(VkViewport *viewport,
                                                                       VkRect2D *rect) const noexcept {
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};

    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext = nullptr;
    viewportStateCreateInfo.flags = 0;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = rect;

    return viewportStateCreateInfo;
}

Result<void> Renderer::loadQueues() {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    Result<std::weak_ptr<const Device>> result = graphicsManager.getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<std::weak_ptr<const Device>>(result);

        if (std::shared_ptr<const Device> dev = device.lock()) {
            this->deviceQueues = dev->getDeviceQueues();
            this->transferQueue = this->deviceQueues[this->deviceQueues.size() - 1];

            return Result<void>::createError(Error::None);
        }
    }

    return Result<void>::createError(result.getError());
}

VkCommandBuffer Renderer::selectCommandBuffer() const noexcept {
    Result<VkCommandBuffer> cmdBuffer = this->deviceQueues[0]->getVulkanBuffer();
    return static_cast<VkCommandBuffer>(cmdBuffer);
}

void Renderer::updateDescriptorSets(std::shared_ptr<SpriteComponent> &spriteComponent) {
    Transform matrixTransform = {};
    VkDescriptorBufferInfo descriptorBufferInfo = {};
    VkDescriptorImageInfo descriptorImageInfo = {};
    VkWriteDescriptorSet writeDescriptorSet[2] = {};

    // Configure Transform
    matrixTransform.model = spriteComponent->getModelTransform();
    matrixTransform.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f),
                                         glm::vec3(0.0f, 0.0f, 0.0f),
                                         glm::vec3(0.0f, 1.0f, 0.0f));
    matrixTransform.proj = glm::ortho(-128.0f, 128.0f, -128.0f, 128.0f, -128.0f, 128.0f);

    // Fill Transform Buffer
    this->transformBuffer->fillBuffer(sizeof(matrixTransform), &matrixTransform);

    // Configure Buffer Info
    descriptorBufferInfo.buffer = static_cast<VkBuffer>(this->transformBuffer->getVulkanBuffer());
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = VK_WHOLE_SIZE;

    // Configure Image Info
    descriptorImageInfo.sampler = this->textureSampler;
    descriptorImageInfo.imageView = spriteComponent->getTexture()->getImageView();
    descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Configure Transform Matrix
    writeDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet[0].pNext = nullptr;
    writeDescriptorSet[0].dstSet = this->descriptorSets[0];
    writeDescriptorSet[0].dstBinding = 0;
    writeDescriptorSet[0].dstArrayElement = 0;
    writeDescriptorSet[0].descriptorCount = 1;
    writeDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet[0].pImageInfo = nullptr;
    writeDescriptorSet[0].pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSet[0].pTexelBufferView = nullptr;

    // Configure Texture and Sampler
    writeDescriptorSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet[1].pNext = nullptr;
    writeDescriptorSet[1].dstSet = this->descriptorSets[0];
    writeDescriptorSet[1].dstBinding = 1;
    writeDescriptorSet[1].dstArrayElement = 0;
    writeDescriptorSet[1].descriptorCount = 1;
    writeDescriptorSet[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet[1].pImageInfo = &descriptorImageInfo;
    writeDescriptorSet[1].pBufferInfo = nullptr;
    writeDescriptorSet[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(this->device,
                           2,
                           writeDescriptorSet,
                           0,
                           nullptr);
}

Renderer::Renderer() {
    this->descriptorLayout = VK_NULL_HANDLE;
    this->descriptorPool = VK_NULL_HANDLE;
    this->device = VK_NULL_HANDLE;
    this->imageSemaphore = VK_NULL_HANDLE;
    this->pipelineLayout = VK_NULL_HANDLE;
    this->pipeline = VK_NULL_HANDLE;
    this->renderPass = VK_NULL_HANDLE;
    this->swapchain = VK_NULL_HANDLE;
    this->framebuffers = std::vector<VkFramebuffer>();
    this->deviceQueues = std::vector<std::shared_ptr<Queue>>();
    this->transferQueue = nullptr;
    this->imageIndex = 0;
    this->width = 0;
    this->height = 0;
}

Renderer::~Renderer() {
    if (!this->deviceQueues.empty() || this->transferQueue != nullptr || this->pipeline != VK_NULL_HANDLE ||
        this->pipelineLayout != VK_NULL_HANDLE || this->descriptorLayout != VK_NULL_HANDLE) {
        std::cout << "WARNING: Renderer deleted without being shutdown..." << std::endl;
        this->shutdown();
    }
}

Result<void> Renderer::begin() {
    Result<VkDevice> result = this->getGraphicsDevice();
    VkCommandBuffer cmdBuffer = this->selectCommandBuffer();

    if (!result.hasError()) {
        this->device = static_cast<VkDevice>(result);

        // Acquire Next Image
        if (vkAcquireNextImageKHR(this->device,
                                  this->swapchain,
                                  0,
                                  this->imageSemaphore,
                                  VK_NULL_HANDLE,
                                  &this->imageIndex) != VK_SUCCESS) {
            return Result<void>::createError(Error::FailedToAcquireNextImage);
        }

        // Begin Render Pass
        VkClearValue clearColor = { 0.5f, 0.75f, 0.25f, 1.0f };
        VkRenderPassBeginInfo renderPassBeginInfo = this->getRenderPassBeginInfo(&clearColor);
        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind Pipelines
        this->deviceQueues[0]->bindPipeline(this->pipeline);

        return Result<void>::createError(Error::None);
    }

    return Result<void>::createError(result.getError());
}

void Renderer::draw(std::shared_ptr<SpriteComponent> spriteComponent) {
    VkBuffer vertexBuffers = static_cast<VkBuffer>(spriteComponent->getVertexBuffer()->getVulkanBuffer());
    VkDeviceSize offsets = 0;
    VkCommandBuffer cmdBuffer = this->selectCommandBuffer();

    this->updateDescriptorSets(spriteComponent);

    // Bind Descriptors
    vkCmdBindDescriptorSets(cmdBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            this->pipelineLayout,
                            0,
                            1,
                            &this->descriptorSets[0],
                            0,
                            nullptr);

    // Draw Vertices
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffers, &offsets);
    vkCmdDraw(cmdBuffer, 6, 1, 0, 0);
}

Result<void> Renderer::end() {
    auto queue = static_cast<VkQueue>(this->deviceQueues[0]->getVulkanQueue());
    VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkPresentInfoKHR presentInfoKHR = this->getPresentInfoKHR();
    VkCommandBuffer cmdBuffer = this->selectCommandBuffer();

    // Submit Buffers
    vkCmdEndRenderPass(cmdBuffer);
    this->deviceQueues[0]->submit(&this->queueSemaphores[0], 1, &this->imageSemaphore, 1, &stage, VK_NULL_HANDLE);

    // Reset Buffers
    vkQueueWaitIdle(queue);
    this->deviceQueues[0]->resetBuffers();

    if (vkQueuePresentKHR(queue, &presentInfoKHR) != VK_SUCCESS) {
        return Result<void>::createError(Error::FailedToPresentImage);
    }

    return Result<void>::createError(Error::None);
}

Result<void> Renderer::executeTransferBuffer(VkCommandBuffer cmdBuffer) const noexcept {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        Result<VkCommandPool> rslt = this->transferQueue->getVulkanPool();

        if (!rslt.hasError()) {
            auto cmdPool = static_cast<VkCommandPool>(rslt);
            Result<VkQueue> res = this->transferQueue->getVulkanQueue();

            if (!res.hasError()) {
                auto queue = static_cast<VkQueue>(res);
                vkEndCommandBuffer(cmdBuffer);

                VkSubmitInfo submitInfo = {};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.pNext = nullptr;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &cmdBuffer;
                submitInfo.signalSemaphoreCount = 0;
                submitInfo.pSignalSemaphores = nullptr;
                submitInfo.waitSemaphoreCount = 0;
                submitInfo.pWaitSemaphores = nullptr;
                submitInfo.pWaitDstStageMask = nullptr;

                VkResult submitResult = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
                VkResult waitResult = vkDeviceWaitIdle(device);
                //VkResult waitResult = vkQueueWaitIdle(queue);

                if (submitResult == VK_SUCCESS && waitResult == VK_SUCCESS) {
                    vkFreeCommandBuffers(device, cmdPool, 1, &cmdBuffer);
                    return Result<void>::createError(Error::None);
                }
                else {
                    return Result<void>::createError(Error::FailedToSubmitQueue);
                }
            }
            else {
                return Result<void>::createError(res.getError());
            }
        }
        else {
            return Result<void>::createError(rslt.getError());
        }
    }

    return Result<void>::createError(result.getError());
}

Result<VkCommandBuffer> Renderer::requestTransferBuffer() const noexcept {
    Result<VkCommandPool> result = this->transferQueue->getVulkanPool();
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    if (!result.hasError()) {
        auto commandPool = static_cast<VkCommandPool>(result);
        Result<VkDevice> rslt = this->getGraphicsDevice();

        if (!rslt.hasError()) {
            auto device = static_cast<VkDevice>(rslt);
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};

            commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.pNext = nullptr;
            commandBufferAllocateInfo.commandBufferCount = 1;
            commandBufferAllocateInfo.commandPool = commandPool;
            commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

            VkResult res = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
            if (res == VK_SUCCESS) {
                VkCommandBufferBeginInfo commandBufferBeginInfo = {};

                commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                commandBufferAllocateInfo.pNext = nullptr;
                commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                VkResult rst = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
                if (rst == VK_SUCCESS) {
                    return Result<VkCommandBuffer>(commandBuffer);
                }
                else {
                    return Result<VkCommandBuffer>::createError(Error::FailedToAllocateCommandBuffer);
                }
            }
            else {
                return Result<VkCommandBuffer>::createError(Error::FailedToAllocateCommandBuffer);
            }
        }
    }

    return Result<VkCommandBuffer>::createError(result.getError());
}

Result<void> Renderer::startup() {
    Result<void> loadResult = this->loadQueues();
    if (loadResult.hasError()) {
        return Result<void>::createError(loadResult.getError());
    }

    Result<void> materialResult = this->createMaterial();
    if (materialResult.hasError()) {
        return Result<void>::createError(materialResult.getError());
    }

    Result<void> swapchainAndBuffersResult = this->acquireSwapchainAndBuffers();
    if (swapchainAndBuffersResult.hasError()) {
        return Result<void>::createError(swapchainAndBuffersResult.getError());
    }

    Result<void> descriptorLayoutResult = this->createDescriptorLayouts();
    if (descriptorLayoutResult.hasError()) {
        return Result<void>::createError(descriptorLayoutResult.getError());
    }

    Result<void> descriptorPoolResult = this->createDescriptorPool();
    if (descriptorPoolResult.hasError()) {
        return Result<void>::createError(descriptorPoolResult.getError());
    }

    Result<void> descriptorSetResult = this->allocateDescriptorSets();
    if (descriptorSetResult.hasError()) {
        return Result<void>::createError(descriptorSetResult.getError());
    }

    Result<void> pipelineLayoutResult = this->createPipelineLayouts();
    if (pipelineLayoutResult.hasError()) {
        return Result<void>::createError(pipelineLayoutResult.getError());
    }

    Result<void> renderPassResult = this->createRenderPass();
    if (renderPassResult.hasError()) {
        return Result<void>::createError(renderPassResult.getError());
    }

    Result<void> framebufferResult = this->createFramebuffers();
    if (framebufferResult.hasError()) {
        return Result<void>::createError(framebufferResult.getError());
    }

    Result<void> pipelineResult = this->createPipeline();
    if (pipelineResult.hasError()) {
        return Result<void>::createError(pipelineResult.getError());
    }

    Result<void> samplerResult = this->createTextureSampler();
    if (samplerResult.hasError()) {
        return Result<void>::createError(samplerResult.getError());
    }

    Result<void> transformResult = this->createTransformBuffer();
    if (transformResult.hasError()) {
        return Result<void>::createError(transformResult.getError());
    }

    Result<void> semaphoreResult = this->createSemaphores();
    if (semaphoreResult.hasError()) {
        return Result<void>::createError(semaphoreResult.getError());
    }

    Result<void> fenceResult = this->createFences();
    if (fenceResult.hasError()) {
        return Result<void>::createError(fenceResult.getError());
    }

    return Result<void>::createError(Error::None);
}

void Renderer::shutdown() {
    Result<VkDevice> result = this->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);

        // Finish Work
        vkDeviceWaitIdle(device);

        if (this->imageFence != VK_NULL_HANDLE) {
            vkDestroyFence(device, this->imageFence, nullptr);
            this->imageFence = VK_NULL_HANDLE;
        }

        for (auto &semaphore : this->queueSemaphores) {
            if (semaphore != VK_NULL_HANDLE) {
                vkDestroySemaphore(device, semaphore, nullptr);
                semaphore = VK_NULL_HANDLE;
            }
        }

        if (this->imageSemaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(device, this->imageSemaphore, nullptr);
            this->imageSemaphore = VK_NULL_HANDLE;
        }

        if (this->textureSampler != VK_NULL_HANDLE) {
            vkDestroySampler(device, this->textureSampler, nullptr);
            this->textureSampler = VK_NULL_HANDLE;
        }

        for (auto &frame : this->framebuffers) {
            if (frame != VK_NULL_HANDLE) {
                vkDestroyFramebuffer(device, frame, nullptr);
                frame = VK_NULL_HANDLE;
            }
        }

        if (this->renderPass != VK_NULL_HANDLE) {
            vkDestroyRenderPass(device, this->renderPass, nullptr);
            this->renderPass = VK_NULL_HANDLE;
        }

        for (auto &view : this->imageBuffers) {
            if (view != VK_NULL_HANDLE) {
                vkDestroyImageView(device, view, nullptr);
                view = VK_NULL_HANDLE;
            }
        }

        if (this->pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(device, this->pipeline, nullptr);
            this->pipeline = VK_NULL_HANDLE;
        }

        if (this->pipelineLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(device, this->pipelineLayout, nullptr);
            this->pipelineLayout = VK_NULL_HANDLE;
        }

        if (!this->descriptorSets.empty()) {
            this->descriptorSets.clear();
        }

        if (this->descriptorPool != VK_NULL_HANDLE) {
            vkResetDescriptorPool(device, this->descriptorPool, 0);
            vkDestroyDescriptorPool(device, this->descriptorPool, nullptr);
            this->descriptorPool = VK_NULL_HANDLE;
        }

        if (this->descriptorLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(device, this->descriptorLayout, nullptr);
            this->descriptorLayout = VK_NULL_HANDLE;
        }
    }

    this->material.reset();
    this->transformBuffer.reset();

    this->deviceQueues.clear();
    this->imageBuffers.clear();

    this->device = VK_NULL_HANDLE;
    this->swapchain = VK_NULL_HANDLE;
    this->transferQueue = nullptr;
}
