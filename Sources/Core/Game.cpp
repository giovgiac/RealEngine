/**
 * Game.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Game.h"
#include "GraphicsManager.h"
#include "MemoryManager.h"
#include "WindowManager.h"
#include "WorldManager.h"

Result<void> Game::startup() {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    MemoryManager &memoryManager = MemoryManager::getManager();
    WindowManager &windowManager = WindowManager::getManager();
    WorldManager &worldManager = WorldManager::getManager();

    Result<void> graphicsStartupResult = graphicsManager.startup();
    if (graphicsStartupResult.hasError()) {
        return Result<void>::createError(graphicsStartupResult.getError());
    }

    Result<void> memoryStartupResult = memoryManager.startup();
    if (memoryStartupResult.hasError()) {
        return Result<void>::createError(memoryStartupResult.getError());
    }

    Result<void> windowStartupResult = windowManager.startup();
    if (windowStartupResult.hasError()) {
        return Result<void>::createError(windowStartupResult.getError());
    }

    Result<void> worldStartupResult = worldManager.startup();
    if (worldStartupResult.hasError()) {
        return Result<void>::createError(worldStartupResult.getError());
    }

    return Result<void>::createError(Error::None);
}

Result<void> Game::play() {
    WorldManager &worldManager = WorldManager::getManager();

    Result<void> mainLoopResult = worldManager.play(this);
    if (mainLoopResult.hasError()) {
        return Result<void>::createError(mainLoopResult.getError());
    }

    return Result<void>::createError(Error::None);
}

void Game::shutdown() {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    MemoryManager &memoryManager = MemoryManager::getManager();
    WindowManager &windowManager = WindowManager::getManager();
    WorldManager &worldManager = WorldManager::getManager();

    worldManager.shutdown();
    windowManager.shutdown();
    memoryManager.shutdown();
    graphicsManager.shutdown();
}
