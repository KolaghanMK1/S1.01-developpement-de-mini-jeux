/***********************************************
 * TUI-Engine::Prequel
 ***********************************************/

/***********************************************
 * Copyright (c) 2025 Bardoux Théo
 ***********************************************/
#include <iostream>
#include "TUI-Engine--Prequel.h"
#ifdef _WIN32
#include <windows.h>
#endif

bool TUI_Core::KeyStructure::operator==(const KeyStructure &other) const
{
    return file == other.file && section == other.section;
}

std::size_t TUI_Core::KeyStructureHash::operator()(const KeyStructure &key) const noexcept // hash function for the structur KeyStructure
{
    std::hash<std::string> hasher;
    size_t h1 = hasher(key.file);
    size_t h2 = hasher(key.section);
    return h1 ^ (h2 * 0x9e3779b9);
}

/**
 * @brief Load a text file and optionally extract a named section.
 *
 * The file may contain sections delimited by the `section_indic` marker. Each
 * section is stored as a vector of lines and cached under the pair
 * (filename, section). If `section` is empty the entire file is accumulated
 * into the default section.
 *
 * @param filename Path to the file to read.
 * @param section Name of the section to extract (empty = whole file).
 * @param section_indic Marker string used to indicate sections in the file.
 * @return true if the file was parsed and stored in the cache.
 */
bool TUI_Core::TUI_Cache::load(const std::string &filename, const std::string &section, const std::string &section_indic)
{
    std::ifstream file(filename);
    if (!file.is_open()) // if file is not open
    {
        std::cerr << filename << " is not found." << std::endl;
        return false;
    }

    KeyStructure key;
    key.file = filename;
    std::string currentSection = "";
    std::string line;
    size_t start;
    size_t end;
    std::vector<std::string> to_display;
    while (getline(file, line)) // while a complete line is available
    {
        if (line.find(section_indic) != std::string::npos) // detect a new section
        {
            start = line.find(section_indic) + section_indic.length();
            end = line.find(section_indic, start);
            if (end != std::string::npos)
                currentSection = line.substr(start, end - start); // if the section declaration has been closed
            else
                currentSection = line.substr(start);

            // if we had already accumulated a section
            if (!to_display.empty())
            {
                if (key.section.empty())
                {
                    key.section = "default";
                }
                if (Cache.find(key) == Cache.end())
                    Cache.insert({key, to_display});
                else
                {
                    for (std::string &i : to_display)
                    {
                        Cache.at(key).push_back(i);
                    }
                }
                to_display.clear();
            }
            key.section = currentSection;
            // if we are looking for a specific section and we have found it
            if (!section.empty() && currentSection == section)
            {
                to_display.clear();
            }
            continue;
        }
        // recording of lines
        if (section.empty())
            to_display.push_back(line);
        else if (currentSection == section)
            to_display.push_back(line);
    } // end of while loop

    if (!to_display.empty())
    {
        if (key.section.empty())
        {
            key.section = "default";
        }
        if (Cache.find(key) == Cache.end())
            Cache.insert({key, to_display});
        else
        {
            for (std::string &i : to_display)
            {
                Cache.at(key).push_back(i);
            }
        }
    }
    return true;
} // end of load method

/**
 * @brief Check if a cached entry exists for the given key.
 *
 * @param key KeyStructure identifying the cached file/section pair
 * @return true if the entry exists in the cache
 */
bool TUI_Core::TUI_Cache::is_loaded(const KeyStructure &key)
{
    return Cache.find(key) != Cache.end();
}

std::vector<std::string> TUI_Core::TUI_Cache::read(const KeyStructure &key)
{
    if (!is_loaded(key))
        return std::vector<std::string>();
    return Cache.at(key);
}

/**
 * @brief Retrieve the cached lines for a specific filename/section.
 *
 * This convenience wrapper builds a KeyStructure and returns the cached
 * vector of lines if present; otherwise an empty vector is returned.
 */
std::vector<std::string> TUI_Core::TUI_Cache::read(const std::string &filename, const std::string &sectionName)
{
    KeyStructure key;
    key.file = filename;
    key.section = sectionName;
    if (!is_loaded(key))
        return std::vector<std::string>();
    return Cache.at(key);
}


/**
 * @brief Remove cached entries.
 *
 * If both `file` and `section` are empty the whole cache is cleared. If only
 * `file` is provided all sections for that file are removed. If both are
 * supplied only the matching section is erased.
 */
void TUI_Core::TUI_Cache::erase(std::string file, std::string section)
{
    KeyStructure key;
    key.file = file;
    key.section = section;

    if ((key.file.empty()) && (key.section.empty()))
        Cache.clear(); // check if any file and section has been given
    else if (!key.file.empty())
    {
        std::vector<KeyStructure> keysVect;
        for (const auto &kv : Cache) // const auto& kv : Cache --> assigns a key/value pair to the variable kv
        {
            if (kv.first.file == key.file)
                keysVect.push_back(kv.first); // kv.first --> takes only the key
        }
        if (section.empty())
        {
            for (KeyStructure keys : keysVect)
            {
                Cache.erase(keys);
            }
        }
        else
        {
            for (KeyStructure keys : keysVect)
            {
                if (keys.section == section)
                    Cache.erase(keys);
            }
        }
    }
    else
        std::cerr << "Please, give a file for erase a section." << std::endl;
}

// constructeur interne à TUI_Render
TUI_Core::TUI_Render::TUI_Render(TUI_Cache &cacheInstance) : cache(cacheInstance) {}


/**
 * @brief Preload a file or section into the cache.
 *
 * @see TUI_Core::TUI_Cache::load
 */
void TUI_Core::TUI_Render::preload(const std::string &filename, const std::string &section, const std::string &section_indic)
{
    cache.load(filename, section, section_indic);
}

/**
 * @brief Display a cached file/section to stdout using an optional color.
 *
 * If the requested entry is not yet loaded it will be loaded automatically.
 */
void TUI_Core::TUI_Render::display(const std::string &filename, const std::string &sectionName, const std::string &color, const std::string &section_indic)
{
    KeyStructure key;
    key.file = filename;
    key.section = sectionName;
    if (!cache.is_loaded(key))
        cache.load(filename, sectionName, section_indic);
    std::vector<std::string> to_display = cache.read(key);
    std::cout << color;
    for (const auto &line : to_display)
    {
        std::cout << line << std::endl;
    }
    std::cout << "\e[0m";
} // End of display

/**
 * @brief Display multiple cached sections horizontally across columns.
 *
 * Each specified section will be displayed as a column. Shorter columns are
 * padded by skipping lines when printing the row.
 */
void TUI_Core::TUI_Render::horizontal_display(const std::string &filename, const std::vector<std::string> &sectionName)
{
    // Collect each section as a column (vector of lines)
    std::vector<std::vector<std::string>> columns;
    for (const std::string &sec : sectionName)
    {
        if (!sec.empty())
            columns.push_back(cache.read(filename, sec));
        else
            columns.emplace_back();
    }

    // Find the tallest row
    size_t max_size = 0;
    for (const auto &col : columns)
        if (col.size() > max_size)
            max_size = col.size();

    // Print rows horizontally across columns
    for (size_t r = 0; r < max_size; ++r)
    {
        for (size_t c = 0; c < columns.size(); ++c)
        {
            if (r < columns[c].size())
                std::cout << columns[c][r];
            if (c + 1 < columns.size())
                std::cout << "   ";
        }
        std::cout << std::endl;
    }
}

void TUI_Core::TUI_Render::erase(const std::string &file, const std::string &section)
{
    cache.erase(file, section);
}

std::vector<std::string> TUI_Core::TUI_Render::get(const std::string &filename, const std::string &sectionName)
{
    KeyStructure key;
    key.file = filename;
    key.section = sectionName;
    if (!cache.is_loaded(key))
        cache.load(filename, sectionName);
    return cache.read(filename, sectionName);
}



/**
 * @brief Clear the terminal screen (platform specific).
 */
void TUI_Core::TUI_Render::clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

// constructeur public de TUI_Core
TUI_Core::TUI_Core() : cache(), render(cache) {}