/**
 * WindowManager.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Window.h"
#include "WindowManager.h"

#include <iostream>

WindowManager::WindowManager() {
    this->window = nullptr;
}

WindowManager::~WindowManager() {
    this->window.reset();
}

Result<std::shared_ptr<Window>> WindowManager::getWindow() const noexcept {
    if (this->window != nullptr) {
        return Result<std::shared_ptr<Window>>(this->window);
    }
    else {
        return Result<std::shared_ptr<Window>>::createError(Error::WindowManagerNotStartedUp);
    }
}

Result<void> WindowManager::startup() {
    std::cout << "Starting Up WindowManager..." << std::endl;

    this->window = std::make_shared<Window>(640, 480, "Real Engine");

    Result<void> result = this->window->startup();
    if (result.hasError()) {
        std::cout << "Failed To Start Up WindowManager - Window..." << std::endl;
        return Result<void>::createError(result.getError());
    }

    return Result<void>::createError(Error::None);
}

void WindowManager::shutdown() {
    if (this->window != nullptr) {
        this->window->shutdown();
    }

    this->window.reset();
    std::cout << "Shutting Down WindowManager..." << std::endl;
}
