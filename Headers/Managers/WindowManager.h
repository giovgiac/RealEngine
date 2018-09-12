/**
 * WindowManager.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef WINDOWMANAGER_H_
#define WINDOWMANAGER_H_

#include "Result.h"

class WindowManager final {
private:

    std::shared_ptr<class Window> window;

private:
    explicit WindowManager();

    ~WindowManager();

public:

    inline static WindowManager &getManager() noexcept {
        static WindowManager inst;
        return inst;
    }

    Result<std::shared_ptr<class Window>> getWindow() const noexcept;

    Result<void> startup();

    void shutdown();

public:
    WindowManager(const WindowManager &) = delete;
    WindowManager(WindowManager &&) = delete;

    WindowManager &operator=(const WindowManager &) = delete;
    WindowManager &operator=(WindowManager &&) = delete;
};

#endif /* WINDOWMANAGER_H_ */
