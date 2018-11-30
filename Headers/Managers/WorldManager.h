/**
 * WorldManager.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef WORLDMANAGER_H_
#define WORLDMANAGER_H_

#include "Result.h"

class WorldManager final {
private:
    std::shared_ptr<class Renderer> renderer;
    std::forward_list<std::shared_ptr<struct SpriteComponent>> components;

private:
    explicit WorldManager();

    ~WorldManager();

    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

public:
    void addObject(std::shared_ptr<struct SpriteComponent> object) noexcept;

    inline static WorldManager &getManager() noexcept {
        static WorldManager inst;
        return inst;
    }

    Result<std::shared_ptr<class Renderer>> getRenderer() const noexcept;

    Result<void> play(class Game *game);

    Result<void> startup();

    void shutdown();

public:
    WorldManager(const WorldManager &) = delete;
    WorldManager(WorldManager &&) = delete;

    WorldManager &operator=(const WorldManager &) = delete;
    WorldManager &operator=(WorldManager &&) = delete;
};

#endif /* WORLDMANAGER_H_ */
