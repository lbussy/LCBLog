/**
 * @file lcblog.tpp
 * @brief A logging class for handling log levels, formatting, and
 * time stamping within a C++ project.
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
void LCBLog::log(LogLevel level, std::ostream &stream, T t, Args... args)
{
    if (!shouldLog(level))
        return;

    // Format the log message first (reduces time holding the lock)
    std::ostringstream formattedStream;
    logToStream(formattedStream, level, t, args...);

    {
        // Lock only while writing to the actual output stream
        std::lock_guard<std::mutex> lock(logMutex);
        stream << formattedStream.str();
    }
}

/**
 * @brief Logs a formatted message to a specified stream.
 *
 * Processes multi-line messages, applies optional time stamping, and formats
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
void LCBLog::logToStream(std::ostream &stream, LogLevel level, T t, Args... args)
{
    // Helper lambda to convert any streamable type to a string with desired formatting.
    auto toString = [](const auto &val) -> std::string {
        std::ostringstream oss;
        if constexpr (std::is_floating_point_v<std::decay_t<decltype(val)>>) {
            // Check if the value is exactly an integer.
            if (val == static_cast<long long>(val)) {
                // For integer values like 0.0, force one decimal digit (e.g. "0.0")
                oss << std::showpoint << std::fixed << std::setprecision(1);
            }
            // Otherwise, let the stream use its default formatting.
        }
        oss << val;
        return oss.str();
    };

    std::ostringstream oss;
    // Convert and print the first argument.
    std::string prevStr = toString(t);
    oss << prevStr;

    if constexpr (sizeof...(args) > 0)
    {
        const auto appendArgs = [&](const auto &prev, const auto &first, const auto &...rest)
        {
            std::string prevS = toString(prev);
            std::string firstS = toString(first);
            // Use the string versions for spacing logic.
            oss << (shouldSkipSpace(prevS, firstS) ? "" : " ") << firstS;
            ((oss << (shouldSkipSpace(firstS, toString(rest)) ? "" : " ") << toString(rest)), ...);
        };

        appendArgs(t, args...);
    }

    std::string logMessage = oss.str();
    std::istringstream messageStream(logMessage);
    std::string line;
    bool firstLine = true;

    constexpr int LOG_LEVEL_WIDTH = 5; // e.g., "INFO " is padded to 5 characters.
    std::string levelStr = logLevelToString(level);
    levelStr.append(LOG_LEVEL_WIDTH - levelStr.size(), ' ');

    while (std::getline(messageStream, line))
    {
        if (!firstLine)
            stream << std::endl;

        crush(line); // Clean up whitespace.
        if (printTimestamps)
            stream << getStamp() << "\t";

        stream << "[" << levelStr << "] " << line;
        firstLine = false;
    }
    stream << std::endl;
    stream.flush();
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
bool shouldSkipSpace(const PrevT &prevArg, const T &arg)
{
    auto toString = [](const auto &val) -> std::string {
        std::ostringstream oss;
        if constexpr (std::is_floating_point_v<std::decay_t<decltype(val)>>) {
            // If the floating-point value is an integer value, force one decimal.
            if (val == static_cast<long long>(val))
                oss << std::showpoint << std::fixed << std::setprecision(1);
            else
                oss << std::showpoint;
        }
        oss << val;
        return oss.str();
    };

    std::string prevStr = toString(prevArg);
    std::string currStr = toString(arg);

    // If either string is empty, do not insert an extra space.
    if (prevStr.empty() || currStr.empty())
        return true;

    char last = prevStr.back();
    char first = currStr.front();

    // If the previous string already ends with whitespace, do nothing.
    if (std::isspace(static_cast<unsigned char>(last)))
        return true;

    // If the previous segment ends with an opening punctuation, skip adding a space.
    if (last == '(' || last == '[' || last == '{')
        return true;

    // If the current segment begins with a closing punctuation, skip adding a space.
    if (first == ')' || first == ']' || first == '}')
        return true;

    // Otherwise, if the last character is alphanumeric or punctuation, we want a space.
    if (std::isalnum(static_cast<unsigned char>(last)) || std::ispunct(static_cast<unsigned char>(last)))
        return false;

    return true;
}
