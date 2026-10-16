#pragma once

#include <sstream>
#include <string>

namespace bmi {
enum class LogLevel { Debug, Info, Warn, Error };

bool logOpen(const std::string& path, bool verbose);

void logWrite(LogLevel level, const char* file, int line, const std::string& msg);

}

#define LOG_AT(LEVEL, MSG)                                               \
    do {                                                                 \
        std::ostringstream log_os_;                                      \
        log_os_ << MSG;                                                  \
        ::bmi::logWrite(LEVEL, __FILE__, __LINE__, log_os_.str());       \
    } while (0)

#define LOG_DEBUG(MSG) LOG_AT(::bmi::LogLevel::Debug, MSG)
#define LOG_INFO(MSG)  LOG_AT(::bmi::LogLevel::Info, MSG)
#define LOG_WARN(MSG)  LOG_AT(::bmi::LogLevel::Warn, MSG)
#define LOG_ERROR(MSG) LOG_AT(::bmi::LogLevel::Error, MSG)
