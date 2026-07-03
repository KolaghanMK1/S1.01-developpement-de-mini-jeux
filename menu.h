#pragma once
#include <string>
#include <vector>
#include <functional>

struct game
{
    /**
     * @brief Name of the game displayed in the menu
     */
    std::string name; // Name of the game

    /**
     * @brief Callable used to launch the game.
     *
     * The callable must be invokable with no arguments and should implement
     * the game entrypoint (blocking until the game finishes).
     */
    std::function<void()> launchFunction; // Function to launch the game

    /**
     * @brief Construct a new game entry for the menu.
     *
     * @param n Display name
     * @param func Callable to launch the game
     */
    game(const std::string &n, std::function<void()> func) : name(n), launchFunction(func) {}
};

/**
 * @brief Access the global list of registered games.
 *
 * The returned reference refers to the internal container used by the menu
 * system to store registered game entries.
 *
 * @return std::vector<game>& Reference to the games container.
 */
std::vector<game> &getGameList();

/**
 * @brief Register a new game in the menu system.
 *
 * This function adds a new entry visible in the program menu. It is typically
 * called by static registration objects inside each game translation unit so
 * games are registered at startup.
 *
 * @param name Display name of the game
 * @param launchFunction Callable invoked to start the game
 */
void addGame(const std::string &name, std::function<void()> launchFunction);