/**
 * @file main.cpp
 * @brief Test framework for LCBLog, a logging class for handling log levels,
 * formatting, and time stamping within a C++ project.
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

#include "lcblog.hpp"
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>

void threadSafetyTest()
{
    std::cout << "Testing thread safety..." << std::endl;

    // Lambda function to log in multiple threads
    auto logTask = [](int threadId)
    {
        for (int i = 0; i < 5; ++i)
        {
            llog.logS(INFO, "Thread " + std::to_string(threadId) + " logging message " + std::to_string(i));
        }
    };

    // Create multiple threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i)
    {
        threads.push_back(std::thread(logTask, i));
    }

    // Wait for all threads to complete
    for (auto &t : threads)
    {
        t.join();
    }

    std::cout << "Thread safety test completed." << std::endl;
}

void logToDifferentStreamsTest()
{
    std::cout << "Testing log output to stdout and stderr..." << std::endl;

    // Log to stdout
    llog.logS(INFO, "This is an INFO message to stdout.");

    // Log to stderr
    llog.logE(ERROR, "This is an ERROR message to stderr.");

    std::cout << "Log output test completed." << std::endl;
}

void runLogLevelFilteringTests()
{
    std::cout << "Testing log level filtering..." << std::endl;

    // Set log level to DEBUG (lowest level)
    llog.logS(FATAL, "Logging at DEBUG level...");
    llog.setLogLevel(DEBUG);
    std::cout << "Logging at DEBUG level..." << std::endl;
    llog.logS(DEBUG, "DEBUG log message");
    llog.logS(INFO, "INFO log message");
    llog.logS(WARN, "WARN log message");
    llog.logS(ERROR, "ERROR log message");
    llog.logS(FATAL, "FATAL log message");

    // Set log level to INFO
    llog.logS(FATAL, "Logging at INFO level...");
    llog.setLogLevel(INFO);
    std::cout << "Logging at INFO level..." << std::endl;
    llog.logS(DEBUG, "DEBUG log message - this should not be here");
    llog.logS(INFO, "INFO log message");
    llog.logS(WARN, "WARN log message");
    llog.logS(ERROR, "ERROR log message");
    llog.logS(FATAL, "FATAL log message");

    // Set log level to WARN
    llog.logS(FATAL, "Logging at WARN level...");
    llog.setLogLevel(WARN);
    std::cout << "Logging at WARN level..." << std::endl;
    llog.logS(DEBUG, "DEBUG log message - this should not be here");
    llog.logS(INFO, "INFO log message - this should not be here");
    llog.logS(WARN, "WARN log message");
    llog.logS(ERROR, "ERROR log message");
    llog.logS(FATAL, "FATAL log message");

    // Set log level to ERROR
    llog.logS(FATAL, "Logging at ERROR level...");
    llog.setLogLevel(ERROR);
    std::cout << "Logging at ERROR level..." << std::endl;
    llog.logS(DEBUG, "DEBUG log message - this should not be here");
    llog.logS(INFO, "INFO log message - this should not be here");
    llog.logS(WARN, "WARN log message - this should not be here");
    llog.logS(ERROR, "ERROR log message");
    llog.logS(FATAL, "FATAL log message");

    // Set log level to FATAL
    llog.logS(FATAL, "Logging at FATAL level...");
    llog.setLogLevel(FATAL);
    std::cout << "Logging at FATAL level..." << std::endl;
    llog.logS(DEBUG, "DEBUG log message - this should not be here");
    llog.logS(INFO, "INFO log message - this should not be here");
    llog.logS(WARN, "WARN log message - this should not be here");
    llog.logS(ERROR, "ERROR log message - this should not be here");
    llog.logS(FATAL, "FATAL log message");

    std::cout << "Log level filtering tests completed." << std::endl;
}

void messageFormattingTest()
{
    std::cout << "Testing message formatting..." << std::endl;

    // Set log level to DEBUG (lowest level)
    llog.setLogLevel(DEBUG);

    // Log some messages and verify their formatting
    llog.logS(DEBUG, "This is a DEBUG message with formatting.");
    llog.logS(INFO, "This is an INFO message with formatting.");
    llog.logS(WARN, "This is a WARN message with formatting.");
    llog.logS(ERROR, "This is an ERROR message with formatting.");
    llog.logS(FATAL, "This is a FATAL message with formatting.");

    std::cout << "Message formatting test completed." << std::endl;
}

void crushTestViaLog()
{
    std::cout << "Testing crush function via log..." << std::endl;

    // Test case 1: String with leading and trailing spaces
    std::string testStr1 = "   Hello World   ";
    llog.logS(INFO, "Test 1: String with leading and trailing spaces");
    llog.logS(INFO, testStr1); // This should automatically be cleaned by crush

    // Test case 2: String with multiple spaces between words
    std::string testStr2 = "Hello    World";
    llog.logS(INFO, "Test 2: String with multiple spaces between words");
    llog.logS(INFO, testStr2); // This should automatically be cleaned by crush

    // Test case 3: String with spaces before punctuation
    std::string testStr3 = "Hello  , World";
    llog.logS(INFO, "Test 3: String with spaces before punctuation");
    llog.logS(INFO, testStr3); // This should automatically be cleaned by crush

    // Test case 4: String with spaces inside parentheses
    std::string testStr4 = "Hello (   World   )";
    llog.logS(INFO, "Test 4: String with spaces inside parentheses");
    llog.logS(INFO, testStr4); // This should automatically be cleaned by crush

    // Test case 5: String with multiple spaces and tabs
    std::string testStr5 = "   This    is   \t\ttest   ";
    llog.logS(INFO, "Test 5: String with multiple spaces and tabs");
    llog.logS(INFO, testStr5); // This should automatically be cleaned by crush

    // Test case 6: Empty string
    std::string testStr6 = "";
    llog.logS(INFO, "Test 6: Empty string");
    llog.logS(INFO, testStr6); // This should be an empty line in the logs

    // Test case 7: String with only one word
    std::string testStr7 = "   Hello   ";
    llog.logS(INFO, "Test 7: String with only one word");
    llog.logS(INFO, testStr7); // This should automatically be cleaned by crush

    std::cout << "crush function via log test completed." << std::endl;
}

void multiLineLogTest()
{
    std::cout << "Testing multiline function via log..." << std::endl;

    // Test case 1: String with leading and trailing spaces
    std::string testStr1 = "Line 1\nLine 2";
    llog.logS(INFO, "Testing: String with a linefeed");
    llog.logS(INFO, testStr1); // This should automatically be split

    std::cout << "Line split function via log test completed." << std::endl;
}

void longTest()
{
    llog.enableTimestamps(false);
    llog.setLogLevel(DEBUG);
    std::cout << std::endl
              << "Testing stdout." << std::endl;
    llog.logS(INFO, 100);
    llog.logS(INFO, 100.01);
    llog.logS(INFO, "\t\t\t\t\t\tFoo");
    llog.logS(INFO, "Foo", " foo foo.");
    llog.logS(INFO, "Foo", " \t\t\t\t\t\t\t\t\tfoo foo.");
    llog.logS(INFO, "Foo ", 100, " \t\t\t\t\t\t\t\t\tfoo foo.");
    llog.logS(INFO, "Multiline ", 100.01, " \nNew line.");

    llog.enableTimestamps(true);
    std::cout << std::endl
              << "Testing stdout with timestamps." << std::endl;
    llog.logS(INFO, 100);
    llog.logS(INFO, 100.01);
    llog.logS(INFO, "\t\t\t\t\t\tFoo");
    llog.logS(INFO, "Foo", " foo foo.");
    llog.logS(INFO, "Foo", " \t\t\t\t\t\t\t\t\tfoo foo.");
    llog.logS(INFO, "Foo ", 100, " \t\t\t\t\t\t\t\t\tfoo foo.");
    llog.logS(INFO, "Multiline ", 100.01, " \nNew line.");

    llog.enableTimestamps(false);
    std::cerr << std::endl
              << "Testing stderr." << std::endl;
    llog.logE(INFO, 100);
    llog.logE(INFO, 100.01);
    llog.logE(INFO, "\t\t\t\t\t\tFoo");
    llog.logE(INFO, "Foo", " foo foo.");
    llog.logE(INFO, "Foo", " \t\t\t\t\t\t\t\t\tfoo foo.");
    llog.logE(INFO, "Foo ", 100, " \t\t\t\t\t\t\t\t\tfoo foo.");
    llog.logE(INFO, "Multiline ", 100.01, " \nNew line.");

    llog.enableTimestamps(true);
    std::cerr << std::endl
              << "Testing stderr with timestamps." << std::endl;
    llog.logE(INFO, 100);
    llog.logE(INFO, 100.01);
    llog.logE(INFO, "\t\t\t\t\t\tFoo");
    llog.logE(INFO, "Foo", " foo foo.");
    llog.logE(INFO, "Foo", " \t\t\t\t\t\t\t\t\tfoo foo.");
    llog.logE(INFO, "Foo ", 100, " \t\t\t\t\t\t\t\t\tfoo foo.");
    llog.logE(INFO, "Multiline ", 100.01, " \nNew line.");
}

void testShouldSkipSpace()
{
    std::cout << "Running shouldSkipSpace() tests...\n";

    // Test cases for punctuation handling
    assert(shouldSkipSpace("Word", ".") == true); // No space before `.`
    assert(shouldSkipSpace("Word", ",") == true); // No space before `,`
    assert(shouldSkipSpace("Word", "!") == true); // No space before `!`
    assert(shouldSkipSpace("Word", ";") == true); // No space before `;`

    assert(shouldSkipSpace(":", "Word") == false); // Space after `:`
    assert(shouldSkipSpace(".", "Word") == false); // Space after `.`
    assert(shouldSkipSpace(",", "Word") == false); // Space after `,`
    assert(shouldSkipSpace(";", "Word") == false); // Space after `;`

    // Test with mixed types
    assert(shouldSkipSpace(42, "Word") == false);   // int should not break it
    assert(shouldSkipSpace(3.14, "Word") == false); // double should not break it
    assert(shouldSkipSpace("Word", 100) == false);  // No space impact for int
    assert(shouldSkipSpace("Word", 2.71) == false); // No space impact for double

    // Edge cases
    assert(shouldSkipSpace("", ".") == true); // Empty string before `.`
    assert(shouldSkipSpace("", ",") == true); // Empty string before `,`
    assert(shouldSkipSpace("", "!") == true); // Empty string before `!`
    assert(shouldSkipSpace("", ";") == true); // Empty string before `;`

    assert(shouldSkipSpace("", "Word") == false); // Empty before word (allow space)
    assert(shouldSkipSpace(":", "") == false);    // Space after `:` even if empty

    std::cout << "All shouldSkipSpace() tests passed successfully!\n";
}

void testMixedDataTypes()
{
    std::cout << "Running testMixedDataTypes()...\n";

    llog.logS(INFO, "Test start:", 42, "is an int,", 3.14159, "is Pi,", -7.25,
              "is negative,", "true", "is a bool,", nullptr, "is null,",
              "this", "should", "have", "spaces.", "(Parentheses)",
              "[Brackets]", "{Braces}", "\"Quotes\"", "'Single quotes'",
              "Comma,", "period.", "exclamation!", "question?", "colon:",
              "semicolon;", "hyphen-", "underscore_", "slash/", "backslash\\",
              "percent%", "ampersand&", "asterisk*", "at@", "hash#", "dollar$",
              "caret^", "pipe|", "tilde~", "backtick`");

    llog.logS(INFO, "Transmission completed,", "(", 0.000, "sec", ")");

    std::cout << "Completed testMixedDataTypes(). Check log output.\n";
}

void testParentheses()
{
    llog.setLogLevel(DEBUG);
    llog.logS(INFO, "Testing1", "(", 0.0, ")");
    llog.logS(INFO, "Testing2", "(", 0.0, ").");
    llog.logS(INFO, "Testing3 (", 3.1415, ")");
}

int main()
{
    llog.enableTimestamps(true);
    // threadSafetyTest();
    // logToDifferentStreamsTest();
    // crushTestViaLog();
    // multiLineLogTest();
    // messageFormattingTest();
    // runLogLevelFilteringTests();
    // longTest();
    // testShouldSkipSpace();
    // testMixedDataTypes();
    testParentheses();
    return 0;
}
