/**
 * Material.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Result.h"

class Material {
private:

    struct VkShaderModule_T *fragmentShader;

    struct VkShaderModule_T *vertexShader;

private:
    explicit Material();

    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

    struct VkShaderModuleCreateInfo getShaderModuleCreateInfo(std::vector<char> code) const noexcept;

    Result<std::vector<char>> readShaderFile(const utf8 *filename) const noexcept;

public:
    ~Material();

    static Result<std::shared_ptr<Material>> createMaterial(const utf8 *vertexFilename,
                                                            const utf8 *fragmentFilename);

public:
    Material(const Material &) = delete;
    Material(Material &&) = delete;

    Material &operator=(const Material &) = delete;
    Material &operator=(Material &&) = delete;
};

#endif /* MATERIAL_H_ */
