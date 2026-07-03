#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include "menu.h"
#include "TUI-Engine--Prequel.h"
#include "secure_cin.hpp"
#include "Card_core.h"

// #ifdef Deck
// #pragma message("Deck IS define")
// #else
// #pragma message("Deck is NOT define")
// #endif

// Global flags for settings (shared between setter/getter)
/**
 * @file card_game_-_the_war.cpp
 * @brief Implementation of a simple card game "the war" and its menu.
 */

TUI_Core the_war_rendering;

// Global flags for settings (shared between setter/getter)
static bool g_special_rules = false;
static bool g_manual_mode = false;

/**
 * @brief Get or set the number of players used by the game.
 *
 * When `nb` is 2 or 4 it sets the stored value; calling without argument
 * returns the currently configured number of players.
 *
 * @param nb Optional new number of players (2 or 4).
 * @return int Currently configured number of players.
 */
int Set_nb_player(int nb = -1)
{
    static int nb_of_player = 2;

    if (nb == 2 || nb == 4)
    {
        nb_of_player = nb;
    }

    return nb_of_player;
}

// Special rules: provide explicit setter and getter to avoid accidental state changes
/**
 * @brief Enable or disable special game rules.
 *
 * @param state True to enable the special rules behavior.
 */
void Set_special_rules(bool state)
{
    g_special_rules = state;
}

/**
 * @brief Query whether special rules are active.
 *
 * @return true if special rules are enabled.
 */
bool Get_special_rules()
{
    return g_special_rules;
}

// Manual mode: explicit setter/getter
/**
 * @brief Enable or disable manual play mode (player 1 chooses cards).
 */
void Set_manual_mode(bool state)
{
    g_manual_mode = state;
}

/**
 * @brief Return whether manual play is enabled.
 */
bool Get_manual_mode()
{
    return g_manual_mode;
}

/**
 * @brief Distribute the cards from `deck` to the players until the deck is empty.
 *
 * Cards are drawn randomly from the deck and added to each player's hand in
 * round-robin fashion.
 */
void Distribute(std::vector<Player> &players, Deck &deck)
{
    while (!deck.empty())
    {
        for (int i = 0; i < players.size(); i++)
        {
            players.at(i).add_card_to_hand(deck.draw_random_card());
        }
    }
}

/**
 * @brief Display the ASCII-art representation of a player's hand.
 *
 * @param p Player whose hand will be displayed.
 */
void display_hand(Player p)
{
    std::vector<std::string> cards = p.get_cards_in_hand();
    the_war_rendering.render.horizontal_display("ascii_art/En/Art/cartes_ascii.txt", cards);
}

// Game function
/**
 * @brief Main game loop for "the war" card game.
 *
 * This function implements an automated round system where each active
 * player plays a card and the highest value wins the round. The game runs
 * until a victory condition, a safety round limit, or manual exit.
 *
 * @param nb_player Number of players to create for this session.
 * @return int Exit code (0 normally).
 */
int Play_the_war(int nb_player = Set_nb_player())
{
    std::srand((unsigned)std::time(nullptr));

    the_war_rendering.render.preload("ascii_art/En/Art/cartes_ascii.txt");
    Deck deck;
    Table table;
    std::vector<Player> Player_list;
    for (int i = nb_player; i > 0; i--)
    {
        Player_list.emplace_back();
    }
    Distribute(Player_list, deck);
    // Compute total number of cards in game (used as victory condition)
    int total_cards = 0;
    for (const auto &p : Player_list)
        total_cards += p.get_nb_card_handed();

    // Simple round loop: each round every player with at least one card plays one chosen card.
    // Winner is the player with the highest card value; ties are resolved by random choice among tied players.
    // Cap rounds to avoid infinite loops in pathological cases.
    while (true)
    {
        static const int MAX_ROUNDS = 100000;
        static int round_count = 0;
        if (++round_count > MAX_ROUNDS)
        {
            std::cout << "Max rounds reached, aborting game to avoid infinite loop." << std::endl;
            break;
        }
        // Check how many players still have cards
        int players_with_cards = 0;
        for (auto &p : Player_list)
            if (p.get_nb_card_handed() > 0)
                ++players_with_cards;
        if (players_with_cards <= 1)
            break; // game over

        // Each player plays one card (if they have one)
        struct Played
        {
            int idx;
            Deck::Card card;
        };
        std::vector<Played> played_cards;

        for (size_t i = 0; i < Player_list.size(); ++i)
        {
            Player &p = Player_list[i];
            if (p.get_nb_card_handed() == 0)
                continue; //"skip" the rest of the code to go to the end of the loop for this iteration.
            std::vector<std::string> hand_keys = p.get_cards_in_hand();
            size_t visible = std::min<size_t>(3, hand_keys.size());
            std::string key;
            if (i == 0 && Get_manual_mode())
            {
                the_war_rendering.render.clearScreen();
                // Let player 1 choose among first `visible` cards
                std::vector<std::string> visible_keys(hand_keys.begin(), hand_keys.begin() + visible);
                the_war_rendering.render.horizontal_display("ascii_art/En/Art/cartes_ascii.txt", visible_keys);
                std::cout << "Choose a card to play (1-" << visible << "): ";
                int choice = 1;
                secure_cin(choice);
                if (choice < 1 || choice > (int)visible)
                    choice = 1;
                key = visible_keys[choice - 1];
            }
            else
            {
                // AI: pick randomly among first `visible` cards
                size_t pick = std::rand() % visible;
                key = hand_keys[pick];
            }
            Deck::Card c = p.play(key);
            table.put_on_table(c);
            played_cards.push_back(Played{int(i), c});
        }

        // Display the table (ASCII art of the keys currently on table)
        the_war_rendering.render.horizontal_display("ascii_art/En/Art/cartes_ascii.txt", table.get_table());

        if (played_cards.empty())
            break; // no plays this round

        // Determine highest value considering special rules
        auto card_value_for_compare = [&](const Deck::Card &c) -> int
        {
            // If special rules activated: 2 (value 2) beats Ace (value 14)
            if (Get_special_rules())
            {
                if (c.value == 2)
                    return 100 + c.value; // boost 2s
                if (c.value == 14)
                    return c.value; // ace normal
            }
            return c.value;
        };

        int best_val = std::numeric_limits<int>::min();
        for (const auto &p : played_cards)
        {
            int v = card_value_for_compare(p.card);
            if (v > best_val)
                best_val = v;
        }

        // collect winners (could be ties)
        std::vector<int> winners;
        for (const auto &p : played_cards)
        {
            if (card_value_for_compare(p.card) == best_val)
                winners.push_back(p.idx);
        }

        // Choose a winner (random among tied players)
        int winner_idx = winners.size() == 1 ? winners[0] : winners[std::rand() % winners.size()];

        // Transfer all cards on the table to the winner
        std::vector<std::string> on_table_keys = table.get_table();
        for (const auto &k : on_table_keys)
        {
            Deck::Card c = table.take_from_table(k);
            Player_list[winner_idx].add_card_to_hand(c);
        }

        // Optional: show which player won the round
        std::cout << "Round winner: Player " << (winner_idx + 1) << " (won " << on_table_keys.size() << " cards)\n";
        std::cout << "Press enter to continue...";
        std::cin.ignore();
        std::cin.get();

        // Check victory condition: a player who has all cards wins
        if (Player_list[winner_idx].get_nb_card_handed() == total_cards)
        {
            std::cout << "Player " << (winner_idx + 1) << " has all the cards and wins the game!\n";
            break;
        }
    }

    // Final standings: sort players by number of cards (desc)
    std::vector<std::pair<int, int>> standings; // (player_index, card_count)
    for (size_t i = 0; i < Player_list.size(); ++i)
        standings.emplace_back(int(i), Player_list[i].get_nb_card_handed());

    std::sort(standings.begin(), standings.end(), [](const auto &a, const auto &b)
              { return a.second > b.second; });

    std::cout << "\nFinal standings:\n";
    for (size_t rank = 0; rank < standings.size(); ++rank)
    {
        std::cout << (rank + 1) << ". Player " << (standings[rank].first + 1) << " - " << standings[rank].second << " cards\n";
    }

    return 0;
}

/**
 * @brief Show and handle the settings menu for the war game.
 *
 * Allows changing the number of players, toggling special rules, and
 * enabling/disabling manual mode.
 *
 * @return int Menu exit code (0 normally).
 */
int Settings_the_war()
{
    int choice_setting = 0;
    do
    {
        switch (choice_setting)
        {
        default:
        {
            the_war_rendering.render.clearScreen();
            return 0;
        }
        case 0:
        {
            the_war_rendering.render.clearScreen(); // Clear console
            the_war_rendering.render.display("ascii_art/En/Font/Sub-zero/Menu.txt", "Settings", "\e[1;92m");
            std::cout << "1. Modify the number of players (current: " << Set_nb_player() << ")" << std::endl;
            std::cout << "2. Special rules (current: " << (Get_special_rules() ? "ON" : "OFF") << ")" << std::endl;
            std::cout << "3. (Un)activate manual play (player 1 chooses) (current: " << (Get_manual_mode() ? "ON" : "OFF") << ")" << std::endl;
            std::cout << "4. Exit" << std::endl;
            secure_cin(choice_setting);
            break;
        }
        case 1:
        {
            the_war_rendering.render.clearScreen();
            the_war_rendering.render.display("ascii_art/En/Font/Sub-zero/Menu.txt", "Settings", "\e[1;92m");
            int nb_player;
            std::cout << "Choose the number of players between 2 or 4 players:" << std::endl;
            std::cout << "Number of players: ";
            secure_cin(nb_player);
            Set_nb_player(nb_player);
            std::cout << "Press Enter to return to the setting menu..." << std::endl;
            std::cin.ignore(); // To ignore any leftover newline character in the input buffer
            std::cin.get();    // Wait for user to press Enter
            choice_setting = 0;
            break;
        }
        case 2:
        {
            the_war_rendering.render.clearScreen();
            the_war_rendering.render.display("ascii_art/En/Font/Sub-zero/Menu.txt", "Settings", "\e[1;92m");
            int spe_r = 0;
            do
            {
                switch (spe_r)
                {
                default:
                {
                    the_war_rendering.render.clearScreen();
                    the_war_rendering.render.display("ascii_art/En/Font/Sub-zero/Menu.txt", "Settings", "\e[1;92m");
                    std::cout << "Special rule hyper two" << std::endl;
                    std::cout << "1. Enable/Disable" << std::endl;
                    std::cout << "2. Help" << std::endl;
                    std::cout << "3. Exit";
                    secure_cin(spe_r);
                    break;
                }
                case 1:
                {
                    the_war_rendering.render.clearScreen();
                    the_war_rendering.render.display("ascii_art/En/Font/Sub-zero/Menu.txt", "Settings", "\e[1;92m");
                    if (Get_special_rules())
                    {
                        Set_special_rules(false);
                        std::cout << "The special rule has been disabled" << std::endl;
                    }
                    else
                    {
                        Set_special_rules(true);
                        std::cout << "The special rule has been enabled" << std::endl;
                    }

                    std::cout << "Press enter to return to the special rule menu";
                    std::cin.ignore();
                    std::cin.get();
                    spe_r = 0;
                    break;
                }
                case 2:
                {
                    the_war_rendering.render.clearScreen();
                    the_war_rendering.render.display("ascii_art/En/Font/Sub-zero/Menu.txt", "Settings", "\e[1;92m");
                    std::cout << "When this rule is enabled, the two only beats the ace, and the ace only loses to the 2.\nPress enter to return to the special rule menu";
                    std::cin.ignore();
                    std::cin.get();
                    spe_r = 0;
                    break;
                }
                case 3:
                {
                    the_war_rendering.render.clearScreen();
                    the_war_rendering.render.display("ascii_art/En/Font/Sub-zero/Menu.txt", "Settings", "\e[1;92m");
                    std::cout << "Press enter to exit";
                    std::cin.ignore();
                    std::cin.get();
                    spe_r = 4;
                    break;
                }
                }
            } while (spe_r >= 0 && spe_r < 4);
            choice_setting = 0;
            break;
        }
        case 3:
        {
            the_war_rendering.render.clearScreen();
            the_war_rendering.render.display("ascii_art/En/Font/Sub-zero/Menu.txt", "Settings", "\e[1;92m");
            if (Get_manual_mode())
            {
                Set_manual_mode(false);
                std::cout << "Manual play disabled" << std::endl;
            }
            else
            {
                Set_manual_mode(true);
                std::cout << "Manual play enabled (player 1 will choose among first 3 cards)" << std::endl;
            }
            std::cout << "Press enter to return to settings..." << std::endl;
            std::cin.ignore();
            std::cin.get();
            choice_setting = 0;
            break;
        }
        }
    } while (choice_setting <= 3);
    return 0;
}

/**
 * @brief Display the credits and wait for user acknowledgement.
 *
 * @return int Exit code (0 to return to menu).
 */
int Credits_the_war()
{
    the_war_rendering.render.clearScreen(); // Clear console
    std::cout << "Credits: Bardoux Théo" << std::endl;
    std::cout << "Press Enter to return to the menu..." << std::endl;
    std::cin.ignore(); // To ignore any leftover newline character in the input buffer
    std::cin.get();    // Wait for user to press Enter
    return 0;          // Return to main menu
}

/**
 * @brief Top-level menu for the war card game.
 *
 * Provides entries to play the game, configure settings, view credits or exit.
 */
void the_war_Menu()
{
    int choice = 0;
    do
    {
        switch (choice)
        {
        default:
        {
            the_war_rendering.render.clearScreen(); // Clear console
            the_war_rendering.render.display("ascii_art/En/Font/3D-ASCII/Card_game_war.txt", "the war", "\e[1;93m");
            the_war_rendering.render.display("ascii_art/En/Font/Chunky/Menu_choices.txt", "1. Play", "\e[1;91m");
            the_war_rendering.render.display("ascii_art/En/Font/Chunky/Menu_choices.txt", "2. Settings", "\033[1;92m");
            the_war_rendering.render.display("ascii_art/En/Font/Chunky/Menu_choices.txt", "3. Credits", "\033[1;94m");
            the_war_rendering.render.display("ascii_art/En/Font/Chunky/Menu_choices.txt", "4. Exit", "\033[1;97m");
            std::cout << "Enter your choice: ";
            secure_cin(choice);
            break; // Prevent fallthrough
        }
        case 1:
        {
            the_war_rendering.render.clearScreen();
            choice = Play_the_war();
            break;
        }
        case 2:
        {
            the_war_rendering.render.clearScreen();
            choice = Settings_the_war();
            break;
        }
        case 3:
        {
            the_war_rendering.render.clearScreen();
            choice = Credits_the_war();
            break;
        }
        case 4:
        {
            std::cout << "Exiting  Menu." << std::endl;
            break;
        }
        }
    } while (choice != 4);
    the_war_rendering.render.clearScreen(); // Clear console
}

// Register the game in the menu system
/**
 * @brief Static helper that registers the war game at program startup.
 */
struct register_Cardgame__the_war
{
    register_Cardgame__the_war()
    {
        addGame("Card game: the war", the_war_Menu);
    }
};

static register_Cardgame__the_war registerGameInstance; // Static instance to register games at startup