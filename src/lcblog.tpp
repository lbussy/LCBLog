/**
 * @file lcblog.tpp
 * @brief A logging class for handling log levels, formatting, and
 * timestamping within a C++ project.
 *
 * This logging class provides a flexible and thread-safe logging mechanism
 * with support for multiple log levels, timestamped logs, and customizable
 * output streams. include the header (`lcblog.hpp`), implementation
 * (`lcblog.cpp`), and template definitions (`lcblog.tpp`)when using in
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

#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

/**
 * @brief Logs a message at a given log level.
 *
 * @tparam T The first type of the message content.
 * @tparam Args Variadic template arguments for additional message content.
 * @param level The log level of the message.
 * @param stream The output stream to write the log message to.
 * @param t The first message content.
 * @param args Additional message content.
 */
template <typename T, typename... Args>
void LCBLog::log(LogLevel level, std::ostream& stream, T t, Args... args) {
    if (!shouldLog(level)) return;

    std::lock_guard<std::mutex> lock(logMutex);
    logToStream(stream, level, t, args...);
}

/**
 * @brief Logs a formatted message to a specified stream.
 *
 * Processes multi-line messages, applies optional timestamping, and formats
 * the output with log level tags.
 *
 * @tparam T The first type of the message content.
 * @tparam Args Variadic template arguments for additional message content.
 * @param stream The output stream to write the log message to.
 * @param level The log level of the message.
 * @param t The first message content.
 * @param args Additional message content.
 */
template <typename T, typename... Args>
void LCBLog::logToStream(std::ostream& stream, LogLevel level, T t, Args... args) {
    std::ostringstream oss;
    oss << t;  // First argument is directly added

    // Append additional arguments while handling spacing correctly
    if constexpr (sizeof...(args) > 0) {
        const auto appendArgs = [&](const auto& prev, const auto& first, const auto&... rest) {
            oss << (shouldSkipSpace(prev, first) ? "" : " ") << first;
            ((oss << (shouldSkipSpace(first, rest) ? "" : " ") << rest), ...);
        };

        appendArgs(t, args...);
    }

    std::string logMessage = oss.str();
    std::istringstream messageStream(logMessage);
    std::string line;
    bool firstLine = true;

    constexpr int LOG_LEVEL_WIDTH = 5;  // Maximum length of log level names (DEBUG, ERROR, FATAL = 5)
    std::string levelStr = logLevelToString(level);
    levelStr.append(LOG_LEVEL_WIDTH - levelStr.size(), ' ');  // Pad to align all levels

    while (std::getline(messageStream, line)) {
        if (!firstLine) stream << std::endl;

        crush(line);  // Remove excessive whitespace
        if (printTimestamps) stream << getStamp() << "\t";

        stream << "[" << levelStr << "] " << line;
        firstLine = false;
    }
    stream << std::endl;
}

/**
 * @brief Determines if a space should be adjusted around a given argument.
 *
 * Ensures correct spacing after colons and prevents spaces before punctuation.
 *
 * @param prevArg The previous argument (for trailing spaces check).
 * @param arg The current argument.
 * @return True if no space should be added before `arg`, false if space is needed.
 */
template <typename PrevT, typename T>
bool shouldSkipSpace(const PrevT& prevArg, const T& arg) {
    std::string prevStr;
    std::string argStr;

    // Convert previous argument to string (if possible)
    if constexpr (std::is_convertible_v<PrevT, std::string>) {
        prevStr = std::string(prevArg);
    }

    // Convert current argument to string (if possible)
    if constexpr (std::is_convertible_v<T, std::string>) {
        argStr = std::string(arg);
    }

    // Remove space *before* punctuation marks and parentheses
    if (argStr.size() == 1 && (argStr == "." || argStr == "," || argStr == ";" || argStr == "!" ||
                               argStr == ")" || argStr == "]" || argStr == "}")) {
        return true;  // No space before these characters
    }

    // Prevent a space *after* `(`, `[` or `{` by checking the last character of prevStr
    if (!prevStr.empty() && (prevStr.back() == '(' || prevStr.back() == '[' || prevStr.back() == '{')) {
        return true;  // No space after opening parenthesis
    }

    return false;
}
