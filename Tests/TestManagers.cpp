/**
 * TestManagers.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "GraphicsManager.h"
#include "MemoryManager.h"

#include <gtest/gtest.h>

TEST(RealEngineTestManagers, TestGraphicsManager) {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    Result<void> result = graphicsManager.startup();
    EXPECT_EQ(result.getError(), Error::None);

    graphicsManager.shutdown();
}

TEST(RealEngineTestManagers, TestMemoryManager) {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();
    Result<void> gResult = graphicsManager.startup();
    EXPECT_EQ(gResult.getError(), Error::None);

    MemoryManager &memoryManager = MemoryManager::getManager();
    Result<void> result = memoryManager.startup();
    EXPECT_EQ(result.getError(), Error::None);

    memoryManager.shutdown();
    graphicsManager.shutdown();
}
