/**
 * main.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "GraphicsManager.h"

int main() {
    GraphicsManager &graphicsManager = GraphicsManager::getManager();

    // Startup Managers
    Result<void> graphicsResult = graphicsManager.startup();
    if (graphicsResult.hasError())
        return 1;

    // Shutdown Managers
    graphicsManager.shutdown();

    return EXIT_SUCCESS;
}