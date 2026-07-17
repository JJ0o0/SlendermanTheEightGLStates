#pragma once

#include <INIReader.h>
#include <optional>

class ConfigFile {
    public:
        static void Load(const std::string& path);

        static bool GetBool(const std::string& section, const std::string& key, bool default_value);
    private:
        inline static std::optional<INIReader> s_reader;
        inline static bool s_loaded = false;
};