/**
 * Material.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Device.h"
#include "GraphicsManager.h"
#include "Material.h"

#include <fstream>
#include <vulkan/vulkan.h>

Material::Material() {
    this->fragmentShader = VK_NULL_HANDLE;
    this->vertexShader = VK_NULL_HANDLE;
}

Result<VkDevice> Material::getGraphicsDevice() const noexcept {
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

VkShaderModuleCreateInfo Material::getShaderModuleCreateInfo(std::vector<char> code) const noexcept {
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};

    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pNext = nullptr;
    shaderModuleCreateInfo.flags = 0;
    shaderModuleCreateInfo.codeSize = static_cast<uint32>(code.size());
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32 *>(code.data());

    return shaderModuleCreateInfo;
}

Result<std::vector<char>> Material::readShaderFile(const utf8 *filename) const noexcept {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    std::vector<char> shader;

    if (file.is_open()) {
        auto size = static_cast<uint64>(file.tellg());
        shader.resize(size);

        file.seekg(0);
        file.read(shader.data(), size);
        file.close();

        return Result<std::vector<char>>(shader);
    }

    return Result<std::vector<char>>::createError(Error::FailedToOpenShaderFile);
}

Material::~Material() {
    Result<VkDevice> result = this->getGraphicsDevice();
    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);

        vkDestroyShaderModule(device, this->fragmentShader, nullptr);
        vkDestroyShaderModule(device, this->vertexShader, nullptr);

        this->fragmentShader = VK_NULL_HANDLE;
        this->vertexShader = VK_NULL_HANDLE;
    }
}

Result<std::shared_ptr<Material>> Material::createMaterial(const utf8 *vertexFilename,
                                                           const utf8 *fragmentFilename) {
    std::shared_ptr<Material> material(new Material);
    Result<VkDevice> result = material->getGraphicsDevice();

    if (!result.hasError()) {
        auto device = static_cast<VkDevice>(result);
        Result<std::vector<char>> fragmentResult = material->readShaderFile(fragmentFilename);
        Result<std::vector<char>> vertexResult = material->readShaderFile(vertexFilename);

        if (!vertexResult.hasError()) {
            auto vertexCode = static_cast<std::vector<char>>(vertexResult);
            VkShaderModuleCreateInfo vertexModuleCreateInfo = material->getShaderModuleCreateInfo(vertexCode);

            if (vkCreateShaderModule(device,
                                     &vertexModuleCreateInfo,
                                     nullptr,
                                     &material->vertexShader) != VK_SUCCESS) {
                return Result<std::shared_ptr<Material>>::createError(Error::FailedToCreateVertexShader);
            }
        }
        else {
            return Result<std::shared_ptr<Material>>::createError(vertexResult.getError());
        }

        if (!fragmentResult.hasError()) {
            auto fragmentCode = static_cast<std::vector<char>>(fragmentResult);
            VkShaderModuleCreateInfo fragmentModuleCreateInfo = material->getShaderModuleCreateInfo(fragmentCode);

            if (vkCreateShaderModule(device,
                                     &fragmentModuleCreateInfo,
                                     nullptr,
                                     &material->fragmentShader) != VK_SUCCESS) {
                return Result<std::shared_ptr<Material>>::createError(Error::FailedToCreateFragmentShader);
            }
        }
        else {
            return Result<std::shared_ptr<Material>>::createError(fragmentResult.getError());
        }

        return Result<std::shared_ptr<Material>>(material);
    }

    return Result<std::shared_ptr<Material>>::createError(result.getError());
}
