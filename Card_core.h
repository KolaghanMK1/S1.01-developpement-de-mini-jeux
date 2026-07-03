#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>

/**
 * @file Card_core.h
 * @brief Core card/deck/player/table types used by the card games.
 */

/**
 * @brief Represents a deck of playing cards.
 *
 * The Deck class stores a collection of Card objects and provides drawing
 * operations. The deck can be a full 52-card deck or a smaller 32-card deck
 * depending on the `full` flag passed to the constructor.
 */
class Deck
{
public:
    struct Card
    {
        /** Numeric value of the card (2-14 where 14=Ace). */
        int value;

        /** Suit character ('S','H','D','C'). */
        char suit;

        /**
         * @brief Compare two cards for equality (value and suit).
         */
        bool operator==(const Card &other) const;

        /**
         * @brief Build a key string representing a card from value and suit.
         *
         * @param v Numeric value
         * @param s Suit character
         * @return std::string Key used in maps
         */
        static std::string make_key(int v, char s);

        /** Construct a card from value and suit. */
        Card(int v, char s);

        /** Construct a card from its key representation (e.g. "14S"). */
        Card(std::string k);
    };

private:
    bool complete;                              /**< Whether the deck is the full 52-card deck. */
    std::unordered_map<std::string, Card> deck; /**< Map key->Card storing remaining cards. */

    /**
     * @brief Build the underlying map of cards for the requested deck size.
     *
     * @param full When true produce a full 52-card deck, otherwise a 32-card deck.
     */
    static std::unordered_map<std::string, Card> build(bool full);

public:
    /** Construct the deck; `full` selects 52 vs 32 cards. */
    Deck(bool full = true) : complete(full), deck(build(full)) {}

    /** @return true when no cards remain in the deck. */
    bool empty();

    /**
     * @brief Draw and remove a specific card identified by its key.
     *
     * @param k Key of the card to draw
     * @return Card The drawn card
     */
    Card draw(const std::string &k);

    /**
     * @brief Draw a random card from the deck and remove it.
     *
     * @return Card Randomly selected card.
     * @throws std::runtime_error if the deck is empty.
     */
    Card draw_random_card();
};

/**
 * @brief Represents a player holding cards in hand.
 */
class Player
{
private:
    using Card = Deck::Card;
    std::unordered_map<std::string, Card> hand; /**< Map key->Card for quick access. */
    std::vector<std::string> cards_in_hand;     /**< Order of keys in the player's hand. */

public:
    /** @return Vector of card keys currently in the player's hand. */
    std::vector<std::string> get_cards_in_hand();

    /** Add a card to the player's hand. */
    void add_card_to_hand(Card c);

    /**
     * @brief Play (remove and return) a specific card by key.
     * @param k Key of the card to play
     * @return Card The played card
     */
    Card play(const std::string &k);

    /**
     * @brief Play the first card in the player's hand (FIFO).
     * @return Card The played card
     */
    Card play();

    /** @return Number of cards currently held by the player. */
    int get_nb_card_handed() const;
};

/**
 * @brief Represents the central table where played cards are placed.
 */
class Table
{
private:
    using Card = Deck::Card;
    std::unordered_map<std::string, Card> on_table; /**< cards currently on the table */
    std::vector<std::string> v_on_table;            /**< insertion/play order of cards */

public:
    /** Place a card on the table. */
    void put_on_table(Card c);

    /**
     * @brief Remove and return a card from the table identified by key.
     * @param k Key of the card to take
     * @return Card The removed card
     */
    Card take_from_table(const std::string k);

    /** @return Keys of cards currently on the table in play order. */
    std::vector<std::string> get_table();
};
