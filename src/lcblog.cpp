/**
 * @file lcblog.cpp
 * @brief A logging class for handling log levels, formatting, and
 * time stamping within a C++ project.
 *
 * This logging class provides a flexible and thread-safe logging mechanism
 * with support for multiple log levels, timestamped logs, and customizable
 * output streams. include the header (`lcblog.hpp`), implementation
 * (`lcblog.cpp`), and template definitions (`lcblog.tpp`) when using in
 * a project.
 *
 * This software is distributed under the MIT License. See LICENSE.MIT.md for
 * details.
 *
 * Copyright (C) 2023-2025 Lee C. Bussy (@LBussy). All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "lcblog.hpp"
#include <algorithm>
#include <regex>
#include <iostream>

LCBLog llog;

/**
 * @brief Converts a log level to its string representation.
 *
 * @param level The log level to convert.
 * @return A string representing the log level.
 */
std::string logLevelToString(LogLevel level)
{
    switch (level)
    {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARN:
        return "WARN";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

/**
 * @brief Constructs the logging class with specified output streams.
 *
 * @param outStream The output stream for standard logs (default: std::cout).
 * @param errStream The output stream for error logs (default: std::cerr).
 */
LCBLog::LCBLog(std::ostream &outStream, std::ostream &errStream)
    : logLevel(INFO), out(outStream), err(errStream)
{
    out << std::unitbuf; // Ensure immediate flushing
    err << std::unitbuf;
}

/**
 * @brief Sets the minimum log level for message output.
 *
 * @param level The log level to set.
 */
void LCBLog::setLogLevel(LogLevel level)
{
    {
        std::lock_guard<std::mutex> lock(logMutex);
        logLevel = level;
    }

    // logS(level, "Log level changed to: ", logLevelToString(level));
}

/**
 * @brief Enables or disables time stamping for log messages.
 *
 * @param enable If true, timestamps will be included in logs.
 */
void LCBLog::enableTimestamps(bool enable)
{
    std::lock_guard<std::mutex> lock(logMutex);
    printTimestamps = enable;
}

/**
 * @brief Checks if a message should be logged based on the current log level.
 *
 * @param level The log level to check.
 * @return True if the message should be logged, otherwise false.
 */
bool LCBLog::shouldLog(LogLevel level) const
{
    return level >= logLevel;
}

/**
 * @brief Generates a timestamp string for log entries.
 *
 * @return A formatted timestamp string.
 */
std::string LCBLog::getStamp()
{
    using namespace std::chrono;
    // Get the current time_point
    auto now = system_clock::now();
    // Convert to time_t for formatting the date/time portion
    auto now_time_t = system_clock::to_time_t(now);
    // Get the number of milliseconds since the last whole second
    auto now_ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm;
    gmtime_r(&now_time_t, &tm);

    std::ostringstream oss;
    // Format the date and time
    oss << std::put_time(&tm, "%F %T");
    // Append the milliseconds and UTC label
    oss << '.' << std::setfill('0') << std::setw(3) << now_ms.count() << " UTC";

    return oss.str();
}

/**
 * @brief Cleans up a string by trimming whitespace and reducing consecutive spaces.
 *
 * @param s The string to sanitize.
 */
void LCBLog::crush(std::string &s)
{
    // Trim leading whitespace
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));

    // Trim trailing whitespace
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());

    // Reduce multiple spaces to a single space
    s.erase(std::unique(s.begin(), s.end(), [](char a, char b)
                        { return std::isspace(a) && std::isspace(b); }),
            s.end());
}

/**
 * @namespace (anonymous)
 * @brief Prevents unused function warnings by ensuring functions are referenced.
 *
 * This anonymous namespace contains a workaround to prevent `cppcheck`
 * from flagging certain functions as unused when they are intentionally
 * not directly called in the current translation unit.
 */
namespace
{

    /**
     * @brief Suppresses unused function warnings.
     *
     * @note This function is never intended to be executed, only referenced.
     */
    void suppress_unused_warnings()
    {
        (void)&LCBLog::setLogLevel;      ///< Explicitly reference function
        (void)&LCBLog::enableTimestamps; ///< Explicitly reference function
    }

    /**
     * @brief Forces `suppress_unused_warnings()` to be referenced.
     *
     * @note The variable is never actually used at runtime, ensuring zero impact.
     */
    static const auto _suppress = (suppress_unused_warnings(), 0);
}
