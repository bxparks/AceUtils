/*
MIT License

Copyright (c) 2018 Brian T. Park

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ACE_UTILS_CLI_COMMAND_HANDLER_H
#define ACE_UTILS_CLI_COMMAND_HANDLER_H

#include <AceCommon.h> // FCString

class Print;

/**
 * Increment argv and decrement argc so that it appears as if the command line
 * arguments are shifted to the *left* by one position. This is useful for
 * processing the command line tokens one at a time. Use
 * CommandHandler::isArgEqual() method to test for match against an expected
 * token.
 */
#define SHIFT_ARGC_ARGV(argc, argv) do { argc--; argv++; } while (false)

namespace ace_utils {
namespace cli {

/**
 * Signature for a command handler.
 */
class CommandHandler {
  public:
   /**
    * Run the command.
    *
    * @param printer The output printer, normally Serial. This is not
    *        expected to change during the lifetime of the CommandHandler
    *        instance, so normally I would inject this parameter into the
    *        object. However, that would cause each instance to consume 2 more
    *        bytes, even though all instances are expected to have the same
    *        'printer'. For an 8-bit AVR processor with only 2kB of RAM, every
    *        byte is important , so I am keeping this as a parameter passed
    *        into the run() method.
    * @param argc Number of tokens in the input command, including the name of
    *        the command itself.
    * @param argv An array of strings for each token.
    */
    virtual void run(Print& printer, int argc, const char* const* argv)
        const = 0;

    /** Return the name of the command. */
    ace_common::FCString getName() const { return mName; }

    /** Return the help string of the command. */
    ace_common::FCString getHelpString() const { return mHelpString; }

  protected:
    /** Test for equality against token. */
    static bool isArgEqual(const char* arg, const char* token) {
      return strcmp(arg, token) == 0;
    }

    /** Test for equality when token is in PROGMEM. */
    static bool isArgEqual(const char* arg, const __FlashStringHelper* token) {
      return strcmp_P(arg, (const char*) token) == 0;
    }

    /** Constructor. Using C strings. */
    CommandHandler(const char* name, const char* helpString):
      mName(name),
      mHelpString(helpString) {}

    /** Constructor. Using Flash strings. */
    CommandHandler(const __FlashStringHelper* name,
        const __FlashStringHelper* helpString):
      mName(name),
      mHelpString(helpString) {}

  private:
    ace_common::FCString const mName;
    ace_common::FCString const mHelpString;
};

} // cli
} // ace_utils

#endif
