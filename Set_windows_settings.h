#pragma once
/**
 * @file Set_windows_settings.h
 * @brief Helpers to enable console features on Windows (ANSI colors and UTF-8).
 */

/**
 * @brief Enable ANSI escape sequence processing for the Windows console.
 *
 * On Windows this attempts to set the console mode flag
 * `ENABLE_VIRTUAL_TERMINAL_PROCESSING` so programs can use ANSI color codes.
 */
void EnableANSIColors();

/**
 * @brief Configure the console to use UTF-8 for input/output on Windows.
 */
void EnableUTF8();