#include <iostream>
#include "menu.h"

using namespace std;

/**
 * @file morpion.cpp
 * @brief Tic-tac-toe (morpion) game implementation and menu registration.
 */

/**
 * @brief Display the morpion board to stdout.
 *
 * @param tab 2D character grid representing the board
 * @param size Size of the board (square)
 */
void affichage_morpion(char **tab, int size)
{
	cout << endl
		 << "   ";
	for (int i = 0; i < size; ++i)
		cout << " " << i << "  ";
	cout << endl;
	for (int i = 0; i < size; ++i)
	{

		cout << i << " | ";
		for (int j = 0; j < size; ++j)
		{

			cout << tab[i][j] << " | ";
		}
		cout << endl; // permet l'affichage du tableau créé
	}
}

/**
 * @brief Allocate and initialise a morpion board.
 *
 * Allocates a size x size grid and fills it with spaces ' '. The caller is
 * responsible for calling `delete_morpion` to free the memory.
 *
 * @param tab Reference to the pointer that will receive the allocated grid.
 * @param size Dimension of the board.
 */
void init_morpion(char **&tab, int size)
{

	tab = new char *[size];

	for (int i = 0; i < size; ++i)
		tab[i] = new char[size];

	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			tab[i][j] = ' '; // permet la création de chaque ligne et colonne de la grille du morpion
}

/**
 * @brief Free a previously allocated morpion board.
 *
 * @param tab Pointer to the board returned by `init_morpion`.
 * @param size Dimension of the board that was allocated.
 */
void delete_morpion(char **tab, int size)
{
	for (int i = 0; i < size; ++i)
		{
			delete[] tab[i];
		}
	delete[] tab;
} // Libère toutes les lignes du tableau pour la mémoire

/**
 * @brief Place a pawn on the board if the target cell is within bounds and free.
 *
 * This function performs bounds checking and only places the character when
 * the destination cell is empty (' ').
 *
 * @param tab Board pointer
 * @param size Board dimension
 * @param x Row index
 * @param y Column index
 * @param pion Pawn character to place (e.g. 'X' or 'O')
 */
void placer_morpion(char **tab, int size, int x, int y, char pion)
{

	if (x < size && x >= 0 && y >= 0 && y < size && tab[x][y] == ' ')
		tab[x][y] = pion; // permet de s'assurer que les placement ne peuvent se faire que dans le tableau et dans une case vide donc disponible
}

/**
 * @brief Check whether a cell is free and inside the board bounds.
 *
 * @return true if the cell is available for a move.
 */
bool estLibre_morpion(char **tab, int size, int x, int y)
{

	return (x >= 0 && y >= 0 && x < size && y < size && tab[x][y] == ' ');

} // permet que le joueur ne joue que dans une case où il est possible de jouer

/**
 * @brief Verify whether a player has achieved the win condition.
 *
 * The function checks vertical, horizontal and two diagonal directions for
 * `nbpion` consecutive occurrences of `pion`.
 *
 * @param tab Board pointer
 * @param size Board dimension
 * @param nbpion Number of pieces to align to win
 * @param pion Pawn character to check
 * @return true if the pawn has a winning alignment
 */
bool verifVictoire_morpion(char **tab, int size, int nbpion, char pion)
{
	// permet de réaliser plusieurs tests pour voir si la condition de victoire est accomplie
	for (int i = 0; i < size; ++i)
	{

		for (int j = 0; j < size; ++j)
			if (tab[i][j] == pion)
			{

				int align = 0, i2, j2;

				i2 = i;
				align = 0;
				while (i2 < size && (i2 - i) < nbpion)
				{
					align = align + (tab[i2][j] == pion);
					i2++;
				} // vérification verticale
				if (align == nbpion)
					return true;

				j2 = j;
				align = 0;
				while (j2 < size && (j2 - j) < nbpion)
				{
					align = align + (tab[i][j2] == pion);
					j2++;
				} // vérification horizontale
				if (align == nbpion)
					return true;

				j2 = j;
				i2 = i;
				align = 0;
				while (i2 < size && j2 < size && (i2 - i) < nbpion)
				{
					align += (tab[i2][j2] == pion);
					i2++;
					j2++;
				}
				if (align == nbpion)
					return true; // vérification diaonale droite

				j2 = j;
				i2 = i;
				align = 0;
				while (i2 < size && j2 >= 0 && (i2 - i) < nbpion)
				{
					align += (tab[i2][j2] == pion);
					i2++;
					j2--;
				}
				if (align == nbpion)
					return true; // vérification diaonale gauche
			}
	}

	return false;
}

/**
 * @brief Main entrypoint for the morpion game used by the menu system.
 *
 * This interactive function reads board parameters from `std::cin` and runs
 * a two-player game until a win or a full board is reached.
 *
 * @return int Always returns 0.
 */
int play_morpion()
{

	char **tab;
	int size;
	int nbPion;
	cout << "Entrez la taille du morpion (ex : 3, 5, 20...) : ";
	cin >> size;

	cout << "Entrez le nombre de pions à aligner pour gagner : ";
	cin >> nbPion;

	init_morpion(tab, size);

	bool alignement = false;
	bool joueur = false;

	int nbPl = 0;

	while (!alignement && nbPl < size * size)
	{

		affichage_morpion(tab, size);

		char pion;
		if (joueur)
		{
			pion = 'X';
			cout << "Joueur 1 (X), à vous de jouer : ";
		}
		else
		{
			pion = 'O';
			cout << "Joueur 2 (O), à vous de jouer : ";
		}
		joueur = !joueur;

		int x = -1;
		int y = -1;

		do
		{

			std::cin >> x >> y;
			std::cout << std::endl;

		} while (!estLibre_morpion(tab, size, x, y));

		placer_morpion(tab, size, x, y, pion);
		affichage_morpion(tab, size);

		alignement = verifVictoire_morpion(tab, size, nbPion, pion);
		nbPl++;
	}
	delete_morpion(tab, size);
	return 0;
}

// Register the game in the menu system
/**
 * @brief Static registration helper to expose the morpion game to the menu.
 */
struct register_morpion
{
	register_morpion()
	{
		addGame("Morpion", play_morpion);
	}
};

static register_morpion registerGameInstance; // Static instance to register games at startup