/***********************************************
 * TUI-Engine::Prequel - Header File
 ***********************************************/

/***********************************************
 * Copyright (c) 2025 Kolaghan_MK1
 ***********************************************/
#pragma once

#ifndef TUI_CORE_H
#define TUI_CORE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

class TUI_Core
{
private:
    /**
     * @brief Key used to identify cached sections inside a file.
     */
    struct KeyStructure
    {
        std::string file;
        std::string section = "";

        /**
         * @brief Equality operator for key comparisons used by unordered_map.
         */
        bool operator==(const KeyStructure &other) const;
    };

    /**
     * @brief Hash functor for KeyStructure so it can be used in unordered_map.
     */
    struct KeyStructureHash
    {
        std::size_t operator()(const KeyStructure &key) const noexcept; // const noexept --> permet à la méthode d'agir comme une fonction = fonction objet = foncteur
    };

    /**
     * @brief Simple file cache storing named sections read from text files.
     *
     * The cache maps (filename, section) -> vector<string> where each string is
     * a line from the corresponding section.
     */
    class TUI_Cache
    {
    private:
        std::unordered_map<KeyStructure, std::vector<std::string>, KeyStructureHash> Cache;

    public:
        /**
         * @brief Load a file and optionally extract a specific section.
         *
         * @param filename Path to the file to read.
         * @param section Section name to extract (default: empty => whole file).
         * @param section_indic Marker used to separate sections in the file.
         * @return true on success.
         */
        bool load(const std::string &filename, const std::string &section = "", const std::string &section_indic = "===");

        /**
         * @brief Check if a given key is already loaded in the cache.
         */
        bool is_loaded(const KeyStructure &key);

        /**
         * @brief Read a cached entry by key.
         */
        std::vector<std::string> read(const KeyStructure &key);

        /**
         * @brief Read a cached entry by filename and section name.
         */
        std::vector<std::string> read(const std::string &filename, const std::string &sectionName);

        /**
         * @brief Erase a cached file or a specific section.
         */
        void erase(std::string file = "", std::string section = "");
    };

private:
    TUI_Cache cache;

public:
    /**
     * @brief Rendering helpers that use the cache to display text-based UIs.
     */
    class TUI_Render
    {
    private:
        TUI_Cache &cache;

    public:
        /**
         * @brief Construct a renderer associated with a cache instance.
         */
        explicit TUI_Render(TUI_Cache &cacheInstance);

        /**
         * @brief Preload a file or section into the cache.
         */
        void preload(const std::string &filename, const std::string &section = "", const std::string &section_indic = "===");

        /**
         * @brief Return the lines associated to a (file,section) pair.
         */
        std::vector<std::string> get(const std::string &filename, const std::string &sectionName);

        /**
         * @brief Display a cached section to stdout with optional color.
         */
        void display(const std::string &filename, const std::string &sectionName = "", const std::string &color = "\033[0m", const std::string &section_indic = "===");

        /**
         * @brief Display multiple sections side-by-side horizontally.
         */
        void horizontal_display(const std::string &filename, const std::vector<std::string> &section_name);

        /**
         * @brief Erase cached content for a file or section.
         */
        void erase(const std::string &file = "", const std::string &section = "");

        /**
         * @brief Clear the terminal screen.
         */
        void clearScreen();
    };

    TUI_Render render;

public:
    /**
     * @brief Construct the TUI_Core instance and its render helper.
     */
    TUI_Core(); // constructeur
};

#endif // TUI_CORE_H