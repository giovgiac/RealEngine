/**
 * TestManagerFixture.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef TESTMANAGERFIXTURE_H_
#define TESTMANAGERFIXTURE_H_

#include "GraphicsManager.h"
#include "MemoryManager.h"

#include <gtest/gtest.h>

class TestManagerFixture : public ::testing::Test {
public:
    void SetUp() override {
        GraphicsManager &graphicsManager = GraphicsManager::getManager();
        MemoryManager &memoryManager = MemoryManager::getManager();

        Result<void> gResult = graphicsManager.startup();
        ASSERT_EQ(gResult.getError(), Error::None);

        Result<void> mResult = memoryManager.startup();
        ASSERT_EQ(mResult.getError(), Error::None);
    }

    void TearDown() override {
        GraphicsManager &graphicsManager = GraphicsManager::getManager();
        MemoryManager &memoryManager = MemoryManager::getManager();

        memoryManager.shutdown();
        graphicsManager.shutdown();
    }
};

#endif /* TESTMANAGERFIXTURE_H_ */
