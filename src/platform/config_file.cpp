#include <platform/config_file.hpp>
#include <filesystem>
#include <fstream>

void ConfigFile::Load(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::ofstream file(path);
        file << "[Debug]\n";
        file << "DebugTools=false";
    }

    s_reader.emplace(INIReader(path));
    s_loaded = s_reader->ParseError() >= 0;
}

bool ConfigFile::GetBool(const std::string& section, const std::string& key, bool default_value) {
    if (!s_reader.has_value()) return default_value;
    return s_reader->GetBoolean(section, key, default_value);
}