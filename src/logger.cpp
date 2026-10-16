#include "logger.hpp"

#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>

namespace bmi {
namespace {
std::ofstream g_file;
bool g_verbose = false;

const char* levelName(LogLevel level) {
    switch (level) {
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO ";
    case LogLevel::Warn:  return "WARN ";
    case LogLevel::Error: return "ERROR";
    }
    return "?????";
}

}

bool logOpen(const std::string& path, bool verbose) {
    g_verbose = verbose;
    g_file.open(path, std::ios::app);
    return g_file.is_open();
}

void logWrite(LogLevel level, const char* file, int line, const std::string& msg) {
    if (level == LogLevel::Debug && !g_verbose) return;
    if (!g_file.is_open() && !g_verbose) return;

    char time[32];
    const std::time_t now = std::time(nullptr);
    std::strftime(time, sizeof time, "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    const char* slash = std::strrchr(file, '/');
    const char* base = slash ? slash + 1 : file;

    std::ostringstream os;
    os << time << " [" << levelName(level) << "] " << base << ':' << line << "  " << msg;

    if (g_file.is_open()) g_file << os.str() << std::endl;
    if (g_verbose) std::cerr << os.str() << '\n';
}

}
