#include <string>
#include <filesystem>
#include <ThING/extras/handMade.h>
#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #undef max
    #undef min
    std::string osd::getExecutableDir() {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        return std::filesystem::path(path).parent_path().string();
    }
#elif defined(__linux__)
    #include <unistd.h>
    std::string osd::getExecutableDir() {
        char path[4096];
        ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
        if (count == -1) {
            throw std::runtime_error("cannot determine executable path");
        }
        return std::filesystem::path(std::string(path, count)).parent_path().string();
    }
#else
    #error "Unsupported platform"
#endif

std::random_device rd;
std::mt19937 rng(rd());