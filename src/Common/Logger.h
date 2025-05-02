#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define MAX_BUFFER_SIZE 256

#define TEXT(format, ...)                                                                                    \
    [](const char *fmt, auto... args) {                                                                      \
        char buffer[MAX_BUFFER_SIZE];                                                                        \
        snprintf(buffer, sizeof(buffer), fmt, args...);                                                      \
        return std::string(buffer);                                                                          \
    }(format, __VA_ARGS__)

#define LOG(severity, message) jlang::Logger::log(severity, message, __FILE__, __LINE__)

#define JNETWORK_DEBUG(message) LOG("DEBUG", message)
#define JNETWORK_INFO(message) LOG("INFO", message)
#define JNETWORK_WARN(message) LOG("WARN", message)
#define JENTWORK_ERROR(message) LOG("ERROR", message)

namespace jlang
{

class Logger
{
  public:
    Logger() = delete;

    static void log(const std::string &severity, const std::string &message, const char *file,
                    uint32_t lineNumber)
    {
        std::ofstream logfile("C:\\Users\\josip\\Desktop\\Jlang\\log.txt", std::ios::app);

        if (!logfile.is_open())
        {
            std::cerr << "No can do for opening log file!" << std::endl;
            return;
        }

        putLogMessage(logfile, message, severity, lineNumber, file);

        logfile.close();
    }

  private:
    static void putLogMessage(std::ofstream &log, const std::string &message, const std::string &severity,
                              uint32_t lineNumber, const char *file)
    {
        log << "[";
        log << "] ";

        std::ostringstream location;
        location << "[" << file << ":" << lineNumber << "]";

        log << location.str();

        size_t padding = 74 - location.str().length();
        if (padding > 0)
        {
            log << std::setw(padding) << " ";
        }

        log << severity << ": " << message << std::endl;
    }
};

} // namespace jlang