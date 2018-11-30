/**
 * Game.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef GAME_H_
#define GAME_H_

#include "Result.h"

class Game {
public:
    Game() = default;
    virtual ~Game() = default;

    virtual void begin() = 0;
    virtual void update() = 0;

    Result<void> startup();

    Result<void> play();

    void shutdown();

public:
    Game(const Game &) = delete;
    Game(Game &&) = delete;

    Game &operator=(const Game &) = delete;
    Game &operator=(Game &&) = delete;
};

#endif // GAME_H_
