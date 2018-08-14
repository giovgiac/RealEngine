/**
 * main.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "GraphicsManager.h"
#include "Instance.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

int main() {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();

    // Startup Managers
    Result<void> graphicsResult = graphicsManager.startup();
    if (graphicsResult.hasError())
        return 1;

    Result<std::weak_ptr<const Instance>> res = graphicsManager.getGraphicsInstance();
    if (!res.hasError()) {
        auto instance = static_cast<std::weak_ptr<const Instance>>(res);

        if (std::shared_ptr<const Instance> inst = instance.lock()) {
            Result<VkInstance> vulkanResult = inst->getVulkanInstance();

            if (!vulkanResult.hasError()) {
                auto vulkanInstance = static_cast<VkInstance>(vulkanResult);

                std::cout << "Vulkan Instance: " << vulkanInstance << std::endl;
            }
        }
    }

    // Shutdown Managers
    graphicsManager.shutdown();

    return EXIT_SUCCESS;
}
