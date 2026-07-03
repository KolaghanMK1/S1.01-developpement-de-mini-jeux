#ifndef SECURE_CIN_H
#define SECURE_CIN_H

#pragma once
#include <iostream>
#include <limits>
#include <string>

/**
 * @file secure_cin.hpp
 * @brief Small utility to perform safe std::cin reads with optional retry behavior.
 *
 * The functions in this header provide a simple wrapper around `std::cin` to
 * detect failed formatted extractions and optionally prompt the user to
 * retry until a valid value is entered.
 */

/**
 * @brief Global flag controlling whether `secure_cin` should prompt for retries.
 *
 * When true the `secure_cin` template will repeatedly ask the user for input
 * until a valid value is read. When false, `secure_cin` returns immediately on
 * input failure leaving the variable unmodified.
 */
inline bool secure_cin_retry = true;

/**
 * @brief Set the global retry behavior for `secure_cin`.
 *
 * @param val If true then `secure_cin` will repeatedly prompt on invalid input.
 */
inline void secure_cin_retry_set(bool val = true)
{
    secure_cin_retry = val;
}

/**
 * @brief Read a value from `std::cin` with error checking and optional retry.
 *
 * This template attempts to extract a value of type `T` from `std::cin` using
 * the usual formatted extraction operator. If the extraction fails the function
 * clears the error state and discards the rest of the line. If the global
 * `secure_cin_retry` flag is set the function will prompt the user and retry
 * until a valid value is provided; otherwise it returns immediately.
 *
 * @tparam T Type to read (must be extractable from `std::istream`).
 * @param var Reference to the variable where the parsed value will be stored.
 */
template <typename T>
inline void secure_cin(T &var)
{
    std::cin >> var; // Try to read input
    while (std::cin.fail())
    {
        std::cin.clear();                                                   // Clear the error flag on cin
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        if (secure_cin_retry)
        {
            std::cout << "Invalid input. Please enter a valid value: "; // Prompt user for valid input
            std::cin >> var;                                            // Try to read input again
        }
        else
        {
            return;
        }
    }
}

#endif // SECURE_CIN_H