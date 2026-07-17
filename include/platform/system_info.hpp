#pragma once

#include <cstddef>
#include <string>

class SystemInfo {
    public:
        static void Initialize();

        static size_t GetMemoryUsage();

        static const std::string& GetGPUVendor() { return s_gpuVendor; }
        static const std::string& GetGPURenderer() { return s_gpuRenderer; }
        static const std::string& GetDriverVersion() { return s_driverVersion; }
    private:
        static inline std::string s_gpuVendor;
        static inline std::string s_gpuRenderer;
        static inline std::string s_driverVersion;
};