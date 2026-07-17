#include <platform/system_info.hpp>
#include <glad/gl.h>
#include <fstream>
#include <string>

#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#endif

void SystemInfo::Initialize() {
    s_gpuVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    s_gpuRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    s_driverVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
}

size_t SystemInfo::GetMemoryUsage() {
    #ifdef _WIN32
        PROCESS_MEMORY_COUNTERS_EX pmc;

        if (GetProcessMemoryInfo(
            GetCurrentProcess(),
            reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
            sizeof(pmc))) {
            return pmc.WorkingSetSize;
        }
    #elif defined(__linux__)
        std::ifstream file("/proc/self/status");
        std::string line;

        while (std::getline(file, line)) {
            if (line.starts_with("VmRSS:")) {
                return std::stoull(line.substr(6)) * 1024;
            }
        }
    #endif

    return 0;
}