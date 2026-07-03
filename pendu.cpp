/**
 * pendu.cpp
 * Hangman (Pendu) game implementation and menu registration.
 * This file contains game logic and helper functions.
 */
#include <iostream>
#include "menu.h"
#include "TUI-Engine--Prequel.h"
#include "secure_cin.hpp"
using namespace std;

TUI_Core tui;

/**
 * @brief Determine whether a string contains unsupported (non-printable) characters.
 *
 * Only printable ASCII characters (32..126) are considered supported by the
 * game's input system.
 *
 * @param str Input string to check.
 * @return true if the string contains unsupported characters.
 */
bool hasUnsupportedChar(const string &str)
{ // Fonction vérifiant si la chaine de caractères contient des caractères non-ASCII

	for (unsigned char c : str)
	{

		if (c < 32 || c > 126)
		{
			return true;
		}
	}

	return false;

} // Fin fonction hasUnsupportedChar

/**
 * @brief Verify the format of the given string for use in the game.
 *
 * Returns an integer code describing the result (0 = OK, 1 = contains unsupported
 * characters but has letters, 2 = contains no letters, 3 = unsupported characters
 * and no letters).
 *
 * @param str Reference to the string to verify (may be modified).
 * @return int Status code described above.
 */
int verifFormat(string &str)
{ // Fonction vérifiant si le format de la chaine de caractères est conforme pour utilisation

	int countLetters = 0;
	bool containsUnsupportedChar = hasUnsupportedChar(str);

	for (size_t i = 0; i < str.length(); ++i)
	{

		unsigned char c = static_cast<unsigned char>(str[i]);

		if (c < 128)
		{ // Si c'est un caractère ASCII

			if ((c > 64 && c < 91) || (c > 96 && c < 123))
			{
				++countLetters;
			} // Si c'est une lettre, ajoute 1 au compteur

		} // Fin if

	} // Fin for

	if (containsUnsupportedChar && countLetters > 0)
	{
		return 1;
	} // Si caractères non-ASCII trouvés

	else if (!containsUnsupportedChar && countLetters == 0)
	{
		return 2;
	} // Sinon si pas de lettres trouvées

	else if (containsUnsupportedChar && countLetters == 0)
	{
		return 3;
	} // Sinon si les 2 (caractère non-ASCII + pas de lettre)

	else
	{ //(!containsUnsupportedChar && count_Letters>0) //Sinon (si pas de caractère non-ASCII + au moins une lettre)

		if (str[str.length() - 1] == ' ')
		{
			str.pop_back();
		} // Si le dernier caractère de la chaine de caractères est un espace ' ', le supprimer

		return 0;

	} // Fin else

} // Fin fonction verifFormaat

/**
 * @brief Display messages explaining format verification results.
 *
 * When `stateFormat` is 0 this function sets `formatOK` to true, otherwise it
 * prints appropriate messages describing the input issue.
 */
void displayVerifFomat(int &stateFormat, bool &formatOK)
{ // Processus affichant bloquant sur une boucle et affichant un message si le format de la chaine de caractères est non-conforme

	if (stateFormat != 0)
	{

		if (stateFormat == 1 || stateFormat == 3)
		{
			cout << "Vous avez saisi un ou plusieurs caractères non-autorisés, veuillez réessayer." << endl;
		}

		if (stateFormat == 2 || stateFormat == 3)
		{
			cout << "Veuillez saisir au moins une lettre." << endl;
		}
	}

	else
	{
		formatOK = true;
	}

} // Fin processus displayVerifFomat

/**
 * @brief Reveal non-letter characters immediately in the progress display.
 *
 * Non-letter characters (punctuation, spaces, etc.) are copied into
 * `progress` and marked in `remainingChars` so they are not considered
 * part of the letters to be guessed.
 */
void displayNonLetters(const string &toGuess, string &progress, string &remainingChars)
{ // Processus qui les caractères spéciaux dès le début dans la progression

	for (size_t i = 0; i < toGuess.length(); ++i)
	{

		if ((toGuess[i] > 64 && toGuess[i] < 91) || (toGuess[i] > 96 && toGuess[i] < 123))
		{
			remainingChars[i] = '^';
		} // Si c'est une lettre, la mettre en avant

		else
		{ // Sinon (si c'est un caractère spécial)

			progress[i] = toGuess[i]; // L'afficher dans la progression
			remainingChars[i] = ' ';  // Ne plus mettre en avant le caractère

		} // Fin else

	} // Fin for

} // Fin fonction remainingChars

/**
 * @brief Convert lowercase ASCII letters in a string to uppercase.
 *
 * The conversion is performed in-place and the modified string is returned.
 */
string formatMAJ(string &str)
{ // Change toutes les "lettres" en "LETTRES" (les met en MAJUSCULES)

	for (size_t i = 0; i < str.length(); ++i)
	{ // Analyse chaque caractère de la chaine de caractère

		if (96 < str[i] && str[i] < 123)
		{
			str[i] = str[i] - 32;
		} // Si c'est une lettre, la changer en MAJUSCULE (grâce au décalage dans la table ASCII)

	} // FIn for

	return str;
}

/**
 * @brief Build a printable representation of a vector of strings.
 *
 * Each element is wrapped with `str1` and `str2` and joined using `str3`.
 */
string displayVector(const vector<string> &vec, const string &str1, const string &str2, const string &str3)
{ // Fonction qui affiche un vecteur, en précisant les caractères encadrant chaque donnée (str1 et str2) et les séparant lors de l'affichage (str3)

	string displayedVector;

	for (size_t i = 0; i < vec.size(); ++i)
	{

		if (displayedVector.empty())
		{
			displayedVector = str1 + vec[i] + str2;
		}

		else
		{
			displayedVector = displayedVector + str3 + str1 + vec[i] + str2;
		}
	}

	return displayedVector;
}
/**
 * @brief Count occurrences of a character inside a string.
 *
 * @param str Input string
 * @param cha Character to count
 * @return int Number of occurrences
 */
int countOccurencesChar(const string &str, char cha)
{ // Compte le nombre d'occurences d'un caractère dans une chaine de caractère

	int counter = 0;

	for (size_t i = 0; i < str.length(); ++i)
	{ // Parcourt le mot

		if (str[i] == cha)
		{
			++counter;
		} // Vérifie que la lettre est dans le mot et ajoute 1 au compteur

	} // Fin for

	return counter;
}

/**
 * @brief Count occurrences of a string inside a vector.
 *
 * @param vec Vector to search
 * @param str Element to count
 * @return int Number of occurrences
 */
int countOccurencesVector(const vector<string> &vec, const string &str)
{ // Compte le nombre d'occurences d'une chaine de caractère dans un vecteur

	int counter = 0;

	for (size_t i = 0; i < vec.size(); ++i)
	{ // parcourt le vecteur

		if (vec[i] == str)
		{
			++counter;
		} // vérifie que la chaine de caractère est dans le vecteur

	} // Fin for

	return counter;
}

/**
 * @brief Process end-of-game checks and display final messages.
 *
 * If all letters were found the player wins; if `errors` reaches the limit
 * the player loses. The function also stops the game loop via `play` and
 * waits for the player to press Enter.
 */
void endGame(const string &toGuess, const string &remainingChars, int &errors, bool &play)
{ // Processus gérant et affichant les cas de victoire et de défaite

	if (countOccurencesChar(remainingChars, '^') == 0 || errors == 10)
	{

		if (countOccurencesChar(remainingChars, '^') == 0)
		{
			cout << "Bravo, vous avez gagné !" << endl;
		}

		else if (errors == 10)
		{
			cout << "Dommage, vous avez perdu !" << endl;
		}

		cout << "Vous deviez deviner : " << toGuess << endl
			 << endl;
		play = false;
		cout << "Appuyez 2 fois sur la touche Entrée pour quitter." << endl;
		cin.ignore();
		cin.get();

	} // Fin if
}

/**
 * @brief Verify the player's guess and update game state.
 *
 * The function updates tried word/letter lists, the health bar and the
 * `errors` counter and sets `verif` to indicate the verification result.
 */
void verifPendu(vector<string> &triedWrongWords, vector<string> &triedWrongLetters, vector<string> &triedCorrectLetters, vector<string> &healthBar, string &useranswer, const string toGuess, string &progress, string &remainingChars, int &errors, int &verif)
{ // Processus de vérification de la réponse

	if (useranswer.length() > toGuess.length())
	{ // Vérifie que la réponse n'est pas plus longue que toGuess

		if (countOccurencesVector(triedWrongWords, useranswer) == 0)
		{ // Si la réponse n'a jamais été tentée

			triedWrongWords.push_back(useranswer); // Ajout de la réponse au vecteur contenant les mauvaises réponses
			verif = 1;
			++errors;			  // Ajout de 1 au compteur d'erreurs
			healthBar.pop_back(); // Retrait d'une vie au compteur d'erreurs
		}

		else
		{
			verif = 5;
		}
	}

	else if (useranswer == toGuess)
	{ // Vérifie que la réponse est toGuess

		for (size_t i = 0; i < toGuess.length(); ++i)
		{
			remainingChars[i] = ' ';
		}

		progress = useranswer;
		verif = 0;
	}

	else
	{ /*(useranswer!=toGuess)*/ // Vérifie que la réponse n'est pas toGuess

		if (useranswer.length() > 1)
		{ // Vérifie que la réponse n'est pas plus longue qu'un caractère

			if (countOccurencesVector(triedWrongWords, useranswer) == 0)
			{ // Si la réponse n'a jamais été tentée

				triedWrongWords.push_back(useranswer);
				verif = 2;
				++errors;
				healthBar.pop_back();
			}

			else
			{
				verif = 5;
			}
		}

		else if (useranswer.length() == 1)
		{ // Vérifie que la réponse est une lettre

			bool found = false;

			for (size_t i = 0; i < toGuess.length(); ++i)
			{

				if (toGuess[i] == useranswer[0])
				{ // Vérifie que la lettre proposée est dans toGuess

					progress[i] = useranswer[0];
					remainingChars[i] = ' ';
					found = true;

				} // Fin if

			} // Fin for

			if (found)
			{ // Si le caractère est dans toGuess

				if (countOccurencesVector(triedCorrectLetters, useranswer) == 0)
				{ // Si la lettre n'a jamais été tentée

					triedCorrectLetters.push_back(useranswer);
					verif = 3;
				}

				else
				{
					verif = 6;
				}
			}

			else
			{ // Sinon (si la réponse n'est ni toGuess, ni un caractère correcte)

				if (countOccurencesVector(triedWrongLetters, useranswer) == 0)
				{ // Si la lettre n'a jamais été tentée

					triedWrongLetters.push_back(useranswer);
					verif = 4;
					++errors;
					healthBar.pop_back();
				}

				else
				{
					verif = 6;
				}

			} // Fin else

		} // Fin else if

	} // Fin else

} // Fin processus verifPendu

/**
 * @brief Display verification result messages produced by `verifPendu`.
 *
 * The function maps verification codes to user-friendly messages and calls
 * `endGame` if the game has been completed.
 */
void displayVerifPendu(const string useranswer, const string toGuess, const string remainingChars, int &errors, int &verif, bool &play)
{ // Processus d'affichage du résultat de la vérification

	if (!useranswer.empty())
	{

		switch (verif)
		{

		case 1:
			cout << "La réponse proposée est trop longue." << endl
				 << endl;
			break;
		case 2:
			cout << useranswer << " n'est pas la bonne réponse." << endl
				 << endl;
			break;
		case 3:
			cout << "Bonne lettre : " << useranswer << endl
				 << endl;
			break;
		case 4:
			cout << "Mauvaise lettre : " << useranswer << endl
				 << endl;
			break;
		case 5:
			cout << "La réponse " << useranswer << " a déjà été proposée, réessayez avec une autre réponse." << endl
				 << endl;
			break;
		case 6:
			cout << "La lettre " << useranswer << " a a déjà été proposée, réessayez avec une autre lettre." << endl
				 << endl;
			break;
		}

		endGame(toGuess, remainingChars, errors, play);

	} // Fin if

} // Fin processus displayVerifPendu

/**
 * @brief Print game rules and input support notes for players.
 */
void DisplayRulesAndSupport()
{ // Affichage des règles + support

	cout << "Règles :" << endl
		 << endl;
	cout << "Le Joueur 1 saisit un mot ou une phrase à faire deviner." << endl;
	cout << "Le Joueur 2 doit ensuite le deviner en proposant des lettres ou une réponse complète :" << endl;
	cout << "- Si la lettre est correcte, toutes ses occurrences apparaissent dans la progression." << endl;
	cout << "- Si la réponse complète est correcte, le Joueur 2 gagne immédiatement." << endl;
	cout << "- Si la lettre ou la réponse est incorrecte, le pendu progresse." << endl;
	cout << "- Lorsque le pendu est complet, la partie est terminée et le Joueur 2 a perdu." << endl
		 << endl;

	cout << "Remarques liées au support :" << endl
		 << endl;
	cout << "Seuls les caractères ASCII visibles et les phrases composées de lettres et d'espaces sont supportés." << endl;
	cout << "Les caractères ASCII non visibles et tous les caractères non-ASCII ne sont pas pris en charge." << endl;
	cout << "Les caractères ASCII visibles qui ne sont pas des lettres (comme les espaces ou la ponctuation) sont affichés dans la progression dès le début de la partie." << endl;
	cout << "La répétition d'une lettre ou d'un mot, intentionnelle ou non, n'est pas pénalisée et sera simplement signalée." << endl
		 << endl;

	cout << "Exemples d'entrées non conformes :" << endl;
	cout << "- Lettres accentuées (é, è, à, ç, etc.) ;" << endl;
	cout << "- Caractères spéciaux invisibles ;" << endl;
	cout << "- Caractères non-ASCII obtenus via un raccourci du type \"Alt+X\" avec X un nombre." << endl
		 << endl;

	cout << "IMPORTANT : Règle de sécurité : l'usage des flèches directionnelles est interdit." << endl;
	cout << "Elles permettent d'afficher les saisies précédentes et compromettent le déroulement du jeu." << endl
		 << endl
		 << endl;
}

/**
 * @brief Handle input and validation for player 1 (the secret word entry).
 *
 * Reads the secret string from stdin, validates its format and converts it
 * to uppercase for the game.
 */
void Player1Turn(string &toGuess, int &stateFormat, bool &formatOK)
{

	// Affiche le logo du jeu
	tui.render.display("ascii_art/En/Font/Poison/Pendu_menu.txt", "Title", "\e[1;92m");

	// Affichage des règles + support
	DisplayRulesAndSupport();

	// Joueur 1 saisit toGuess
	cout << "JOUEUR 1 | Que souhaitez-vous faire deviner ? : ";
	getline(cin >> ws, toGuess);
	stateFormat = verifFormat(toGuess);
	displayVerifFomat(stateFormat, formatOK);

	// Boucle si format incorrect détecté
	while (!formatOK)
	{

		cout << endl
			 << endl;
		cout << "JOUEUR 1 | Que souhaitez-vous faire deviner ? : ";
		getline(cin >> ws, toGuess);
		stateFormat = verifFormat(toGuess);
		displayVerifFomat(stateFormat, formatOK);
	}
	formatOK = false;

	toGuess = formatMAJ(toGuess);
}

/**
 * @brief Main interactive loop for player 2 where guesses are entered.
 *
 * This loop displays the current game state, prompts for guesses and
 * delegates verification to `verifPendu`.
 */
void Player2Turn(vector<string> &triedWrongWords, vector<string> &triedWrongLetters, vector<string> &triedCorrectLetters, vector<string> &healthBar, string &useranswer, string &toGuess, string &progress, string &remainingChars, int &errors, int &verif, int &stateFormat, bool &play, bool &formatOK)
{

	while (play)
	{

		tui.render.display("ascii_art/En/Font/Poison/Pendu_menu.txt", "Title", "\e[1;92m");

		// Appel processus vérification réponse Joueur 2
		verifPendu(triedWrongWords, triedWrongLetters, triedCorrectLetters, healthBar, useranswer, toGuess, progress, remainingChars, errors, verif);

		// Affichage de la barre de vie
		if (healthBar.empty())
		{
			healthBar.push_back("X");
		}
		cout << "Vies restantes : " << displayVector(healthBar, "", "", " ") << " (" << (10 - errors) << ")" << endl
			 << endl;

		// Affichage du tableau du pendu
		tui.render.display("ascii_art/En/Art/Design_Jeu_du_pendu.txt", to_string(errors), "\e[1;92m", "$$$");

		// Affichage vérification réponse Joueur 2
		displayVerifPendu(useranswer, toGuess, remainingChars, errors, verif, play);

		// Affichage progression jeu
		if (play)
		{

			cout << progress << endl;
			cout << remainingChars << endl
				 << endl;
			if (!triedWrongWords.empty())
			{
				cout << "Mauvaies réponses tentées : " << displayVector(triedWrongWords, "\"", "\"", ", ") << endl;
			}
			if (!triedWrongLetters.empty())
			{
				cout << "Mauvaises lettres tentées : " << displayVector(triedWrongLetters, "\"", "\"", ", ") << endl;
			}
			cout << "JOUEUR 2 | Saisissez une lettre ou tentez une réponse: ";
			getline(cin >> ws, useranswer);
			stateFormat = verifFormat(useranswer);
			displayVerifFomat(stateFormat, formatOK);

			// Boucle si format incorrect
			while (!formatOK)
			{

				cout << endl
					 << endl;
				cout << "JOUEUR 2 | Saisissez une lettre ou tentez une réponse: ";
				getline(cin >> ws, useranswer);
				stateFormat = verifFormat(useranswer);
				displayVerifFomat(stateFormat, formatOK);
			}
			formatOK = false;

			useranswer = formatMAJ(useranswer);
			tui.render.clearScreen();

		} // Fin if

	} // Fin while
}

/**
 * @brief Entrypoint for the hangman (Pendu) game used by the menu system.
 *
 * Performs initialisation and runs the two-player flow until completion.
 */
int PlayPendu()
{

	tui.render.clearScreen(); // Efface le contenu de la console

	// Définitions des variables : démarrage du jeu / Tour du Joueur 1
	string toGuess;

	int stateFormat;

	bool formatOK = false;
	bool play = true;

	Player1Turn(toGuess, stateFormat, formatOK);

	// Définition des variables : Tour du Joueur 2
	string progress(toGuess.length(), '-'); // Affichage d'autant de tirets que de caractères dans toGuess
	string remainingChars(toGuess.length(), '^');
	displayNonLetters(toGuess, progress, remainingChars);
	string useranswer;

	int errors = 0; // Variable nombre d'erreurs
	int verif = 0;	// Varibale cas associé à useranswer

	vector<string> triedWrongWords;
	vector<string> triedWrongLetters;
	vector<string> triedCorrectLetters;
	vector<string> healthBar(10, "♥");

	tui.render.clearScreen();

	Player2Turn(triedWrongWords, triedWrongLetters, triedCorrectLetters, healthBar, useranswer, toGuess, progress, remainingChars, errors, verif, stateFormat, play, formatOK);

	return 0;

} // Fin fonction PlayPendu

/**
 * @brief Settings menu placeholder for the pendu game.
 *
 * Currently a stub that clears the screen and waits for Enter.
 */
int SettingsPendu()
{
	tui.render.clearScreen(); // Clear console
	cout << "SettingsPendu executed!" << endl;
	cout << "Press Enter to return to the menu..." << endl;
	cin.ignore(); // To ignore any leftover newline character in the input buffer
	cin.get();	  // Wait for user to press Enter
	return 0;
}

/**
 * @brief Top-level menu for the pendu game exposing Play/Settings/Credits/Exit.
 */
void PenduMenu()
{
	int choice_Pendu;
	do
	{
		switch (choice_Pendu)
		{
		default:
		{
			tui.render.clearScreen(); // Clear console

			tui.render.display("ascii_art/En/Font/Poison/Pendu_menu.txt", "Menu", "\e[0;95m");
			cout << "Pendu Menu" << endl;
			cout << "1. Play" << endl;
			cout << "2. Settings" << endl;
			cout << "3. Credits" << endl;
			cout << "4. Exit" << endl;
			cout << "Enter your choice: ";
			secure_cin(choice_Pendu);
			break; // Prevent fallthrough
		}
		case 1:
		{
			choice_Pendu = PlayPendu();
			break;
		}
		case 2:
		{
			choice_Pendu = SettingsPendu();
			break;
		}
		case 3:
		{
			tui.render.clearScreen(); // Clear console
			cout << "Credits: Game developed by Comndamine Thibault." << endl;
			cout << "Press Enter to return to the menu..." << endl;
			cin.ignore();	  // To ignore any leftover newline character in the input buffer
			cin.get();		  // Wait for user to press Enter
			choice_Pendu = 0; // Return to main menu
			break;
		}
		case 4:
		{
			cout << "Exiting Pendu Menu." << endl;
			tui.render.erase();
			break;
		}
		}
	} while (choice_Pendu != 4);
	tui.render.clearScreen(); // Clear console
}

// int main() {
// 	PenduMenu();
// 	return 0;
// }

// Register the game in the menu system
/**
 * @brief Static registration helper to register the Pendu game at startup.
 */
struct registerPendu
{
	registerPendu()
	{
		addGame("Pendu", PenduMenu);
	}
};

static registerPendu registerGameInstance; // Static instance to register games at startup