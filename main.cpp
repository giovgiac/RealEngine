/**
 * main.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Buffer.h"
#include "Device.h"
#include "GraphicsManager.h"
#include "Image.h"
#include "Instance.h"
#include "Material.h"
#include "Memory.h"
#include "MemoryManager.h"
#include "PoolAllocator.h"
#include "Renderer.h"
#include "SpriteComponent.h"
#include "Texture.h"
#include "Window.h"
#include "WindowManager.h"
#include "WorldManager.h"

#include <iostream>
#include <vulkan/vulkan.h>

int main() {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    MemoryManager &memoryManager = MemoryManager::getManager();
    WindowManager &windowManager = WindowManager::getManager();
    WorldManager &worldManager = WorldManager::getManager();

    // Startup Managers
    Result<void> graphicsResult = graphicsManager.startup();
    if (graphicsResult.hasError()) {
        return 1;
    }

    Result<void> memoryResult = memoryManager.startup();
    if (memoryResult.hasError()) {
        return 1;
    }

    Result<void> windowResult = windowManager.startup();
    if (windowResult.hasError()) {
        return 1;
    }

    Result<void> worldResult = worldManager.startup();
    if (worldResult.hasError()) {
        return 1;
    }

    // Test Render Loop
    Result<void> playResult = worldManager.play();
    if (playResult.hasError()) {
        std::cout << "Render Loop Error: " << static_cast<uint32>(playResult.getError()) << std::endl;
        return 1;
    }

    // Shutdown Managers
    worldManager.shutdown();
    windowManager.shutdown();
    memoryManager.shutdown();
    graphicsManager.shutdown();

    return EXIT_SUCCESS;
}
