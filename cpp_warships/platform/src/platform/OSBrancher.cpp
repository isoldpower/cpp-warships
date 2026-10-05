#include <platform/OSBrancher.h>

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace cpp_warships::platform {
    bool isOutputTerminal() {
#ifdef _WIN32
        return _isatty(_fileno(stdout)) != 0;
#else
        return isatty(STDOUT_FILENO) != 0;
#endif
    }

    std::tm localTimeOf(const std::time_t moment) {
        std::tm broken{};

#ifdef _WIN32
        localtime_s(&broken, &moment);
#else
        localtime_r(&moment, &broken);
#endif

        return broken;
    }

    std::optional<std::string> environmentValue(const char* name) {
#ifdef _WIN32
        char* value = nullptr;
        std::size_t length = 0;
        if (_dupenv_s(&value, &length, name) != 0 || value == nullptr) {
            return std::nullopt;
        }

        std::string found{value};
        std::free(value);

        return found;
#else
        const char* value = std::getenv(name);
        if (value == nullptr) {
            return std::nullopt;
        }

        return std::string{value};
#endif
    }

    std::optional<std::string> homeDirectory() {
        if (const std::optional<std::string> home = environmentValue("HOME")) {
            return home;
        }

        return environmentValue("USERPROFILE");
    }

    bool hostTranslatesKeyboardLayouts() {
#ifdef __EMSCRIPTEN__
        return true;
#else
        return false;
#endif
    }

    void prepareConsoleForUnicode() {
#ifdef _WIN32
        std::system("chcp 65001");
        SetConsoleOutputCP(CP_UTF8);
#endif
    }
}  // namespace cpp_warships::platform
