#pragma once

#include <format>
#include <string>

class ErrorHandling {
    public:
        static void ShowError(const std::string& title, const std::string& message);

        template<typename... Args>
        static void ShowError(
            const std::string& title,
            std::format_string<Args...> fmt,
            Args&&... args
        ) {
            ShowError(title, std::format(fmt, std::forward<Args>(args)...));
        }
};