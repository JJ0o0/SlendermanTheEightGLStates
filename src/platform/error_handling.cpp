#include <platform/error_handling.hpp>
#include <tinyfiledialogs/tinyfiledialogs.h>

void ErrorHandling::ShowError(const std::string &title, const std::string &message) {
    tinyfd_messageBox(
        title.c_str(),
        message.c_str(),
        "ok",
        "error",
        1
    );
}