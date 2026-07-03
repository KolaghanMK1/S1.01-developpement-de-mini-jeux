#ifdef _WIN32
#include <windows.h>
#include <clocale>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

/**
 * @file Set_windows_settings.cpp
 * @brief Implementation of helpers to configure the Windows console.
 */

/**
 * @brief Enable support for ANSI escape sequences on Windows consoles.
 *
 * Attempts to set the `ENABLE_VIRTUAL_TERMINAL_PROCESSING` flag in the
 * console mode so that ANSI color sequences and cursor control sequences are
 * processed. If the operation fails this function silently returns.
 */
void EnableANSIColors()
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // Get the standard output handle
    if (hOut != INVALID_HANDLE_VALUE)              // Check if the handle is valid
    {
        DWORD dwMode = 0;                  // Variable to hold the console mode
        if (GetConsoleMode(hOut, &dwMode)) // Get the current console mode
        {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // Enable ANSI escape codes
            SetConsoleMode(hOut, dwMode);                 // Set the new console mode
        }
    }
#endif
}

/**
 * @brief Set the console input/output code pages to UTF-8 on Windows.
 *
 * This function calls the Win32 API to set the console code pages and also
 * sets the C runtime locale to UTF-8 so that UTF-8 encoded text is handled
 * correctly by standard library I/O functions.
 */
void EnableUTF8()
{

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
#endif
}