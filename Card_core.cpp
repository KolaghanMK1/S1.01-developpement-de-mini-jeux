#include "Card_core.h"
/**
 * @file Card_core.cpp
 * @brief Implementation of Deck, Player and Table operations.
 */

// ==== Deck::Card ====
bool Deck::Card::operator==(const Deck::Card &other) const
{
    return value == other.value && suit == other.suit;
}

std::string Deck::Card::make_key(int v, char s)
{
    return std::to_string(v) + s;
}

Deck::Card::Card(int v, char s) : value(v), suit(s) {}

Deck::Card::Card(std::string k) : value(std::stoi(k.substr(0, k.size() - 1))), suit(k.back()) {}

// ==== Deck ====
std::unordered_map<std::string, Deck::Card> Deck::build(bool full)
{
    std::unordered_map<std::string, Deck::Card> d;
    d.reserve(full ? 52 : 32);
    char suits[4] = {'S', 'H', 'D', 'C'};
    int start = full ? 2 : 7;

    for (char s : suits)
        for (int v = start; v <= 14; ++v)
            d.emplace(Deck::Card::make_key(v, s), Deck::Card(v, s));

    return d;
}

bool Deck::empty()
{
    return deck.empty();
}

Deck::Card Deck::draw(const std::string &k)
{
    Deck::Card c = deck.at(k);
    deck.erase(k);
    return c;
}

Deck::Card Deck::draw_random_card()
{
    if (deck.empty())
    {
        throw std::runtime_error("Le deck est vide"); // Lance une erreur si le deck est vide
    }

    // Générer un index aléatoire dans le deck
    auto it = deck.begin();
    std::advance(it, rand() % deck.size());
    Card c = it->second; // Obtenir la carte

    deck.erase(it); // Supprimer la carte tirée du deck
    return c;
}

// ==== Player ====
std::vector<std::string> Player::get_cards_in_hand()
{
    return cards_in_hand;
}

void Player::add_card_to_hand(Card c)
{
    std::string key = Deck::Card::make_key(c.value, c.suit);
    hand.insert({key, c});
    cards_in_hand.push_back(key);
}

Player::Card Player::play(const std::string &k)
{
    for (std::vector<std::string>::iterator it = cards_in_hand.begin(); it != cards_in_hand.end(); ++it)
    {
        if (*it == k)
        {
            cards_in_hand.erase(it);
            break;
        }
    }
    Card c = hand.at(k);
    hand.erase(k);
    return c;
}

Player::Card Player::play()
{
    Card c = hand.at(cards_in_hand.front());
    hand.erase(cards_in_hand.front());
    cards_in_hand.erase(cards_in_hand.begin());
    return c;
}

int Player::get_nb_card_handed() const
{
    return int(cards_in_hand.size());
}

// ==== Table ====
void Table::put_on_table(Card c)
{
    std::string key = Deck::Card::make_key(c.value, c.suit);
    on_table.insert({key, c});
    v_on_table.push_back(key);
}

Table::Card Table::take_from_table(const std::string k)
{
    Card c = on_table.at(k);
    on_table.erase(k);
    for (auto it = v_on_table.begin(); it != v_on_table.end(); ++it)
    {
        if (*it == k)
        {
            v_on_table.erase(it);
            break;
        }
    }
    return c;
}

std::vector<std::string> Table::get_table()
{
    // Return keys in insertion/play order
    return v_on_table;
}