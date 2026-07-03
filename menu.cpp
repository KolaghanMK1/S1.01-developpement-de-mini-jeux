#include<iostream>
#include<limits>
#include"menu.h"
#include"TUI-Engine--Prequel.h"
#include"secure_cin.hpp"
#include"Set_windows_settings.h"

using namespace std;
TUI_Core MainMenu;

//Internal functions: keeps a single list of games (static ensure it's created once)
vector<game>& getGameList() 
{
    static vector<game> games; // Static variable to hold the list of games
    return games;
}



// Function to add a new game to the list
void addGame(const string& name, function<void()> launchFunction) 
{
    getGameList().push_back(game(name, launchFunction));
}



// Function to list avaible games and handle game selection
void listGames(int& choice_main)
{
    int choice_game;
    auto& games = getGameList(); // Retrieve the list of games
    if (games.empty())
    {
        MainMenu.render.display("ascii_art/En/Font/Alpha/Menu.txt", "Games", "\033[1;31m"); // Display the games section from Menu_ascii.txt in yellow color
        cout << "No games available.\n";
        cout << "Press any key to return to the main menu...";
        cin.ignore(); // Clear any leftover input
        cin.get();    // Wait for user to press a key
        choice_main = 0; // Return to main menu
    }
    else
    {
        while(!games.empty()) // Loop until the user chooses to return to the main menu
        {
            MainMenu.render.display("ascii_art/En/Font/Alpha/Menu.txt", "Games", "\033[1;31m"); // Display the games section from Menu_ascii.txt in yellow color
            for (size_t i = 0; i < games.size(); ++i)
            {
                cout << i + 1 << ". " << games[i].name << "\n"; // List available games
            }
            cout << "0. Main menu\n";      
            cout << "Select a game to play: ";
            secure_cin(choice_game);
            if (choice_game == 0)
            {
                choice_main = 0; // Return to main menu
                break;
            }
            else if (choice_game > 0 && choice_game <= static_cast<int>(games.size())) // static_cast<int>(...) convert size_t to int for safe comparison
            {
                games[choice_game - 1].launchFunction(); // Launch the selected game
            }
            else
            {
                cout << "Invalid choice. Please try again.\n";
            } //fin else
        } //fin while
    } //fin else
} // End of listGames function


void Menu(int& choice)
{
    MainMenu.render.clearScreen(); // Clear console
    MainMenu.render.display("ascii_art/En/Font/Alpha/Menu.txt", "Menu", "\033[1;33m"); // Display the menu section from Menu_ascii.txt in red color
    MainMenu.render.display("ascii_art/En/Font/Chunky/Menu_choices.txt","1. Play","\033[1;31m");
    MainMenu.render.display("ascii_art/En/Font/Chunky/Menu_choices.txt","2. Settings","\033[1;32m");
    MainMenu.render.display("ascii_art/En/Font/Chunky/Menu_choices.txt","3. Credits","\033[1;34m");
    MainMenu.render.display("ascii_art/En/Font/Chunky/Menu_choices.txt","4. Exit","\033[1;37m");
    cout << endl;
    cout << "Select an option: ";
    secure_cin(choice);
}


// Display settings menu and get the user's choice
int settings()
{
    int choice = -1;
    do
    {
        MainMenu.render.clearScreen(); // Clear console
        MainMenu.render.display("ascii_art/En/Font/Alpha/Menu.txt", "Settings", "\033[1;32m"); // Display the settings section from Menu_ascii.txt in green color
        cout << "0. Main menu\n";
        secure_cin(choice);
    } while (choice != 0);
    return 0;
}


// Display credits and wait for user to press enter
int credits() {
    MainMenu.render.clearScreen(); // Clear console
    MainMenu.render.display("ascii_art/En/Font/Alpha/Menu.txt", "Credits", "\033[1;34m"); // Display the credits section from Menu_ascii.txt in blue color
    cout << "Game developed by Maelstrom studio\nLeading developer: Bardoux Theo\nDevelopers: Condamine Thibault; Morin Warren\n";
    cout << "Press enter to exit...";
    cin.ignore();
    cin.get();
    return 0;
}



void mainMenu()
// Displays the main menu and handles user input
{
    int choice_main=0;
    MainMenu.render.clearScreen();
    do   
    {
        switch(choice_main)
        {
            default: // Main menu 
            {
                Menu(choice_main); // Display the main menu and get the user's choice
                break;
            }

            case 1: // Play
            {
                MainMenu.render.clearScreen(); // Clear console
                listGames(choice_main); //List avaible games and handle game selection
                break;
            }
            
            case 2: // Settings
            {
                choice_main = settings(); // Display settings menu and get the user's choice
                break;
            }
            
            case 3: // Credits
            {
                choice_main = credits(); // Display credits and wait for user to press enter
                break;
            }
            
            case 4: // Exit
            {
                cout << "Exiting the game\n";
                break;
            }
        }
    } while (choice_main != 4);
    MainMenu.render.clearScreen(); // Clear console
} // End of Menu function


int main()
{
    EnableANSIColors(); // Enable ANSI colors for Windows
    EnableUTF8(); // Enable UTF-8 encoding for Windows
    mainMenu();
    return 0;
}