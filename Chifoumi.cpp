
#include <iostream>
#include <ctime>
#include "menu.h"

using namespace std;

/**
 * @file Chifoumi.cpp
 * @brief Simple rock-paper-scissors game implementation and menu registration.
 */

/**
 * @brief Play a simple rock-paper-scissors (chifoumi) match.
 *
 * The function prompts the user for their selection (0=pierre, 1=feuille, 2=ciseaux)
 * and compares it to a single random choice made by the computer. After the
 * result is shown the user can choose to play again.
 *
 * @return int Always returns 0 (menu style entrypoint).
 */
int play_chifoumi()
{
	srand(time(0));
	int pierre = 0;
	int feuille = 1;
	int ciseaux = 2;
	int choix;
	int random = (rand() % 3);
	char rejouer;
	do
	{
		cout << "Pierre, feuille ou ciseaux ? (0=pierre, 1=feuille et 2=ciseaux.) ";
		cin >> choix;
		cout << "random: " << random << std::endl;
		cout << "choix :" << choix << std::endl;
		if (choix == 0 && random == 0)
		{
			cout << "Le joueur a choisit pierre et l'ordinateur a choisit pierre. Il y a donc egalite.";
		}
		else if (choix == 0 && random == 1)
		{
			cout << "Le joueur a choisit pierre et l'ordinateur a choisit feuille. L'ordinateur gagne la partie.";
		}
		else if (choix == 0 && random == 2)
		{
			cout << "Le joueur a choisit pierre et l'ordinateur a choisit ciseaux. le joueur gagne la partie.";
		}
		else if (choix == 1 && random == 0)
		{
			cout << "Le joueur a choisit feuille et l'ordinateur a choisit pierre. le joueur gagne la partie.";
		}
		else if (choix == 1 && random == 1)
		{
			cout << "Le joueur a choisit feuille et l'ordinateur a choisit feuille. Il y a donc egalite.";
		}
		else if (choix == 1 && random == 2)
		{
			cout << "Le joueur a choisit feuille et l'ordinateur a choisit ciseaux. L'ordinateur gagne la partie.";
		}
		else if (choix == 2 && random == 0)
		{
			cout << "Le joueur a choisit ciseaux et l'ordinateur a choisit pierre. L'ordinateur gagne la partie.";
		}
		else if (choix == 2 && random == 1)
		{
			cout << "Le joueur a choisit ciseaux et l'ordinateur a choisit feuille. Le joueur gagne la partie.";
		}
		else
		{
			cout << "Le joueur a choisit ciseaux et l'ordinateur a choisit ciseaux. Il y a donc egalite.";
		}
		do
		{
			cout << "Rejouer ? (o/n)";
			cin >> rejouer;
		} while (rejouer != 'o' && rejouer != 'n');
	} while (rejouer == 'o');

	return 0;
}
// Register the game in the menu system
struct register_chifoumi
{
	register_chifoumi()
	{
		addGame("Chifoumi", play_chifoumi);
	}
};

static register_chifoumi registerGameInstance; // Static instance to register games at startup
