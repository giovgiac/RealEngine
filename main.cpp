/**
 * main.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Device.h"
#include "Instance.h"

#include <iostream>
#include <vulkan/vulkan.hpp>

int main() {
    std::vector<const utf8 *> requiredExtensions = {};
    VkPhysicalDeviceFeatures requiredFeatures = {};
    VkPhysicalDeviceLimits requiredLimits = {};

    auto inst = new Instance("Test Application", VK_MAKE_VERSION(1, 0, 0), false);
    auto dev = new Device(inst, requiredExtensions, requiredFeatures, requiredLimits, false);

    inst->startup();
    dev->startup();

    Result<VkInstance> res = inst->getVulkanInstance();
    if (!res.hasError())
        std::cout << "Instance: " << static_cast<VkInstance>(res) << std::endl;

    Result<VkPhysicalDevice> pd = dev->getVulkanPhysicalDevice();
    if (!pd.hasError())
        std::cout << "Physical Device: " << static_cast<VkPhysicalDevice>(pd) << std::endl;

    Result<VkDevice> d = dev->getVulkanDevice();
    if (!d.hasError())
        std::cout << "Device: " << static_cast<VkDevice>(d) << std::endl;

    dev->shutdown();
    inst->shutdown();

    return EXIT_SUCCESS;
}